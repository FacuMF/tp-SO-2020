#include "broker.h"

void manejar_mensaje_localized(t_conexion_buffer *combo) {
	t_buffer * buffer = malloc(sizeof(t_buffer));
	int socket_cliente= combo->conexion;

	memcpy(buffer, combo->buffer, sizeof(t_buffer));
	free(combo->buffer);
	free(combo);

	t_localized_pokemon* mensaje_localized_pokemon =
			deserializar_localized_pokemon(buffer);


	int id_mensaje_recibido = asignar_id_localized_pokemon(
			mensaje_localized_pokemon);

	log_info(logger, "Llegada de mensaje nuevo %i a cola LOCALIZED_POKEMON",
			id_mensaje_recibido);

	devolver_localized_pokemon(socket_cliente, mensaje_localized_pokemon);
	log_trace(logger,
			"Se devolvio el mensaje LOCALIZED_POKEMON con id asignado.");

	enviar_a_todos_los_subs_localized_pokemon(mensaje_localized_pokemon);

	cachear_localized_pokemon(mensaje_localized_pokemon);

	liberar_mensaje_localized_pokemon(mensaje_localized_pokemon);
	pthread_exit(NULL);
}

int asignar_id_localized_pokemon(t_localized_pokemon* mensaje) {
	int id = get_id_mensajes();
	mensaje->id_mensaje = id;
	return id;
}


void devolver_localized_pokemon(int socket_cliente, t_localized_pokemon* mensaje_localized_pokemon) {
	t_buffer* mensaje_serializado;
	mensaje_serializado = serializar_localized_pokemon(mensaje_localized_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, LOCALIZED_POKEMON);
}

void enviar_a_todos_los_subs_localized_pokemon(t_localized_pokemon* mensaje) {
	log_trace(logger,
			"Se van a enviar a todos los subs, el nuevo LOCALIZED_POKEMON.");

	void enviar_localized_pokemon_a_suscriptor_aux(void* suscriptor) {
		enviar_localized_pokemon_a_suscriptor((int)suscriptor, mensaje);
	}

	list_iterate(localized_pokemon,
			enviar_localized_pokemon_a_suscriptor_aux);
}

void enviar_localized_pokemon_a_suscriptor(int suscriptor,
		t_localized_pokemon* mensaje) {

	//Envio del mensaje
	log_trace(logger, "Se va a enviar mensaje LOCALIZED_POKEMON id: %i a sub: %i.",
				mensaje->id_mensaje, suscriptor);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_localized_pokemon(mensaje);

	enviar_mensaje(suscriptor, mensaje_serializado, LOCALIZED_POKEMON);

	log_info(logger, "Envio de LOCALIZED_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor);
}


void cachear_localized_pokemon(t_localized_pokemon* mensaje){
	int size_stream = sizeof(uint32_t) * (2 + 2 * (mensaje->cantidad_posiciones))
				+ mensaje-> size_pokemon;//Size se calcula aca porque lo necesita la funcion cachear_mensaje (general)

	int tipo_mensaje = LOCALIZED_POKEMON;
	int id_mensaje = mensaje->id_mensaje;
	void* mensaje_a_cachear = serializar_cache_localized_pokemon(mensaje, size_stream);

	cachear_mensaje(size_stream, id_mensaje, tipo_mensaje, mensaje_a_cachear);

	liberar_stream(mensaje_a_cachear);
}


