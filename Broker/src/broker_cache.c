#include "broker.h"

void inicializacion_cache(void) {
	// Leer config y parsear valores.

	tamano_memoria = config_get_int_value(config, "TAMANO_MEMORIA");
	tamano_minimo_particion = config_get_int_value(config,
			"TAMANO_MINIMO_PARTICION");

	algoritmo_memoria = de_string_a_alg_memoria(
			config_get_string_value(config, "ALGORITMO_MEMORIA"));
	algoritmo_remplazo = de_string_a_alg_remplazo(
			config_get_string_value(config, "ALGORITMO_REEMPLAZO"));
	algoritmo_particion_libre = de_string_a_alg_particion_libre(
			config_get_string_value(config, "ALGORITMO_PARTICION_LIBRE"));

	frecuencia_compactacion = config_get_int_value(config,
			"FRECUENCIA_COMPACTACION");

	log_trace(logger,
			"Se leyo de config: tamano_memoria: %i, tamano_minimo_particion: %i, alg_memoria: %i, alg_remplazo: %i, alg_part_libre: %i, frec_compact:  %i.",
			tamano_memoria, tamano_minimo_particion, algoritmo_memoria,
			algoritmo_remplazo, algoritmo_particion_libre,
			frecuencia_compactacion);

	// Inicializar memoria cache

	memoria_cache = malloc(tamano_memoria);

	// Inicializar memoria administrativa

	t_mensaje_cache* primer_particion = malloc(sizeof(t_mensaje_cache));
	primer_particion->tipo_mensaje = VACIO;
	primer_particion->offset = 0;
	primer_particion->tamanio = tamano_memoria;

	struct_admin_cache = list_create();
	list_add(struct_admin_cache, primer_particion);

	// Inicializar LRU flag

	actual_lru_flag = 0;

	//Compactacion

	contador_part_eliminar_para_compactar = frecuencia_compactacion;
}

void cachear_mensaje(int size_stream, int id_mensaje, int tipo_mensaje,
		void* mensaje_a_cachear) {

	int tamano_a_cachear = calcular_tamanio_a_cachear(size_stream);

	_Bool no_se_agrego_mensaje_a_cache = true;

	log_trace(logger, "Tamano de mensaje a cachear: %i (size stream: %i).",
			tamano_a_cachear, size_stream);

	pthread_mutex_lock(&mutex_memoria_cache);

	_Bool compacto_o_elimino = true;//True = Compacto - False = Elimino particion

	while (no_se_agrego_mensaje_a_cache) { // Se repite hasta que el mensaje este en cache.

		ordenar_cache_para_rellenar(size_stream);

		if (particion_valida_para_llenar(list_get(struct_admin_cache, 0),
				tamano_a_cachear)) {

			log_trace(logger,
					"La particion elegida tiene tamano suficiente (%ib) para el mensaje(%ib).",
					((t_mensaje_cache*) list_get(struct_admin_cache, 0))->tamanio,
					tamano_a_cachear);

			t_mensaje_cache* particion_mensaje =
					crear_y_agregar_particion_mensaje_nuevo(tipo_mensaje,
							id_mensaje, tamano_a_cachear);

			if (queda_espacio_libre(tamano_a_cachear,
					list_get(struct_admin_cache, 0)))
				crear_y_agregar_particion_sobrante(tamano_a_cachear);

			borrar_particiones_del_inicio(1);

			agregar_mensaje_a_cache(mensaje_a_cachear, size_stream,
					particion_mensaje);


			ordenar_cache_segun_su_lugar_en_memoria();

			no_se_agrego_mensaje_a_cache = false; //Para que salga del while.

		} else {

			log_trace(logger, "No hay lugar");

			if(compacto_o_elimino){
				compactar_cache_si_corresponde();
				compacto_o_elimino = false;
			}else {
				elegir_vitima_y_eliminarla(); // Y consolido
				compacto_o_elimino = true;

			}

		}
		log_dump_de_cache();

	}

	pthread_mutex_unlock(&mutex_memoria_cache);

}

int calcular_tamanio_a_cachear(int size_stream) {
	int tamanio;

	switch (algoritmo_memoria) {
	case PARTICIONES:
		tamanio = size_stream;

		break;
	case BS:
		tamanio = 2;
		while (tamanio < size_stream) {
			tamanio *= 2;
		}

		break;
	default:
		log_debug(logger, "Algoritmo de memoria incorrecto.");
		break;
	}

	return ((tamanio >= tamano_minimo_particion) ?
			tamanio : tamano_minimo_particion);
}

_Bool hay_particion_tamanio_suficiente(int size_stream) {

	_Bool tamanio_mayor_a_size(void* particion) {
		return ((t_mensaje_cache*) particion)->tamanio >= size_stream;
	}

	return list_any_satisfy(struct_admin_cache, tamanio_mayor_a_size);
}

