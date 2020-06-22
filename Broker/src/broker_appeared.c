#include "broker.h"

void manejar_mensaje_appeared(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
	int socket_cliente= combo->conexion;

	t_appeared_pokemon* mensaje_appeared_pokemon =
			deserializar_appeared_pokemon(buffer);

	int id_mensaje_recibido = asignar_id_appeared_pokemon(
			mensaje_appeared_pokemon);

	log_info(logger, "Llegada de mensaje nuevo %i a cola APPEARED_POKEON",
			id_mensaje_recibido);

	devolver_appeared_pokemon(socket_cliente, mensaje_appeared_pokemon);
	log_trace(logger,
			"Se devolvio el mensaje APPEARED_POKEMON con id asignado.");

	almacenar_en_cola_appeared_pokemon(mensaje_appeared_pokemon);
	log_trace(logger, "Se almaceno el mensaje APPEARED_POKEMON en la cola.");

	enviar_a_todos_los_subs_appeared_pokemon(mensaje_appeared_pokemon);

	cachear_appeared_pokemon(mensaje_appeared_pokemon);

	//free (liberar memoria)
}

int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje) {
	int id = get_id_mensajes();
	mensaje->id_mensaje = id;
	return id;
}

void devolver_appeared_pokemon(int socket_cliente,
		t_appeared_pokemon* mensaje_appeared_pokemon) {
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, APPEARED_POKEMON);
}

void almacenar_en_cola_appeared_pokemon(t_appeared_pokemon* mensaje) {
	list_add(appeared_pokemon->mensajes, mensaje);

	int size = list_size(appeared_pokemon->mensajes);
	t_appeared_pokemon* elemento_agregado = list_get(appeared_pokemon->mensajes,
			size - 1);
	log_trace(logger,
			"Se agrego a la cola APPEARED_POKEMON el mensaje con id: %i.",
			elemento_agregado->id_mensaje);



}

