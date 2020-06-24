#include "broker.h"

void manejar_mensaje_appeared(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
	int socket_cliente= combo->conexion;

	t_appeared_pokemon* mensaje_appeared_pokemon =
			deserializar_appeared_pokemon(buffer);

	printf("%s", mostrar_appeared_pokemon(mensaje_appeared_pokemon));

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

	//cachear_appeared_pokemon(mensaje_appeared_pokemon);

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
	int tamano_a_cachear = ((size_stream >= tamano_minimo_particion)? size_stream : tamano_minimo_particion);
	_Bool no_se_agrego_mensaje_a_cache = true;

	while (no_se_agrego_mensaje_a_cache) { // Se repite hasta que el mensaje este en cache.

		_Bool ordenar_para_rellenar_aux(void* mensaje_1_aux, void* mensaje_2_aux) {
			t_mensaje_cache* mensaje_1 = (t_mensaje_cache*) mensaje_1_aux;
			t_mensaje_cache* mensaje_2 = (t_mensaje_cache*) mensaje_2_aux;

			return ordenar_para_rellenar(mensaje_1,mensaje_2,size_stream);
		}
		// Buscar particion a llenar
		list_sort(struct_admin_cache, ordenar_para_rellenar_aux);
			// Se ordenan las particiones, tanto ocupadas como desocupadas, dejando adelante las libres y con tamano suficiente
			// y dejando primera la que se debe remplazar.En FirstFit, se deja primero la de offset menor, y en BestFit la de tamano menor. (Switch)

		if( particion_valida_para_llenar( list_get (struct_admin_cache, 0) , tamano_a_cachear)){ // TODO: Ver si la t_list empiezan en 0 o en 1
			// Este if va a dejar llenar la particion si la que quedo primera, que deberia ser la mas cercana a poder ser llenada, efectivamente lo es.
			// Si esta no tiene tamano suficiente, significa que ninguna lo tiene, entonces habra que eliminar una particion (else)

			t_mensaje_cache* particion_mensaje = crear_particion_mensaje(tipo_mensaje ,id_mensaje ,tamano_a_cachear , list_get (struct_admin_cache, 0) );
					//Se crea la particion llena con el contenido del mensaje, en base a la info de la particion vacia elegida.
			list_add(struct_admin_cache, particion_mensaje);
				// Al final de la estructura administrativa agrego un elemento que referencia el mensaje por agregar.

			if( queda_espacio_libre( tamano_a_cachear, list_get (struct_admin_cache, 0) ) ){

				t_mensaje_cache* particion_sobrante = crear_particion_sobrante(tamano_a_cachear, list_get (struct_admin_cache, 1));
				list_add(struct_admin_cache, particion_sobrante);
				// Si el mensaje deja espacio suficiente como para generar una nueva particion libre, esta tambien se agrega al final
				// de la estructura administrativa.

			}

			list_remove_and_destroy_element(struct_admin_cache, 0, liverar_t_mensaje_cache );
			// Ahora borro el primer elemento de la estructura administrativa. Es la particion libre elegida, que se llenara total o parcialmente,
			// pero en ambos casos su informacion ya esta contemplada por los elementos que acabo de agregar.

			agregar_mensaje_a_cache(mensaje_a_cachear,size_stream , particion_mensaje);
			// Se agrega el mensaje a cachear al malloc de la cache con el offset que indica en la estructura administrativa.

			no_se_agrego_mensaje_a_cache = false; //Para que salga del while.

			list_sort(struct_admin_cache, ordenar_segun_su_lugar_en_memoria); // Se reordena la estructura administrativa.

		}else{
			elegir_vitima_y_eliminarla(); // Y consolido
			//compactar_cache_si_corresponde();

		}

	}
}