void ordenar_cache_para_rellenar(int size_stream) {
	_Bool ordenar_para_rellenar_aux(void* mensaje_1_aux, void* mensaje_2_aux) {
		t_mensaje_cache* mensaje_1 = (t_mensaje_cache*) mensaje_1_aux;
		t_mensaje_cache* mensaje_2 = (t_mensaje_cache*) mensaje_2_aux;

		return ordenar_para_rellenar(mensaje_1, mensaje_2, size_stream);
	}
	// Buscar particion a llenar
	list_sort(struct_admin_cache, ordenar_para_rellenar_aux);
	// Se ordenan las particiones, tanto ocupadas como desocupadas, dejando adelante las libres y con tamano suficiente
	// y dejando primera la que se debe remplazar.En FirstFit, se deja primero la de offset menor, y en BestFit la de tamano menor. (Switch)
}

t_mensaje_cache* crear_y_agregar_particion_mensaje_nuevo(int tipo_mensaje,
		int id_mensaje, int tamano_a_cachear) {
	t_mensaje_cache* particion_mensaje = crear_particion_mensaje(tipo_mensaje,
			id_mensaje, tamano_a_cachear, list_get(struct_admin_cache, 0));
	list_add(struct_admin_cache, particion_mensaje);
	log_trace(logger, "Se agrego la particion con el mensaje.");
	return particion_mensaje;
}

void crear_y_agregar_particion_sobrante(int tamanio_cacheado) {

	t_mensaje_cache* particion_a_llenar = list_get(struct_admin_cache, 0);

	switch (algoritmo_memoria) {

	case PARTICIONES:
		;
		t_mensaje_cache* particion_sobrante = crear_particion_sobrante(
				tamanio_cacheado, particion_a_llenar);
		list_add(struct_admin_cache, particion_sobrante);
		log_trace(logger, "Se agrego la particion sobrante.");

		break;
	case BS:
		;

		int tamano_a_rellenar = (particion_a_llenar->tamanio)
				- tamanio_cacheado;
		int tamanio_proxima_particion_vacia = tamanio_cacheado;
		int offset_proxima_particion_vacia = (particion_a_llenar->offset)
				+ tamanio_cacheado;

		while (tamano_a_rellenar > 0) {
			agrego_part_vacia(offset_proxima_particion_vacia,
					tamanio_proxima_particion_vacia);

			tamano_a_rellenar -= tamanio_proxima_particion_vacia;
			offset_proxima_particion_vacia += tamanio_proxima_particion_vacia;
			tamanio_proxima_particion_vacia *= 2;

		}

		break;
	default:
		log_debug(logger, "Algoritmo de memoria incorrecto.");
		break;
	}

}

void ordenar_cache_segun_su_lugar_en_memoria() {
	list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria);
	log_trace(logger,
			"Se dejo la estructura ordenada por su lugar en memoria.");
}

int de_string_a_alg_memoria(char* string) {
	if (string_equals_ignore_case(string, "PARTICIONES")) {
		return PARTICIONES;
	} else if (string_equals_ignore_case(string, "BS")) {
		return BS;
	} else {
		return -1;
	}
}
int de_string_a_alg_remplazo(char* string) {
	if (string_equals_ignore_case(string, "FIFO")) {
		return FIFO;
	} else if (string_equals_ignore_case(string, "LRU")) {
		return LRU;
	} else {
		return -1;
	}
}
int de_string_a_alg_particion_libre(char* string) {
	if (string_equals_ignore_case(string, "FF")) {
		return FF;
	} else if (string_equals_ignore_case(string, "BF")) {
		return BF;
	} else {
		return -1;
	}
}

_Bool ordenar_para_rellenar(t_mensaje_cache* mensaje_1,
		t_mensaje_cache* mensaje_2, int tamano_mensaje) {
	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista

	if (mensaje_1->tipo_mensaje == 0 && mensaje_2->tipo_mensaje != 0)
		return true; // Si el 1 esta libre y el 2 no => el 1 va primero
	if (mensaje_2->tipo_mensaje == 0 && mensaje_1->tipo_mensaje != 0)
		return false; // Si el 2 esta libre y el 1 no => el 2 va primero
	if (mensaje_1->tipo_mensaje != 0 && mensaje_2->tipo_mensaje != 0)
		return true; // Si ambos estan ocupados => Me da lo mismo el orden, tiro true para que queden como estan
	if (mensaje_1->tipo_mensaje == 0 && mensaje_2->tipo_mensaje == 0) { // Si ambos estan vacios => Sigo evaluando

		if (mensaje_1->tamanio >= tamano_mensaje
				&& mensaje_2->tamanio < tamano_mensaje)
			return true; // Si el 1 tiene tamano suficiente y el 2 no => el 1 va primero
		if (mensaje_2->tamanio >= tamano_mensaje
				&& mensaje_1->tamanio < tamano_mensaje)
			return false; // Si el 2 tiene tamano suficiente y el 1 no => el 2 va primero
		if (mensaje_1->tamanio < tamano_mensaje
				&& mensaje_2->tamanio < tamano_mensaje)
			return true; // Si ambos no tienen tamano suficiente => Me da lo mismo el orden
		if (mensaje_1->tamanio >= tamano_mensaje
				&& mensaje_2->tamanio >= tamano_mensaje) {

			switch (algoritmo_particion_libre) {
			case FF:
				return (mensaje_1->offset <= mensaje_2->offset); //Si el mensaje 1 esta antes en memoria => El 1 va primero
				break;

			case BF:
				return (mensaje_1->tamanio - tamano_mensaje)
						<= (mensaje_2->tamanio - tamano_mensaje); // Si el espacio sobrante de 1 es menor que el de 2 => el 1 va primero.
				break;

			}
		}
	}
	return NULL;
}

