#include "broker.h"

void inicializacion_cache(void){
	// Leer config y parsear valores.

	tamano_memoria = config_get_int_value(config, "TAMANO_MEMORIA");
	tamano_minimo_particion = config_get_int_value(config, "TAMANO_MINIMO_PARTICION");

	algoritmo_memoria = de_string_a_alg_memoria(
			config_get_string_value(config, "ALGORITMO_MEMORIA")
	);
	algoritmo_remplazo = de_string_a_alg_remplazo(
		config_get_string_value(config, "ALGORITMO_REEMPLAZO")
	);
	algoritmo_particion_libre = de_string_a_alg_particion_libre(
		config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE")
	);

	frecuencia_compactacion = config_get_int_value(config, "FRECUENCIA_COMPACTACION");

	log_trace(logger, "Se leyo de config: tamano_memoria: %i, tamano_minimo_particion: %i, alg_memoria: %i, alg_remplazo: %i, alg_part_libre: %i, frec_compact:  %i.",
				tamano_memoria, tamano_minimo_particion, algoritmo_memoria, algoritmo_remplazo, algoritmo_particion_libre, frecuencia_compactacion);

	// Inicializar memoria cache

	memoria_cache = malloc(tamano_memoria);

	// Inicializar memoria administrativa

	t_mensaje_cache* primer_particion = malloc(sizeof(t_mensaje_cache));
	primer_particion -> tipo_mensaje = VACIO;
	primer_particion -> offset = 0;
	primer_particion -> tamanio = tamano_memoria;

	struct_admin_cache = list_create();
	list_add(struct_admin_cache, primer_particion);

	// Inicializar LRU flag

	actual_lru_flag = 0;

	//Inicializar semaforos



}

void cachear_mensaje(int size_stream, int id_mensaje,int tipo_mensaje, void* mensaje_a_cachear){
	//De aca en adelante se puede generalizar para todos los mensajes, no solo appeared.
		//TODO: Pasar a cachear_mensaje() en broker_general
		int tamano_a_cachear = ((size_stream >= tamano_minimo_particion)? size_stream : tamano_minimo_particion);
		_Bool no_se_agrego_mensaje_a_cache = true;


		log_trace(logger, "Tamano de mensaje a cachear: %i (size stream: %i).", tamano_a_cachear, size_stream);
		while (no_se_agrego_mensaje_a_cache) { // Se repite hasta que el mensaje este en cache.

			_Bool ordenar_para_rellenar_aux(void* mensaje_1_aux, void* mensaje_2_aux) {
				t_mensaje_cache* mensaje_1 = (t_mensaje_cache*) mensaje_1_aux;
				t_mensaje_cache* mensaje_2 = (t_mensaje_cache*) mensaje_2_aux;


				//return mensaje_1->tipo_mensaje==VACIO;

				return ordenar_para_rellenar(mensaje_1,mensaje_2,size_stream);
			}
			// Buscar particion a llenar
			list_sort(struct_admin_cache, ordenar_para_rellenar_aux);
				// Se ordenan las particiones, tanto ocupadas como desocupadas, dejando adelante las libres y con tamano suficiente
				// y dejando primera la que se debe remplazar.En FirstFit, se deja primero la de offset menor, y en BestFit la de tamano menor. (Switch)

			log_warning(logger, "Va a hacer dump");
			log_dump_de_cache();

			//no_se_agrego_mensaje_a_cache = false;

			if( particion_valida_para_llenar( list_get (struct_admin_cache, 0) , tamano_a_cachear)) {
				log_trace(logger, "La particion elegida tiene tamano suficiente (%ib) para el mensaje(%ib).",
						((t_mensaje_cache*)list_get (struct_admin_cache, 0))->tamanio, tamano_a_cachear);

				// Este if va a dejar llenar la particion si la que quedo primera, que deberia ser la mas cercana a poder ser llenada, efectivamente lo es.
				// Si esta no tiene tamano suficiente, significa que ninguna lo tiene, entonces habra que eliminar una particion (else)

				t_mensaje_cache* particion_mensaje = crear_particion_mensaje(tipo_mensaje ,id_mensaje ,tamano_a_cachear , list_get (struct_admin_cache, 0) );
						//Se crea la particion llena con el contenido del mensaje, en base a la info de la particion vacia elegida.
				list_add(struct_admin_cache, particion_mensaje);

				log_trace(logger, "Se creo particion.");
				// Al final de la estructura administrativa agrego un elemento que referencia el mensaje por agregar.
				log_trace(logger, "Se agrego la particion con el mensaje.");

				log_dump_de_cache();


				if( queda_espacio_libre( tamano_a_cachear, list_get (struct_admin_cache, 0) ) ){
					log_trace(logger, "Se agregara la particion sobrante.");

					t_mensaje_cache* particion_sobrante = crear_particion_sobrante(tamano_a_cachear, list_get (struct_admin_cache, 0));
					list_add(struct_admin_cache, particion_sobrante);
					// Si el mensaje deja espacio suficiente como para generar una nueva particion libre, esta tambien se agrega al final
					// de la estructura administrativa.
					log_trace(logger, "Se agrego la particion sobrante.");

					log_dump_de_cache();
				}


				borrar_particiones_del_inicio(1);
				log_trace(logger, "Se borro la particion vacia antigua, y fue replazada por la ocupada y su sobrante.");
				// Ahora borro el primer elemento de la estructura administrativa. Es la particion libre elegida, que se llenara total o parcialmente,
				// pero en ambos casos su informacion ya esta contemplada por los elementos que acabo de agregar.
				log_dump_de_cache();

				agregar_mensaje_a_cache(mensaje_a_cachear,size_stream , particion_mensaje);
				log_trace(logger, "Se agrego mensaje a la cache.");
				// TODO, sacar deserializar y loggear.
				// Se agrega el mensaje a cachear al malloc de la cache con el offset que indica en la estructura administrativa.

				list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria); // Se reordena la estructura administrativa.
				log_trace(logger, "Se dejo la estructura ordenada apropiadamente.");

				log_dump_de_cache();

				no_se_agrego_mensaje_a_cache = false; //Para que salga del while.
			}else{

				log_trace(logger, "No hay lugar");

				elegir_vitima_y_eliminarla(); // Y consolido
				//compactar_cache_si_corresponde();

				//no_se_agrego_mensaje_a_cache = false;
			}
		}
}

