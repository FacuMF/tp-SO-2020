#include "broker.h"


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

	particion_llena->tamanio = tamano_a_cachear;

	particion_llena->subscribers_enviados = filtrar_subs_enviados( tipo_mensaje, id_mensaje);
	particion_llena->subscribers_recibidos = filtrar_subs_recibidos( tipo_mensaje, id_mensaje);

	return particion_llena;
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

// Serializacion para cache

void* serializar_cache_appeared_pokemon(t_appeared_pokemon* mensaje, int size){

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

void log_dump_de_cache(){
	char* string;
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

}

char* get_header_dump() {
	//TODO
	/*
	char* string;
	int hora, min, seg, dia, mes, anio;
	time_t now;
	time(&now);
	struct tm *local = localtime(&now);
	hora = local->tm_hour;
	min = local->tm_min;
	seg = local->tm_sec;
	day

	return string;
	*/
}