_Bool particion_valida_para_llenar(t_mensaje_cache* particion,
		int tamano_a_ocupar) {
	_Bool esta_vacia = (particion->tipo_mensaje == VACIO);

	_Bool tamano_suficiente = (particion->tamanio >= tamano_a_ocupar);
	//Si no entra justo, la particion sobrante tiene que ser mayor a la minima.

	return tamano_suficiente && esta_vacia; //Esta vacia y con tamanio suficiente.
}

t_mensaje_cache* crear_particion_mensaje(int tipo_mensaje, int id_mensaje,
		int tamano_a_cachear, t_mensaje_cache* particion_vacia) {
	t_mensaje_cache* particion_llena = malloc(sizeof(t_mensaje_cache));

	particion_llena->tipo_mensaje = tipo_mensaje;
	particion_llena->id = id_mensaje;
	particion_llena->offset = particion_vacia->offset;
	particion_llena->flags_lru = get_lru_flag();

	particion_llena->tamanio = tamano_a_cachear;

	particion_llena->subscribers_enviados = lista_subs_eviados(tipo_mensaje);
	particion_llena->subscribers_recibidos = list_create();

	return particion_llena;
}

int get_lru_flag() {
	pthread_mutex_lock(&mutex_lru_flag);

	int flag = actual_lru_flag;
	actual_lru_flag++;

	pthread_mutex_unlock(&mutex_lru_flag);

	return flag;
}

t_list* lista_subs_eviados(int tipo_mensaje) {
	t_list* lista_subs = list_create();

	t_list* subs_queue = get_cola_segun_tipo(tipo_mensaje);

	list_add_all(lista_subs, subs_queue);

	return lista_subs;
}

_Bool queda_espacio_libre(int tamano_mensaje_a_cachear,
		t_mensaje_cache* particion_vacia) {
	return particion_vacia->tamanio > tamano_mensaje_a_cachear;
}

t_mensaje_cache* crear_particion_sobrante(int tamanio_mensaje_cacheado,
		t_mensaje_cache* particion_vacia) {
	t_mensaje_cache* particion_sobrante = malloc(sizeof(t_mensaje_cache));

	particion_sobrante->tipo_mensaje = 0;
	particion_sobrante->id = -20;
	particion_sobrante->offset = (particion_vacia->offset)
			+ tamanio_mensaje_cacheado;
	particion_sobrante->tamanio = (particion_vacia->tamanio)
			- tamanio_mensaje_cacheado;
	particion_sobrante->flags_lru = -20;

	return particion_sobrante;
}

void liverar_t_mensaje_cache(void* mensaje) {
	list_destroy_and_destroy_elements(
			((t_mensaje_cache*) mensaje)->subscribers_enviados, free);
	list_destroy_and_destroy_elements(
			((t_mensaje_cache*) mensaje)->subscribers_recibidos, free);
	free(mensaje);
}

void agregar_mensaje_a_cache(void* mensaje_a_cachear, int tamano_stream,
		t_mensaje_cache* particion_mensaje) {
	memcpy(memoria_cache + (particion_mensaje->offset), mensaje_a_cachear,
			tamano_stream);

	char* tipo_mensaje = op_code_a_string(particion_mensaje->tipo_mensaje);

	log_info(logger, "Se almaceno un mensaje %s en la posicion %i.",
				tipo_mensaje, (particion_mensaje->offset));
	free(tipo_mensaje);
}

_Bool ordenar_segun_su_lugar_en_memoria(void* mensaje_1, void* mensaje_2) {
	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista
	return (((t_mensaje_cache*) mensaje_1)->offset)
			< (((t_mensaje_cache*) mensaje_2)->offset);
}

void elegir_vitima_y_eliminarla() {
	if(frecuencia_compactacion>0) contador_part_eliminar_para_compactar--;

	list_sort(struct_admin_cache, ordenar_segun_lru_flag); //Dejo primero al que quiero borrar
	log_trace(logger, "Cache ordenada por LRU flag.");

	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);
	int id_victima = victima->id;
	log_trace(logger, "El id de la victima elegida es: %i.", id_victima);

	char* tipo_mensaje = op_code_a_string(victima->tipo_mensaje);
	log_info(logger, "Se elimino una particion %s en la posicion %i.",
						tipo_mensaje,
						victima->offset );
	free(tipo_mensaje);

	void vaciar_una_particion(void* particion) {
		if (((t_mensaje_cache*) particion)->id == id_victima) // Si es victima
			vaciar_particion((t_mensaje_cache*) particion);
	}
	list_iterate(struct_admin_cache, vaciar_una_particion);
	log_trace(logger, "Particion victima vacia.");

	consolidar_cache(); //La victima quedo primera
	log_trace(logger, "Consolidacion terminada.");

	list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria);
	log_trace(logger, "Cache ordenada por offset.");

}