void enviar_a_todos_los_subs_appeared_pokemon(t_appeared_pokemon* mensaje) {
	log_trace(logger,
			"Se van a enviar a todos los subs, el nuevo APPEARED_POKEMON.");

	void enviar_appeared_pokemon_a_suscriptor_aux(void* suscriptor_aux) {
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_appeared_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(appeared_pokemon->subscriptores,
			enviar_appeared_pokemon_a_suscriptor_aux);

	log_trace(logger,
			"Se va a enviar a todos los subs, el nuevo APPEARED_POKEMON.");
}

void enviar_appeared_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor,
		t_appeared_pokemon* mensaje) {

	//Envio del mensaje
	log_trace(logger, "Se va a enviar mensaje APPEARED_POKEMON id: %i a sub: %i.",
				mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje);

	enviar_mensaje(suscriptor->socket, mensaje_serializado, APPEARED_POKEMON);

	//Confirmacion del mensaje
	list_add(suscriptor->mensajes_enviados, (void*) (mensaje->id_mensaje) );
	int tamano_lista = list_size(suscriptor->mensajes_enviados);
	log_trace(logger, "Se agrego el ID: %i a la lista de enviados que tiene %i elementos.",
			list_get(suscriptor->mensajes_enviados, tamano_lista-1), tamano_lista);

	log_info(logger, "Envio de APPEARED_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor->socket);
}



void cachear_appeared_pokemon(t_appeared_pokemon* mensaje){
	int size_stream = sizeof(uint32_t)*3 + mensaje-> size_pokemon;//Size se calcula aca porque lo necesita la funcion cachear_mensaje (general)

	int tipo_mensaje = APPEARED_POKEMON;
	int id_mensaje = mensaje->id_mensaje;
	void* mensaje_a_cachear = serializar_cache_appeared_pokemon(mensaje, size_stream);

	//De aca en adelante se puede generalizar para todos los mensajes, no solo appeared.
	//TODO: Pasar a cachear_mensaje() en broker_general

	_Bool se_agrego_mensaje_a_cache = false;

	while (se_agrego_mensaje_a_cache) { // Se repite hasta que el mensaje este en cache.

		_Bool ordenar_para_rellenar_aux(void* mensaje_1_aux, void* mensaje_2_aux) {
			t_mensaje_cache* mensaje_1 = (t_mensaje_cache*) mensaje_1_aux;
			t_mensaje_cache* mensaje_2 = (t_mensaje_cache*) mensaje_2_aux;

			return ordenar_para_rellenar(mensaje_1,mensaje_2,size_stream);
		}
		// Buscar particion a llenar
		list_sort(struct_admin_cache, ordenar_para_rellenar_aux);
			// Se ordenan las particiones, tanto ocupadas como desocupadas, dejando adelante las libres y con tamano suficiente
			// y dejando primera la que se debe remplazar.En FirstFit, se deja primero la de offset menor, y en BestFit la de tamano menor. (Switch)

		if( particion_valida_para_llenar( list_get (struct_admin_cache, 0) , size_stream)){ // TODO: Ver si la t_list empiezan en o en 1
			// Este if va a dejar llenar la particion si la que quedo primera, que deberia ser la mas cercana a poder ser llenada, efectivamente lo es.
			// Si esta no tiene tamano suficiente, significa que ninguna lo tiene, entonces habra que eliminar una particion (else)

			t_mensaje_cache* particion_mensaje = crear_particion_mensaje(tipo_mensaje ,id_mensaje ,size_stream , list_get (struct_admin_cache, 0) );
					//Se crea la particion llena con el contenido del mensaje, en base a la info de la particion vacia elegida.
			list_add(struct_admin_cache, particion_mensaje);
				// Al final de la estructura administrativa agrego un elemento que referencia el mensaje por agregar.
/*

			if( queda espacio_libre( tamano_mensaje_a_cachear, list_get (struct_admin_cache, 1) ) ){

				t_mensaje_cache* particion_sobrante = cear_particion_sobrante(mensaje, list_get (struct_admin_cache, 1));
				list_add(struct_admin_cache, particion_sobrante);
				// Si el mensaje deja espacio suficiente como para generar una nueva particion libre, esta tambien se agrega al final
				// de la estructura administrativa.

			}

			list_remove_and_destroy_element(struct_admin_cache, 1, free );
			// Ahora borro el primer elemento de la estructura administrativa. Es la particion libre elegida, que se llenara total o parcialmente,
			// pero en ambos casos su informacion ya esta contemplada por los elementos que acabo de agregar.

			agregar_mensaje_a_cache(mensaje_a_cachear, particion_mensaje);
			// Se agrega el mensaje a cachear al malloc de la cache con el offset que indica en la estructura administrativa.

			se_agrego_mensaje_a_cache = true; //Para que salga del while.

			list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria); // Se reordena la estructura administrativa.


		}else{

			elegir_vitima_y_eliminarla() // Y consolido
			compactar_cache_si_corresponde();
*/
		}

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

_Bool particion_valida_para_llenar(t_mensaje_cache* particion, int tamano_mensaje){
	//Esta vacia y con tamanio suficiente.
	return (particion->tamanio >= tamano_mensaje) && (particion->id == VACIO); //Esta vacia y con tamanio suficiente.
}

t_mensaje_cache* crear_particion_mensaje(int tipo_mensaje, int id_mensaje, int tamanio_mensaje, t_mensaje_cache* particion_vacia){
	t_mensaje_cache* particion_llena = malloc(sizeof(t_mensaje_cache));

	particion_llena->tipo_mensaje = tipo_mensaje;
	particion_llena->id = id_mensaje;
	particion_llena->offset = particion_vacia->offset;

	if (tamanio_mensaje >= tamano_minimo_particion){
		particion_llena->tamanio = tamanio_mensaje;
	} else{
		particion_llena->tamanio = tamano_minimo_particion;
	}

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

t_appeared_pokemon* deserializar_cache_appeared_pokemon(void* stream){

	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));

	memcpy( &(mensaje->size_pokemon), stream, sizeof(uint32_t));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy((mensaje->pokemon), stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(uint32_t));
	stream += sizeof(mensaje->posx);

	memcpy(&(mensaje->posy), stream, sizeof(uint32_t));
	stream += sizeof(mensaje->posy);

	return mensaje;
}

