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

	cachear_mensaje(size_stream, id_mensaje, tipo_mensaje, mensaje_a_cachear);
}