_Bool ordenar_segun_lru_flag(void* mensaje_1_aux, void* mensaje_2_aux) {
	t_mensaje_cache* mensaje_1 = (t_mensaje_cache*) mensaje_1_aux;
	t_mensaje_cache* mensaje_2 = (t_mensaje_cache*) mensaje_2_aux;

	//El comparador devuelve si el primer parametro debe aparecer antes que el segundo en la lista
	_Bool rtn =
			(mensaje_1->tipo_mensaje == 0) ?
					false : ((mensaje_1->flags_lru) < (mensaje_2->flags_lru));
	// Si la particion esta vacia, va al fondo, si esta llena se ordenan de menor a mayor valos de flag_lru
	return rtn;
}

void vaciar_particion(t_mensaje_cache* particion) {
	particion->tipo_mensaje = VACIO;
	particion->flags_lru = -20;
	particion->id = -20;
	list_clean(particion->subscribers_enviados);
	list_destroy(particion->subscribers_enviados);
	list_clean(particion->subscribers_recibidos);
	list_destroy(particion->subscribers_recibidos);
}

void consolidar_cache() {
	//IMPORTANTE: la reciente victima quedo primera en la struct_admin_cache
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);
	log_debug(logger, "Victima: offset %i, tamanio %i.", victima->offset,
			victima->tamanio);
	switch (algoritmo_memoria) {

	case PARTICIONES:
		;

		if (siguiente_es_vacio()
				&& (!anterior_es_vacio() || es_primera_part())) { //Se consolida con el siguiente

			consolidar_con_siguiente();

		} else if ((!siguiente_es_vacio() || es_ultima_part())
				&& anterior_es_vacio()) { //Se consolida con el anterior

			consolidar_con_anterior();

		} else if (siguiente_es_vacio() && anterior_es_vacio()
				&& !es_primera_part() && !es_ultima_part()) {

			consolidar_con_anterior_y_siguiente();

		} else {
			log_trace(logger, "No se consolida");
		}

		break;

	case BS:
		;

		while (buddy_es_vacio(victima)) {

			// Se fija si la buddy de la particion es vacia

			if (el_buddy_es_el_siguiente(victima)) {

				consolidar_con_siguiente();

			} else { // El buddy es el anterior

				consolidar_con_anterior();

			}

			particion_consolidada_adelante();
			victima = list_get(struct_admin_cache, 0); // Actializo la victima, que ahora es la consolidada
		}

		break;

	default:
		break;
	}

}

void log_dump_de_cache() {
	int num_particion = 1;
	log_debug(logger,
			"--------------------------------------------------------------------------------------------------");
	log_debug(logger, "Dump: ");

	void log_linea_dump_cache(void* particion) {
		log_info_particion(particion, num_particion);
		num_particion++;
	}
	list_iterate(struct_admin_cache, log_linea_dump_cache);

	log_debug(logger,
			"--------------------------------------------------------------------------------------------------");
}

void log_info_particion(t_mensaje_cache* particion, int num_part) {
	int direc_inicio = ((int) memoria_cache) + particion->offset;
	int direc_final = ((int) memoria_cache) + particion->offset
			+ particion->tamanio;
	char* libre_o_ocupado = (particion->tipo_mensaje == VACIO) ? "[L]" : "[X]";
	int tamano = particion->tamanio;

	if (particion->tipo_mensaje == VACIO) {
		log_debug(logger,
				"Particion %i: 0x%X - 0x%X.    [L]    Size:%ib    Offset:%i",
				num_part, direc_inicio, direc_final, tamano, particion->offset);
	} else {
		log_debug(logger,
				"Particion %i: 0x%X - 0x%X.    [X]    Size:%ib    LRU:%i    Cola:%i    ID:%i    Offset:%i ",
				num_part, direc_inicio, direc_final, tamano,
				particion->flags_lru, particion->tipo_mensaje, particion->id,
				particion->offset);
	}

}

void particion_consolidada_adelante() {
	// La consolidada quedo ultima, la dejo primera.
	int posicion_particion_consolidada = list_size(struct_admin_cache) - 1;
	dejar_particion_adelante(posicion_particion_consolidada);

}

void consolidar_con_siguiente() {
	log_trace(logger, "Se consolida con el siguiente.");
	ordeno_dejando_victima_y_siguiente_adelante();
	agregar_particion_segun_vicima_y_siguiente();
	borrar_particiones_del_inicio(2); // borrar victima y siguiente
}

void consolidar_con_anterior() {
	log_trace(logger, "Se consolida con el anterior.");
	ordeno_dejando_anterior_y_victima_adelante();
	agregar_particion_segun_anterior_y_victima();
	borrar_particiones_del_inicio(2); // borrar victima y anterior
}

void consolidar_con_anterior_y_siguiente() {
	log_trace(logger, "Se consolida con el siguiente y con el anterior.");
	ordeno_dejando_anterior_victima_y_siguiente_adelante();
	agregar_particion_segun_anterior_victima_y_siguiente();
	borrar_particiones_del_inicio(3); // borro victima, anterior y siguiente
}

