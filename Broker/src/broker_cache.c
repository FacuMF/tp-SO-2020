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

	// Inizializar memoria administrativa

	t_mensaje_cache* primer_particion = malloc(sizeof(t_mensaje_cache));
	primer_particion -> tipo_mensaje = VACIO;
	primer_particion -> offset = 0;
	primer_particion -> tamanio = tamano_memoria;

	struct_admin_cache = list_create();
	list_add(struct_admin_cache, primer_particion);

	// Inizializar LRU flag

	actual_lru_flag = 0;

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

	if(mensaje_1->id == 0 && mensaje_2->id != 0) return true; // Si el 1 esta libre y el 2 no => el 1 va primero
	if(mensaje_2->id == 0 && mensaje_1->id != 0) return false; // Si el 2 esta libre y el 1 no => el 2 va primero
	if(mensaje_1->id != 0 && mensaje_1->id != 0) return true; // Si ambos estan ocupados => Me da lo mismo el orden, tiro true para que queden como estan
	if(mensaje_1->id == 0 && mensaje_2->id == 0){ // Si ambos estan vacios => Sigo evaluando

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
	_Bool esta_vacia = (particion->id == VACIO);

	_Bool tamano_suficiente = (particion->tamanio == tamano_a_ocupar) || (particion->tamanio >= (tamano_minimo_particion+tamano_a_ocupar) );
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

_Bool ordenar_segun_su_lugar_en_memoria(t_mensaje_cache* mensaje_1, t_mensaje_cache* mensaje_2){
	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista
	return (mensaje_1->offset) < (mensaje_2->offset);
}


void elegir_vitima_y_eliminarla() {

	list_sort(struct_admin_cache, ordenar_segun_lru_flag); //Dejo primero al que quiero borrar

	int id_victima = ( (t_mensaje_cache*)list_get(struct_admin_cache, 0) ) -> id;

	void vaciar_una_particion(void* particion){
		if(((t_mensaje_cache*) particion)->id == id_victima) // Si es victima
			vaciar_particion((t_mensaje_cache*) particion);
	}
	list_iterate(struct_admin_cache, vaciar_una_particion);

	consolidar_cache(); //La victima quedo primera

	list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria);


}

_Bool ordenar_segun_lru_flag(t_mensaje_cache* mensaje_1, t_mensaje_cache* mensaje_2){
	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista
	_Bool rtn = (mensaje_1->tipo_mensaje == 0)? false : ( (mensaje_1->flags_lru) < (mensaje_2->flags_lru) );
	// Si la particion esta vacia, va al fondo, si esta llena se ordenan de menor a mayor valos de flag_lru
	return rtn;
}

void vaciar_particion(t_mensaje_cache* particion){
	particion->flags_lru=-20;
	particion->id=-20;
	list_clean_and_destroy_elements(particion->subscribers_enviados, free);
	list_clean_and_destroy_elements(particion->subscribers_recibidos, free);
}

void consolidar_cache(){
	//IMPORTANTE: la reciente victima quedo primera en la struct_admin_cache
	if(siguiente_es_vacio && !anterior_es_vacio){
		ordeno_dejando_victima_y_siguiente_adelante();
		agregar_particion_segun_vicima_y_siguiente();
		borrar_particiones_del_inicio(2);// borrar victima y siguiente
	}else if (!siguiente_es_vacio && anterior_es_vacio){
		ordeno_dejando_anterior_y_victima_adelante();
		agregar_particion_segun_anterior_y_victima();
		borrar_particiones_del_inicio(2);// borrar victima y anterior
	}else if (siguiente_es_vacio && anterior_es_vacio){
		ordeno_dejando_anterior_victima_y_siguiente_adelante();
		agregar_particion_segun_anterior_victima_y_siguiente();
		borrar_particiones_del_inicio(3);// borro victima, anterior y siguiente
	}
	//else -> no consolido
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

	return list_any_satisfy(struct_admin_cache, es_siguiente_y_es_vacio);
}

void borrar_particiones_del_inicio(int cant_particiones_a_borrar){
	for (int var = 0; var < cant_particiones_a_borrar; ++var) {
		list_remove_and_destroy_element(struct_admin_cache, 0, liverar_t_mensaje_cache ); //Victima
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
_Bool es_vicitima(void* particion, t_mensaje_cache* victima){
	return ((t_mensaje_cache*) particion)->offset == victima->offset;;
}






// Serializacion para cache

/*void* serializar_cache_appeared_pokemon(t_appeared_pokemon* mensaje, int size){

	void* stream = malloc(size);
	int offset = 0;
	memcpy(stream + offset, &(mensaje->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy( stream + offset, (mensaje->pokemon), (mensaje->size_pokemon));
	offset += mensaje->size_pokemon;

	memcpy( stream + offset, &(mensaje->posx), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->posy), sizeof(uint32_t));
	offset += sizeof(uint32_t);


	return stream;
<<<<<<< HEAD
}*/



void log_dump_de_cache(){

	char* string = malloc(sizeof(char) * 100 * ( list_size(struct_admin_cache) + 1 ) );
	int num_particion = 1;

	void agregar_linea_dump_cache(void* particion) {
		strcpy(string, "Particion ");
		strcpy(string, string_itoa(num_particion));
		num_particion++;
		strcpy(string, ": ");

		strcpy(string, (t_mensaje_cache*) particion);

		strcpy(string, "\n");

	}

	list_iterate(struct_admin_cache, agregar_linea_dump_cache);
	log_info(logger, string); //TODO que no lo loguee, que lo ponga en un archivo creo
	free(string);
}

char* get_string_info_particion(t_mensaje_cache* particion){
	char* string;

	char* direc_inicio = string_itoa(memoria_cache + particion->offset);
	char* direc_final = string_itoa(memoria_cache + particion->offset + particion->tamanio);
	char* libre_o_ocupado = (particion->tipo_mensaje == VACIO) ? "[L]" : "[X]" ;
	char* tamano = string_itoa(particion->tamanio);

	strcpy(string, direc_inicio);
	strcpy(string, " - ");
	strcpy(string, direc_final);
	strcpy(string, ".    ");
	strcpy(string, libre_o_ocupado);
	strcpy(string, "    ");
	strcpy(string, "Size: ");
	strcpy(string, tamano);
	strcpy(string, "b");

	if(particion->tipo_mensaje == 0){
		char* lru = string_itoa(particion->flags_lru);
		char* cola = string_itoa(particion->tipo_mensaje);
		char* id = string_itoa(particion->id);

		strcpy(string, "    LRU: ");
		strcpy(string, lru);
		strcpy(string, "    COLA: ");
		strcpy(string, cola);
		strcpy(string, "    ID: ");
		strcpy(string, tamano);
	}

	return string;
}

char* get_header_dump() {
	char* string = malloc(sizeof(char)*30);
	int hora, min, seg, dia, mes, anio;
	time_t now;

	time(&now);
	struct tm *local = localtime(&now);
	strcpy(string, "Dump: ");
	strcpy(string, string_itoa(local->tm_mday));
	strcpy(string, "/");
	strcpy(string, string_itoa(local->tm_mon));
	strcpy(string, "/");
	strcpy(string, string_itoa(local->tm_year));
	strcpy(string, " ");
	strcpy(string, string_itoa(local->tm_hour));
	strcpy(string, ":");
	strcpy(string, string_itoa(local->tm_min));
	strcpy(string, ":");
	strcpy(string, string_itoa(local->tm_sec));
	strcpy(string, "/n");

	return string;
}