int de_string_a_alg_memoria(char* string){
	if(string_equals_ignore_case(string , "PARTICIONES")) 	{
		return PARTICIONES;
	} else if(string_equals_ignore_case(string , "BS")) 	{
		return BS;
	} else 													{
		return -1;
	}
}
int de_string_a_alg_remplazo(char* string){
	if(string_equals_ignore_case(string , "FIFO")) 			{
		return FIFO;
	} else if(string_equals_ignore_case(string , "LRU")) 	{
		return LRU;
	} else 													{
		return -1;
	}
}
int de_string_a_alg_particion_libre(char* string){
	if(string_equals_ignore_case(string , "FF")) 			{
		return FF;
	} else if(string_equals_ignore_case(string , "BF")) 	{
		return BF;
	} else 													{
		return -1;
	}
}

_Bool ordenar_para_rellenar(t_mensaje_cache* mensaje_1, t_mensaje_cache* mensaje_2, int tamano_mensaje){
	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista

	if(mensaje_1->tipo_mensaje == 0 && mensaje_2->tipo_mensaje != 0) return true; // Si el 1 esta libre y el 2 no => el 1 va primero
	if(mensaje_2->tipo_mensaje == 0 && mensaje_1->tipo_mensaje != 0) return false; // Si el 2 esta libre y el 1 no => el 2 va primero
	if(mensaje_1->tipo_mensaje != 0 && mensaje_1->tipo_mensaje != 0) return true; // Si ambos estan ocupados => Me da lo mismo el orden, tiro true para que queden como estan
	if(mensaje_1->tipo_mensaje == 0 && mensaje_2->tipo_mensaje == 0){ // Si ambos estan vacios => Sigo evaluando

		if( mensaje_1->tamanio >= tamano_mensaje && mensaje_2->tamanio < tamano_mensaje) return true; // Si el 1 tiene tamano suficiente y el 2 no => el 1 va primero
		if( mensaje_2->tamanio >= tamano_mensaje && mensaje_1->tamanio < tamano_mensaje) return false; // Si el 2 tiene tamano suficiente y el 1 no => el 2 va primero
		if( mensaje_1->tamanio < tamano_mensaje && mensaje_2->tamanio < tamano_mensaje) return true; // Si ambos no tienen tamano suficiente => Me da lo mismo el orden
		if( mensaje_1->tamanio >= tamano_mensaje && mensaje_2->tamanio >= tamano_mensaje ) {

			switch(algoritmo_particion_libre){
				case FF:
					return (mensaje_1->offset <= mensaje_2->offset); //Si el mensaje 1 esta antes en memoria => El 1 va primero
					break;

				case BF:
					return (mensaje_1->tamanio - tamano_mensaje) <= (mensaje_2->tamanio - tamano_mensaje); // Si el espacio sobrante de 1 es menor que el de 2 => el 1 va primero.
					break;

			}
		}
	}
	return NULL;
}