_Bool buddy_es_vacio(t_mensaje_cache* particion) {
	if (el_buddy_es_el_siguiente(particion)) {
		return siguiente_es_vacio();
	} else { // El buddy es el anterior
		return anterior_es_vacio();
	}
}

_Bool el_buddy_es_el_siguiente(t_mensaje_cache* particion) {
	//Para saber si el buddy es el anterior o el siguiente de una particion se hace lo siguiente=>
	// if( es_par( offset / tam_particion ) ) => buddy es el siguiente.
	// if( is_impar( ofset / tam_particion ) ) => buddy es el anterior
	return es_par((particion->offset) / (particion->tamanio));
}

_Bool es_par(int numero) {
	return numero % 2 == 0;
}

_Bool siguiente_es_vacio() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool es_siguiente_y_es_vacio(void* posible_siguinte) {

		_Bool es_siguiente = ((t_mensaje_cache*) posible_siguinte)->offset
				== (victima->offset + victima->tamanio);
		_Bool es_vacio = ((t_mensaje_cache*) posible_siguinte)->tipo_mensaje
				== VACIO;

		return es_siguiente && es_vacio;
	}

	return list_any_satisfy(struct_admin_cache, es_siguiente_y_es_vacio);
}

_Bool anterior_es_vacio() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool es_anterior_y_es_vacio(void* posible_anterior) {

		_Bool es_anterior = victima->offset
				== (((t_mensaje_cache*) posible_anterior)->offset
						+ ((t_mensaje_cache*) posible_anterior)->tamanio);
		_Bool es_vacio = ((t_mensaje_cache*) posible_anterior)->tipo_mensaje
				== VACIO;

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
	return es_ultima_particion(victima);
}

void borrar_particiones_del_inicio(int cant_particiones_a_borrar) {
	for (int var = 0; var < cant_particiones_a_borrar; ++var) {
		list_remove_and_destroy_element(struct_admin_cache, 0, free);
		//list_remove_and_destroy_element(struct_admin_cache, 0, liverar_t_mensaje_cache ); //Da seg fault, revisar

	}
	log_trace(logger,
			"Se borraron %i particiones del inicio de la estructura administrativa de la cache.",
			cant_particiones_a_borrar);
}

void agrego_part_vacia(int offset, int tamanio) {
	t_mensaje_cache* nueva_particion_vacia = malloc(sizeof(t_mensaje_cache));

	nueva_particion_vacia->flags_lru = -20;
	nueva_particion_vacia->id = -20;
	nueva_particion_vacia->tipo_mensaje = VACIO;

	nueva_particion_vacia->offset = offset;
	nueva_particion_vacia->tamanio = tamanio;

	list_add(struct_admin_cache, (void*) nueva_particion_vacia);
}

void ordeno_dejando_victima_y_siguiente_adelante() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool victima_y_siguiente_adelante(void* particion_1, void* particion_2) {
		_Bool es_victima_1 = es_victima(particion_1, victima);
		_Bool es_siguiente_1 = es_siguiente(particion_1, victima);
		_Bool es_victima_2 = es_victima(particion_2, victima);

		return es_victima_1 || (es_siguiente_1 && !es_victima_2);
	}

	list_sort(struct_admin_cache, victima_y_siguiente_adelante);
}

void agregar_particion_segun_vicima_y_siguiente() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);
	t_mensaje_cache* siguiente = list_get(struct_admin_cache, 1);

	agrego_part_vacia(victima->offset, victima->tamanio + siguiente->tamanio);

	log_asociacion_de_particiones_bs(victima,siguiente);
}

void log_asociacion_de_particiones_bs(t_mensaje_cache* part_1,t_mensaje_cache* part_2){
	if(algoritmo_memoria==BS){
		log_info(logger, "Se asociaron los bolques vacios que iniciaban en %i y %i.",
				part_1->offset, part_2->offset);
	}
}

void ordeno_dejando_anterior_y_victima_adelante() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool anterior_y_victima_adelante(void* particion_1, void* particion_2) {
		_Bool es_victima_1 = es_victima(particion_1, victima);
		_Bool es_anterior_1 = es_anterior(particion_1, victima);
		_Bool es_anterior_2 = es_anterior(particion_2, victima);

		return (es_anterior_1) || (es_victima_1 && !es_anterior_2);
	}

	list_sort(struct_admin_cache, anterior_y_victima_adelante);
}

void agregar_particion_segun_anterior_y_victima() {
	t_mensaje_cache* anterior = list_get(struct_admin_cache, 0);
	t_mensaje_cache* victima = list_get(struct_admin_cache, 1);

	agrego_part_vacia(anterior->offset, anterior->tamanio + victima->tamanio);

	log_asociacion_de_particiones_bs(anterior,victima);
}

