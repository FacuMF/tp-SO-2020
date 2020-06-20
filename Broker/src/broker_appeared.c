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

	enviar_a_todos_los_subs_appeared_pokemon(mensaje);

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

	log_trace(logger,
			"Se va a enviar mensaje APPEARED_POKEMON id: %i a sub: %i.",
			mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, APPEARED_POKEMON);

	log_info(logger, "Envio de APPEARED_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor->socket);

	log_trace(logger, "Se envio mensaje APPEARED_POKEMON");
}



void cachear_appeared_pokemon(t_appeared_pokemon mensaje){
/*
	void* mensaje_a_cachear = serializar_cache_appeared_pokemon(mensaje);

	//De aca en adelante se puede generalizar para todos los mensajes, no solo appeared.

	int tamano_mensaje_a_cachear = size(mensaje_a_cachear);

	_Bool se_agrego_mensaje_a_cache = false;

	while (se_agrego_mensaje_a_cache) { // Se repite hasta que el mensaje este en cache.

		// Buscar particion a llenar
		list_sort(struct_admin_cache, ordenar_para_remplazo);
			// Se ordenan las particiones, tanto ocupadas como desocupadas, dejando adelante las libres y con tamano suficiente
			// y dejando primera la que se debe remplazar.En Fest Fit, se deja primero la de offset menor, y en BestFit la de tamano menor.

		if( particion_valida_para_llenar( list_get (struct_admin_cache, 1) , tamano_mensaje_a_cachear ) ){
			// Este if va a dejar llenar la particion si la que quedo primera, que deberia ser la mas cercana a poder ser llenada, efectivamente lo es.
			// Si esta no tiene tamano suficiente, significa que ninguna lo tiene, entonces habra que eliminar una particion (else)

			t_mensaje_cache* particion_mensaje = crear_particion_mensaje(mensaje, list_get (struct_admin_cache, 1) );
					//Se crea la particion llena con el contenido del mensaje, en base a la info de la particion vacia elegida.
			list_add(struct_admin_cache, particion_mensaje);
				// Al final de la estructura administrativa agrego un elemento que referencia el mensaje por agregar.

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
			}
	}
*/
}