_Bool particion_valida_para_llenar(t_mensaje_cache* particion, int tamano_a_ocupar){
	_Bool esta_vacia = (particion->tipo_mensaje == VACIO);

	int tamanio_minimo_sobrante =  (tamano_minimo_particion+tamano_a_ocupar);
	_Bool tamanio_justo = (particion->tamanio == tamano_a_ocupar);
	_Bool sobrante_suficiente = (particion->tamanio >=  tamanio_minimo_sobrante);

	_Bool tamano_suficiente = tamanio_justo || sobrante_suficiente;
	//Si no entra justo, la particion sobrante tiene que ser mayor a la minima.

	return tamano_suficiente && esta_vacia; //Esta vacia y con tamanio suficiente.
}

t_mensaje_cache* crear_particion_mensaje(int tipo_mensaje, int id_mensaje, int tamano_a_cachear, t_mensaje_cache* particion_vacia){
	t_mensaje_cache* particion_llena = malloc(sizeof(t_mensaje_cache));

	particion_llena->tipo_mensaje = tipo_mensaje;
	particion_llena->id = id_mensaje;
	particion_llena->offset = particion_vacia->offset;
	particion_llena->flags_lru=get_lru_flag();

	particion_llena->tamanio = tamano_a_cachear;

	particion_llena->subscribers_enviados = filtrar_subs_enviados( tipo_mensaje, id_mensaje);
	particion_llena->subscribers_recibidos = filtrar_subs_recibidos( tipo_mensaje, id_mensaje);

	return particion_llena;
}

int get_lru_flag() {
	int flag = actual_lru_flag;
	actual_lru_flag ++;
	return flag;
}

t_list* filtrar_subs_enviados(int tipo_mensaje, int id_mensaje){
	t_list* lista_subs = list_create();

	t_list* subs_queue = get_cola_segun_tipo(tipo_mensaje)-> subscriptores;

	// Inner function para filter
	_Bool tiene_mensaje_enviado(void* suscriptor_aux) {
		t_suscriptor_queue* suscriptor = (t_suscriptor_queue*) suscriptor_aux;

		// Meta-inner function para any_satisfy
		_Bool tiene_mensaje(void* un_id) {
				return ((int) un_id == id_mensaje);
			}

		return list_any_satisfy(suscriptor->mensajes_enviados, tiene_mensaje);
	}

	// Inner function para map
	int get_socket_suscriptor(void* suscriptor_aux) {
		t_suscriptor_queue* suscriptor = (t_suscriptor_queue*) suscriptor_aux;

		return suscriptor->socket;
	}


	lista_subs = list_map( list_filter(subs_queue, tiene_mensaje_enviado), (void*) get_socket_suscriptor);

	return lista_subs;
}

t_list* filtrar_subs_recibidos(int tipo_mensaje, int id_mensaje){
	t_list* lista_subs = list_create();

	t_list* subs_queue = get_cola_segun_tipo(tipo_mensaje)-> subscriptores;

	// Inner function para filter
	_Bool tiene_mensaje_recibido(void* suscriptor_aux) {
		t_suscriptor_queue* suscriptor = (t_suscriptor_queue*) suscriptor_aux;

		// Meta-inner function para any_satisfy
		_Bool tiene_mensaje(void* un_id) {
				return ((int) un_id == id_mensaje);
			}

		return list_any_satisfy(suscriptor->mensajes_recibidos, tiene_mensaje);
	}

	// Inner function para map
	int get_socket_suscriptor(void* suscriptor_aux) {
		t_suscriptor_queue* suscriptor = (t_suscriptor_queue*) suscriptor_aux;

		return suscriptor->socket;
	}


	lista_subs = list_map( list_filter(subs_queue, tiene_mensaje_recibido), (void*) get_socket_suscriptor);

	return lista_subs;
}

_Bool queda_espacio_libre(int tamano_mensaje_a_cachear, t_mensaje_cache* particion_vacia) {
	return particion_vacia->tamanio > tamano_mensaje_a_cachear;
}