void ordeno_dejando_anterior_victima_y_siguiente_adelante() {
	t_mensaje_cache* victima = list_get(struct_admin_cache, 0);

	_Bool anterior_victima_y_siguiente_adelante(void* particion_1,
			void* particion_2) {
		_Bool es_victima_1 = es_victima(particion_1, victima);
		_Bool es_anterior_1 = es_anterior(particion_1, victima);
		_Bool es_siguiente_1 = es_siguiente(particion_1, victima);
		_Bool es_anterior_2 = es_anterior(particion_2, victima);
		_Bool es_victima_2 = es_victima(particion_2, victima);

		return (es_anterior_1) || (es_victima_1 && !es_anterior_2)
				|| (es_siguiente_1 && !es_anterior_2 && !es_victima_2);
	}

	list_sort(struct_admin_cache, anterior_victima_y_siguiente_adelante);
}

void agregar_particion_segun_anterior_victima_y_siguiente() {
	t_mensaje_cache* anterior = list_get(struct_admin_cache, 0);
	t_mensaje_cache* victima = list_get(struct_admin_cache, 1);
	t_mensaje_cache* siguiente = list_get(struct_admin_cache, 2);

	agrego_part_vacia(anterior->offset,
			anterior->tamanio + victima->tamanio + siguiente->tamanio);
}

_Bool es_siguiente(void* particion, t_mensaje_cache* victima) {
	return ((t_mensaje_cache*) particion)->offset
			== (victima->offset + victima->tamanio);
}
_Bool es_anterior(void* particion, t_mensaje_cache* victima) {
	return (((t_mensaje_cache*) particion)->offset
			+ ((t_mensaje_cache*) particion)->tamanio) == victima->offset;
}
_Bool es_victima(void* particion, t_mensaje_cache* victima) {
	return ((t_mensaje_cache*) particion)->offset == victima->offset;;
}

void compactar_cache_si_corresponde() {
	log_trace(logger, "Compactara si corresponde");

	if (corresponde_compactar()) {
		//Si esta tod0 lleno no compactar
		//Recorrer hasta que encuentres vacio con uno lleno a la derecha, y invertirlos y volves a correr el algoritmo.
		//Si encuentro uno vacio con uno vacio a la derecha consolido y vuelvo a correr el algoritmo.
		//Lo corro hasta que para toda particion, !es_vacio || (es_vacio && es_ultimo), osea lo corro hasta que este compactado

		while (!esta_compactada()) {
			log_trace(logger, "No esta compactada, se va a compactar");

			algoritmo_de_compactacion();

		}
		log_info(logger, "Se ejecuto la compactacion.");
	}
}

void algoritmo_de_compactacion() {

	list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria); //La ordeno por las dudas.

	int num_particion = 0;
	_Bool compactar_en_el_siguiente_elemento = true;

	while (compactar_en_el_siguiente_elemento) {
		log_debug(logger, "Se intentara compactar elemento %i",
				num_particion + 1);

		compactar_en_el_siguiente_elemento = intentar_compactar_elemento(
				num_particion);
		//Si hizo algun cambio devuelve false para salir del while, y volver a comprobar si esta compactada.
		//Si num_part es el ultimo elemento tambien da false.
		//Si ese elemento no necesitaba ningun cambio, devuelve true, vuelve a hacer el while.

		num_particion++;
	}

	log_trace(logger,
			"Se realizo un cambio con el intento de compactar, el resultado quedo asi:");
	list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria);
	//log_dump_de_cache();
}

_Bool intentar_compactar_elemento(int num_particion) {

	_Bool es_ultimo = es_ultima_particion(
			list_get(struct_admin_cache, num_particion));
	_Bool es_vacio = es_vacia_particion(
			list_get(struct_admin_cache, num_particion));
	// Si no es ultimo y esta vacio, se chequea. Si no, se pasa de largo.

	if (!es_ultimo && es_vacio) {
		log_trace(logger,
				"No es ni ultimo ni vacio: Se va a realizar un cambio.");

		_Bool siguiente_vacio = es_vacia_particion(
				list_get(struct_admin_cache, num_particion + 1));

		if (siguiente_vacio) {
			log_trace(logger, "Siguiente es vacio, se consolidara.");

			dejar_particion_adelante(num_particion); //Para consolidar tengo que dejar el elemento vacio adelante.
			consolidar_cache();

			return false; //Que se fije si esta compactada o no

		} else {
			log_trace(logger,
					"Siguiente es lleno, se movera la informacion.");

			mover_a_particion_info_del_siguiente(num_particion);

			return false; //Que se fije si esta compactada o no

		}

	}
	return !es_ultimo; // Si no ultimo, que se fije el siguiente elemento, sino no que se fije si esta compactada o no
}

void dejar_particion_adelante(int num_particion) {
	t_mensaje_cache* particion = list_get(struct_admin_cache, num_particion);

	_Bool particion_adelante(void* una_particion, void* otra_particion) {
		_Bool es_particion = particiones_iguales(particion,
				(t_mensaje_cache*) una_particion);

		return es_particion;
	}

	list_sort(struct_admin_cache, particion_adelante);
}

