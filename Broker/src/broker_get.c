#include "broker.h"

void manejar_mensaje_get(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
	int socket_cliente= combo->conexion;

	t_get_pokemon* mensaje_get_pokemon =
			deserializar_get_pokemon(buffer);


	int id_mensaje_recibido = asignar_id_get_pokemon(
			mensaje_get_pokemon);

	log_info(logger, "Llegada de mensaje nuevo %i a cola APPEARED_POKEON",
			id_mensaje_recibido);

	devolver_get_pokemon(socket_cliente, mensaje_get_pokemon);
	log_trace(logger,
			"Se devolvio el mensaje APPEARED_POKEMON con id asignado.");

	enviar_a_todos_los_subs_get_pokemon(mensaje_get_pokemon);

	cachear_get_pokemon(mensaje_get_pokemon);

	//free (liberar memoria)
}

int asignar_id_get_pokemon(t_get_pokemon* mensaje) {
	int id = get_id_mensajes();
	mensaje->id_mensaje = id;
	return id;
}


void devolver_get_pokemon(int socket_cliente, t_get_pokemon* mensaje_get_pokemon) {
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje_get_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, GET_POKEMON);
}

void enviar_a_todos_los_subs_get_pokemon(t_get_pokemon* mensaje) {
	log_trace(logger,
			"Se van a enviar a todos los subs, el nuevo APPEARED_POKEMON.");

	void enviar_get_pokemon_a_suscriptor_aux(void* suscriptor) {
		enviar_get_pokemon_a_suscriptor((int)suscriptor, mensaje);
	}

	list_iterate(get_pokemon,
			enviar_get_pokemon_a_suscriptor_aux);

	log_trace(logger,
			"Se va a enviar a todos los subs, el nuevo APPEARED_POKEMON.");
}

void enviar_get_pokemon_a_suscriptor(int suscriptor,
		t_get_pokemon* mensaje) {

	//Envio del mensaje
	log_trace(logger, "Se va a enviar mensaje APPEARED_POKEMON id: %i a sub: %i.",
				mensaje->id_mensaje, suscriptor);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje);

	enviar_mensaje(suscriptor, mensaje_serializado, GET_POKEMON);

	log_info(logger, "Envio de APPEARED_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor);
}

void cachear_get_pokemon(t_get_pokemon* mensaje){
	int size_stream = sizeof(uint32_t) + mensaje-> size_pokemon;//Size se calcula aca porque lo necesita la funcion cachear_mensaje (general)

	int tipo_mensaje = GET_POKEMON;
	int id_mensaje = mensaje->id_mensaje;
	void* mensaje_a_cachear = serializar_cache_get_pokemon(mensaje, size_stream);

	cachear_mensaje(size_stream, id_mensaje, tipo_mensaje, mensaje_a_cachear);
}