t_mensaje_cache* crear_particion_sobrante(int tamanio_mensaje_cacheado, t_mensaje_cache* particion_vacia){
	t_mensaje_cache* particion_sobrante = malloc(sizeof(t_mensaje_cache));

		particion_sobrante->tipo_mensaje = 0;
		particion_sobrante->id = -20;
		particion_sobrante->offset = (particion_vacia->offset) + tamanio_mensaje_cacheado;
		particion_sobrante->tamanio = (particion_vacia->tamanio) - tamanio_mensaje_cacheado;
		particion_sobrante->flags_lru = -20;

	return particion_sobrante;
}

void liverar_t_mensaje_cache(void* mensaje){
	list_destroy_and_destroy_elements(((t_mensaje_cache*) mensaje)->subscribers_enviados, free);
	list_destroy_and_destroy_elements(((t_mensaje_cache*) mensaje)->subscribers_recibidos, free);
	free(mensaje);
}

void agregar_mensaje_a_cache(void* mensaje_a_cachear,int tamano_stream, t_mensaje_cache* particion_mensaje){
	memcpy( mensaje_a_cachear+(particion_mensaje->offset) , mensaje_a_cachear, tamano_stream);
}

_Bool ordenar_segun_su_lugar_en_memoria(void* mensaje_1, void* mensaje_2){
	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista
	return ( ((t_mensaje_cache*)mensaje_1)->offset ) < ( ((t_mensaje_cache*)mensaje_2)->offset );
}

void elegir_vitima_y_eliminarla() {

	list_sort(struct_admin_cache, ordenar_segun_lru_flag); //Dejo primero al que quiero borrar
	log_trace(logger, "Cache ordenada por LRU flag.");

	log_dump_de_cache();

	int id_victima = ( (t_mensaje_cache*)list_get(struct_admin_cache, 0) ) -> id;
	log_trace(logger, "El id de la victima elegida es: %i.", id_victima);

	log_dump_de_cache();

	void vaciar_una_particion(void* particion){
		if(((t_mensaje_cache*) particion)->id == id_victima) // Si es victima
			vaciar_particion((t_mensaje_cache*) particion);
	}
	list_iterate(struct_admin_cache, vaciar_una_particion);
	log_trace(logger, "Particion victima vacia.");

	log_dump_de_cache();

	consolidar_cache(); //La victima quedo primera
	log_trace(logger, "Consolidacion terminada.");

	log_dump_de_cache();

	list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria);
	log_trace(logger, "Cache ordenada por offset.");

	log_dump_de_cache();

}

_Bool ordenar_segun_lru_flag(void* mensaje_1_aux, void* mensaje_2_aux){
	t_mensaje_cache* mensaje_1 = (t_mensaje_cache*) mensaje_1_aux;
	t_mensaje_cache* mensaje_2 = (t_mensaje_cache*) mensaje_2_aux;

	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista
	_Bool rtn = (mensaje_1->tipo_mensaje == 0)? false : ( (mensaje_1->flags_lru) < (mensaje_2->flags_lru) );
	// Si la particion esta vacia, va al fondo, si esta llena se ordenan de menor a mayor valos de flag_lru
	return rtn;
}

void vaciar_particion(t_mensaje_cache* particion){
	particion->tipo_mensaje = VACIO;
	particion->flags_lru=-20;
	particion->id=-20;
	list_clean_and_destroy_elements(particion->subscribers_enviados, free);
	list_clean_and_destroy_elements(particion->subscribers_recibidos, free);
}

void consolidar_cache(){
	//IMPORTANTE: la reciente victima quedo primera en la struct_admin_cache
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);
	log_debug(logger, "Victima: offset %i, tamanio %i.", victima->offset, victima->tamanio);

	if(siguiente_es_vacio() && (!anterior_es_vacio() || es_primera_part()) ){ //Se consolida con el siguiente
		log_trace(logger, "Se consolida con el siguiente.");
		ordeno_dejando_victima_y_siguiente_adelante();
		agregar_particion_segun_vicima_y_siguiente();
		borrar_particiones_del_inicio(2);// borrar victima y siguiente
	}else if ((!siguiente_es_vacio() || es_ultima_part()) && anterior_es_vacio()){ //Se consolida con el anterior
		log_trace(logger, "Se consolida con el anterior.");
		ordeno_dejando_anterior_y_victima_adelante();
		agregar_particion_segun_anterior_y_victima();
		borrar_particiones_del_inicio(2);// borrar victima y anterior
	}else if (siguiente_es_vacio() && anterior_es_vacio() && !es_primera_part() && !es_ultima_part() ){
		log_trace(logger, "Se consolida con el siguiente y con el anterior.");
		ordeno_dejando_anterior_victima_y_siguiente_adelante();
		agregar_particion_segun_anterior_victima_y_siguiente();
		borrar_particiones_del_inicio(3);// borro victima, anterior y siguiente
	}else{
		log_trace(logger, "No se consolida");
	}

}