void mover_a_particion_info_del_siguiente(int num_particion) {
	// Remplazar lo que esta en a siguiente, por lo que esta en la actual, que es vacia.
	t_mensaje_cache* particion = list_get(struct_admin_cache, num_particion);
	t_mensaje_cache* siguiente = list_get(struct_admin_cache,
			num_particion + 1);
	int desde_offset = siguiente->offset;
	int hasta_offset = particion->offset;
	int tamanio_mensaje = siguiente->tamanio;

	int nuevo_offset_particion = hasta_offset + tamanio_mensaje;
	int nuevo_offset_siguiente = hasta_offset;

	mover_info_cache(desde_offset, hasta_offset, tamanio_mensaje);

	// Lo que esta en siguiente lo copio en particion.

	si_es_part_mover_struct_a(particion, nuevo_offset_particion);
	si_es_part_mover_struct_a(siguiente, nuevo_offset_siguiente);

}

void si_es_part_mover_struct_a(t_mensaje_cache* particion, int offset_destino) {

	void mover(void* una_particion) {
		if (particiones_iguales(particion, (t_mensaje_cache*) una_particion)) {
			//Si es la particion => lo muevo al lugar de siguiente

			((t_mensaje_cache*) una_particion)->offset = offset_destino;
		}
	}

	list_iterate(struct_admin_cache, mover);

}

_Bool corresponde_compactar() {
	if (algoritmo_memoria == PARTICIONES) {

		if(frecuencia_compactacion == -1) return false; // Si la frecuencia es -1, no se compacta nunca.

		if (contador_part_eliminar_para_compactar == 0) { // Si el contador esta en 0 se compacta.
			contador_part_eliminar_para_compactar = frecuencia_compactacion; //Reinicio contador
			return true;
		} else {
			return false;
		}

	} else // Si el algoritmo de memoria es Buddy Sistem, no se compacta.
		return false;
}

_Bool esta_compactada() {

	_Bool no_es_vacio_o_es_vacio_y_ultimo(void* particion) {
		return !es_vacia_particion(particion)
				|| (es_vacia_particion(particion)
						&& es_ultima_particion(particion));
	}

	return list_all_satisfy(struct_admin_cache, no_es_vacio_o_es_vacio_y_ultimo);
}

_Bool es_vacia_particion(t_mensaje_cache* particion) {
	return particion->tipo_mensaje == VACIO;
}
_Bool es_ultima_particion(t_mensaje_cache* particion) {
	return particion->offset + particion->tamanio == tamano_memoria;
}

void mover_info_cache(int desde_offset, int hasta_offset, int tamanio) {
	memcpy(memoria_cache + hasta_offset, memoria_cache + desde_offset, tamanio);
}

_Bool particiones_iguales(t_mensaje_cache* una_part, t_mensaje_cache* otra_part) {
	_Bool mismo_offset = una_part->offset == otra_part->offset;
	_Bool mismo_tipo = una_part->tipo_mensaje == otra_part->tipo_mensaje;
	_Bool mismo_id_o_vacio = (una_part->id == otra_part->id)
			|| (una_part->tipo_mensaje == VACIO);

	return mismo_offset && mismo_tipo && mismo_id_o_vacio;
}

void estado_actual_de_cache() {
	FILE* dump_file = fopen("Dump Broker", "a");
	int num_particion = 1;
	char* header = string_new();
	char* fecha = obtener_fecha();
	string_append(&header, "Dump: ");
	string_append(&header, fecha);
	free(fecha);
	char* borde =
			"--------------------------------------------------------------------------------------------------\n";

	void escribir_linea_de_particion(void* particion) {
		char* estado_particion = string_new();
		char* aux = obtener_estado_de_particion(particion, num_particion);
		string_append(&estado_particion,
				aux);
		fwrite(estado_particion, 1, strlen(estado_particion), dump_file);
		num_particion++;
		free(aux);
		free(estado_particion);
	}

	fwrite(borde, 1, strlen(borde), dump_file);
	fwrite(header, 1, strlen(header), dump_file);

	list_iterate(struct_admin_cache, escribir_linea_de_particion);
	fwrite(borde, 1, strlen(borde), dump_file);

	fclose(dump_file);
	free(header);
}

char* obtener_estado_de_particion(t_mensaje_cache* particion, int num_part) {
	int direc_inicio = ((int) memoria_cache) + particion->offset;
	int direc_final = ((int) memoria_cache) + particion->offset
			+ particion->tamanio;
	char* libre_o_ocupado = (particion->tipo_mensaje == VACIO) ? "[L]" : "[X]";
	int tamano = particion->tamanio;
	char* estado_particion = malloc(255);

	if (particion->tipo_mensaje == VACIO) {
		snprintf(estado_particion, 255,
				"Particion %i: 0x%X - 0x%X.    [L]    Size:%ib    Offset:%i\n",
				num_part, direc_inicio, direc_final, tamano, particion->offset);
	} else {
		snprintf(estado_particion, 255,
				"Particion %i: 0x%X - 0x%X.    [X]    Size:%ib    LRU:%i    Cola:%i    ID:%i    Offset:%i \n",
				num_part, direc_inicio, direc_final, tamano,
				particion->flags_lru, particion->tipo_mensaje, particion->id,
				particion->offset);
	}
	return estado_particion;

}

char* obtener_fecha() {
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	char* fecha = string_new();
	string_append(&fecha, asctime(timeinfo));
	return fecha;

}