_Bool siguiente_es_vacio() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool es_siguiente_y_es_vacio(void* posible_siguinte){

		_Bool es_siguiente = ((t_mensaje_cache*)posible_siguinte)-> offset == ( victima->offset + victima->tamanio );
		_Bool es_vacio =((t_mensaje_cache*)posible_siguinte)-> tipo_mensaje == VACIO;

		return es_siguiente && es_vacio;
	}

	return list_any_satisfy(struct_admin_cache, es_siguiente_y_es_vacio);
}

_Bool anterior_es_vacio() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool es_anterior_y_es_vacio(void* posible_anterior){

		_Bool es_anterior = victima->offset == ( ((t_mensaje_cache*)posible_anterior)->offset + ((t_mensaje_cache*)posible_anterior)->tamanio );
		_Bool es_vacio =((t_mensaje_cache*)posible_anterior)-> tipo_mensaje == VACIO;

		return es_anterior && es_vacio;
	}

	return list_any_satisfy(struct_admin_cache, es_anterior_y_es_vacio);
}

_Bool es_primera_part() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	return victima->offset == 0;
}

_Bool es_ultima_part() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);
	log_warning(logger, "Victima: offset %i, tamanio %i. Tamanio memoria: tamano_memoria.", victima->offset, victima->tamanio);
	//return (victima->offset + victima->tamanio) == tamano_memoria;
	return false;
}

void borrar_particiones_del_inicio(int cant_particiones_a_borrar){
	for (int var = 0; var < cant_particiones_a_borrar; ++var) {
		list_remove_and_destroy_element(struct_admin_cache, 0, free );
		//list_remove_and_destroy_element(struct_admin_cache, 0, liverar_t_mensaje_cache ); //Da seg fault, revisar

	}
}

void agrego_part_vacia(int offset, int tamanio) {
	t_mensaje_cache* nueva_particion_vacia = malloc(sizeof(t_mensaje_cache));

		nueva_particion_vacia->flags_lru=-20;
		nueva_particion_vacia->id=-20;
		nueva_particion_vacia->tipo_mensaje = VACIO;

		nueva_particion_vacia->offset = offset;
		nueva_particion_vacia->tamanio = tamanio;

		list_add(struct_admin_cache, (void*) nueva_particion_vacia);
}

void ordeno_dejando_victima_y_siguiente_adelante(){
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool victima_y_siguiente_adelante(void* particion_1, void* particion_2) {
		_Bool es_victima_1 = es_victima(particion_1, victima);
		_Bool es_siguiente_1 = es_siguiente(particion_1, victima);
		_Bool es_victima_2 = es_victima(particion_2, victima);

		return es_victima_1 || (es_siguiente_1 && !es_victima_2);
	}

	list_sort(struct_admin_cache, victima_y_siguiente_adelante);
}

void agregar_particion_segun_vicima_y_siguiente(){
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);
	t_mensaje_cache* siguiente = list_get(struct_admin_cache, 1);

	agrego_part_vacia(victima->offset, victima->tamanio + siguiente->tamanio);
}

void ordeno_dejando_anterior_y_victima_adelante(){
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool anterior_y_victima_adelante(void* particion_1, void* particion_2) {
		_Bool es_victima_1 = es_victima(particion_1, victima);
		_Bool es_anterior_1 = es_anterior(particion_1, victima);
		_Bool es_anterior_2 = es_anterior(particion_2, victima);

		return  (es_anterior_1) || (es_victima_1 && !es_anterior_2);
	}

	list_sort(struct_admin_cache, anterior_y_victima_adelante);
}

void agregar_particion_segun_anterior_y_victima() {
	t_mensaje_cache* anterior = list_get(struct_admin_cache, 0);
	t_mensaje_cache* victima = list_get(struct_admin_cache, 1);

	agrego_part_vacia(anterior->offset, anterior->tamanio + victima->tamanio);
}