void log_mensaje_de_cache(t_mensaje_cache* particion_mensaje) {

	void* stream = malloc(particion_mensaje->tamanio);
	memcpy(stream, memoria_cache + particion_mensaje->offset,
			particion_mensaje->tamanio);

	switch (particion_mensaje->tipo_mensaje) {
	case APPEARED_POKEMON:
		;
		t_appeared_pokemon* mensaje = deserializar_cache_appeared_pokemon(
				stream);

		log_trace(logger,
				"El mensaje que se guardo en cache, deserializado es %s. (Ignorar ID, es trash).",
				mostrar_appeared_pokemon(mensaje));

		break;
	default:

		log_trace(logger, "No se puede imprimir un mensaje vacio.");

		break;
	}

}

void enviar_mensajes_cacheados_a_cliente(t_subscriptor* suscripcion,
		int socket_cliente) {

	pthread_mutex_lock(&mutex_memoria_cache);

	void enviar_mensaje_cacheados_a_sub(void* particion) {
		enviar_mensaje_cacheado_a_sub_si_es_de_cola(
				suscripcion->cola_de_mensaje, socket_cliente,
				(t_mensaje_cache*) particion);
	}

	list_iterate(struct_admin_cache, enviar_mensaje_cacheados_a_sub);

	pthread_mutex_unlock(&mutex_memoria_cache);

}



void enviar_mensaje_cacheado_a_sub_si_es_de_cola(int tipo_mensaje,
		int socket_cliente, t_mensaje_cache* particion) {
	if (particion->tipo_mensaje == tipo_mensaje) {
		t_buffer* mensaje_serializado;

		mensaje_serializado = serializar_mensaje_de_cache(particion);

		enviar_mensaje(socket_cliente, mensaje_serializado, tipo_mensaje);

		list_add(particion->subscribers_enviados, (void*) socket_cliente);

		if (algoritmo_remplazo == LRU)
			particion->flags_lru = get_lru_flag();
		log_trace(logger, "Se envio mensaje %i a sub %i. Flag lru/fifo:%i.",
				particion->id, socket_cliente, particion->flags_lru);
	}
}

t_buffer* serializar_mensaje_de_cache(t_mensaje_cache* particion) {
	t_buffer* mensaje_serializado;

	void* stream_mensaje = malloc(particion->tamanio);

	memcpy(stream_mensaje, memoria_cache + (particion->offset),
			particion->tamanio);

	switch (particion->tipo_mensaje) {

	case APPEARED_POKEMON:
		;
		t_appeared_pokemon* mensaje_appeared_pokemon =
				deserializar_cache_appeared_pokemon(stream_mensaje);
		mensaje_appeared_pokemon->id_mensaje = particion->id;

		mensaje_serializado = serializar_appeared_pokemon(
				mensaje_appeared_pokemon);
		liberar_mensaje_appeared_pokemon(mensaje_appeared_pokemon);
		break;

	case CATCH_POKEMON:
		;
		t_catch_pokemon* mensaje_catch_pokemon =
				deserializar_cache_catch_pokemon(stream_mensaje);
		mensaje_catch_pokemon->id_mensaje = particion->id;

		mensaje_serializado = serializar_catch_pokemon(mensaje_catch_pokemon);
		liberar_mensaje_catch_pokemon(mensaje_catch_pokemon);
		break;

	case CAUGHT_POKEMON:
		;
		t_caught_pokemon* mensaje_caught_pokemon =
				deserializar_cache_caught_pokemon(stream_mensaje);
		mensaje_caught_pokemon->id_mensaje = particion->id;

		mensaje_serializado = serializar_caught_pokemon(mensaje_caught_pokemon);
		liberar_mensaje_caught_pokemon(mensaje_caught_pokemon);
		break;

	case GET_POKEMON:
		;
		t_get_pokemon* mensaje_get_pokemon = deserializar_cache_get_pokemon(
				stream_mensaje);
		mensaje_get_pokemon->id_mensaje = particion->id;

		mensaje_serializado = serializar_get_pokemon(mensaje_get_pokemon);
		liberar_mensaje_get_pokemon(mensaje_get_pokemon);
		break;

	case LOCALIZED_POKEMON:
		;
		t_localized_pokemon* mensaje_localized_pokemon =
				deserializar_cache_localized_pokemon(stream_mensaje);
		mensaje_localized_pokemon->id_mensaje = particion->id;
		mensaje_serializado = serializar_localized_pokemon(
				mensaje_localized_pokemon);
		liberar_mensaje_localized_pokemon(mensaje_localized_pokemon);
		break;

	case NEW_POKEMON:
		;
		t_new_pokemon* mensaje_new_pokemon = deserializar_cache_new_pokemon(
				stream_mensaje);
		mensaje_new_pokemon->id_mensaje = particion->id;

		mensaje_serializado = serializar_new_pokemon(mensaje_new_pokemon);
		liberar_mensaje_new_pokemon(mensaje_new_pokemon);
		break;

	default:

		log_debug(logger,
				"No deberia estar aca, trata de deserializar un mensaje vacio o no reconocido de cache.");
		break;
	}
	liberar_stream(stream_mensaje);
	return mensaje_serializado;
}

void test() {

}