void ordeno_dejando_anterior_victima_y_siguiente_adelante(){
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool anterior_victima_y_siguiente_adelante(void* particion_1, void* particion_2) {
		_Bool es_victima_1 = es_victima(particion_1, victima);
		_Bool es_anterior_1 = es_anterior(particion_1, victima);
		_Bool es_siguiente_1 = es_siguiente(particion_1, victima);
		_Bool es_anterior_2 = es_anterior(particion_2, victima);
		_Bool es_victima_2 = es_victima(particion_2, victima);

		return  (es_anterior_1) || (es_victima_1 && !es_anterior_2) || (es_siguiente_1 && !es_anterior_2 && !es_victima_2);
	}

	list_sort(struct_admin_cache, anterior_victima_y_siguiente_adelante);
}

void agregar_particion_segun_anterior_victima_y_siguiente(){
	t_mensaje_cache* anterior = list_get(struct_admin_cache, 0);
	t_mensaje_cache* victima = list_get(struct_admin_cache, 1);
	t_mensaje_cache* siguiente = list_get(struct_admin_cache, 2);

	agrego_part_vacia(anterior->offset, anterior->tamanio + victima->tamanio + siguiente->tamanio);
}

_Bool es_siguiente(void* particion, t_mensaje_cache* victima){
	return ((t_mensaje_cache*)particion)-> offset == ( victima->offset + victima->tamanio );
}
_Bool es_anterior(void* particion, t_mensaje_cache* victima){
	return ( ((t_mensaje_cache*)particion)-> offset + ((t_mensaje_cache*)particion)->tamanio ) == victima->offset;
}
_Bool es_victima(void* particion, t_mensaje_cache* victima){
	return ((t_mensaje_cache*) particion)->offset == victima->offset;;
}

void log_dump_de_cache(){
	int num_particion = 1;
	log_debug(logger, "--------------------------------------------------------------------------------------------------");
	//log_header_dump(); //TODO Por alguna razon se rompe despues de usarlo un par de veces en ejecucion
	log_debug(logger, "Dump: ");


	void log_linea_dump_cache(void* particion) {
		log_info_particion(particion, num_particion);
		num_particion++;
	}
	list_iterate(struct_admin_cache, log_linea_dump_cache);

	log_debug(logger, "--------------------------------------------------------------------------------------------------");
}

void log_info_particion(t_mensaje_cache* particion, int num_part){
	char* string = malloc(sizeof(char)*100);

	int direc_inicio = ((int)memoria_cache) + particion->offset;
	int direc_final = ((int)memoria_cache) + particion->offset + particion->tamanio;
	char* libre_o_ocupado = (particion->tipo_mensaje == VACIO) ? "[L]" : "[X]" ;
	int tamano = particion->tamanio;

	if(particion->tipo_mensaje == VACIO){
		log_debug(logger, "Particion %i: 0x%X - 0x%X.    [L]    Size:%ib    Offset:%i",
				num_part, direc_inicio, direc_final, tamano, particion->offset);
	} else {
		log_debug(logger, "Particion %i: 0x%X - 0x%X.    [X]    Size:%ib    LRU:%i    Cola:%i    ID:%i    Offset:%i ",
				num_part, direc_inicio, direc_final, tamano,
				particion->flags_lru, particion->tipo_mensaje, particion->id, particion->offset);
	}

}

void log_header_dump() {
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);

	log_debug(logger, "Dump: %i/%i/%i %i:%i:%i",
			local->tm_mday, local->tm_mon, local->tm_year,
			local->tm_hour, local->tm_min, local->tm_sec);


}

void test(){

	//Creo un mensaje random para probar dump

	/*
	t_mensaje_cache* particion_llena = malloc(sizeof(t_mensaje_cache));
	particion_llena->tipo_mensaje = 2;
	particion_llena->id = 1000;
	particion_llena->offset = 40;
	particion_llena->flags_lru= get_lru_flag();
	particion_llena->tamanio = 30;
	list_add(struct_admin_cache, particion_llena);

	particion_llena = malloc(sizeof(t_mensaje_cache));
	*/
	//t_mensaje_cache* particion_llena= crear_particion_mensaje(2 ,10 ,40 , list_get (struct_admin_cache, 0) );

	//list_add(struct_admin_cache, particion_llena);


	log_dump_de_cache();

	t_appeared_pokemon* mensaje1 = crear_appeared_pokemon("Pikachuuuuuuuuuuuuuuuuuuuuu",3,5,8);

	cachear_appeared_pokemon(mensaje1);

	//log_dump_de_cache();

	log_trace(logger, "Se termino el test");
}





