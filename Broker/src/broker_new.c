#include "broker.h"

void manejar_mensaje_new(t_conexion_buffer *combo) {
	t_buffer * buffer = malloc(sizeof(t_buffer));
	int socket_cliente= combo->conexion;

	memcpy(buffer, combo->buffer, sizeof(t_buffer));
	free(combo->buffer);
	free(combo);

	t_new_pokemon* mensaje_new_pokemon =
			deserializar_new_pokemon(buffer);


	int id_mensaje_recibido = asignar_id_new_pokemon(
			mensaje_new_pokemon);

	log_info(logger, "Llegada de mensaje nuevo %i a cola NEW_POKEMON",
			id_mensaje_recibido);

	devolver_new_pokemon(socket_cliente, mensaje_new_pokemon);
	log_trace(logger,
			"Se devolvio el mensaje NEW_POKEMON con id asignado.");

	enviar_a_todos_los_subs_new_pokemon(mensaje_new_pokemon);

	cachear_new_pokemon(mensaje_new_pokemon);

	liberar_mensaje_new_pokemon(mensaje_new_pokemon);
	pthread_exit(NULL);
}

int asignar_id_new_pokemon(t_new_pokemon* mensaje) {
	int id = get_id_mensajes();
	mensaje->id_mensaje = id;
	return id;
}


void devolver_new_pokemon(int socket_cliente, t_new_pokemon* mensaje_new_pokemon) {
	t_buffer* mensaje_serializado;
	mensaje_serializado = serializar_new_pokemon(mensaje_new_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, NEW_POKEMON);
}

void enviar_a_todos_los_subs_new_pokemon(t_new_pokemon* mensaje) {
	log_trace(logger,
			"Se van a enviar a todos los subs, el nuevo NEW_POKEMON.");

	void enviar_new_pokemon_a_suscriptor_aux(void* suscriptor) {
		enviar_new_pokemon_a_suscriptor((int)suscriptor, mensaje);
	}

	list_iterate(new_pokemon,
			enviar_new_pokemon_a_suscriptor_aux);
}

void enviar_new_pokemon_a_suscriptor(int suscriptor,
		t_new_pokemon* mensaje) {

	//Envio del mensaje
	log_trace(logger, "Se va a enviar mensaje NEW_POKEMON id: %i a sub: %i.",
				mensaje->id_mensaje, suscriptor);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_new_pokemon(mensaje);

	enviar_mensaje(suscriptor, mensaje_serializado, NEW_POKEMON);

	log_info(logger, "Envio de NEW_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor);
}


void cachear_new_pokemon(t_new_pokemon* mensaje){
	int size_stream = sizeof(uint32_t)*4 + mensaje-> size_pokemon;//Size se calcula aca porque lo necesita la funcion cachear_mensaje (general)

	int tipo_mensaje = NEW_POKEMON;
	int id_mensaje = mensaje->id_mensaje;
	void* mensaje_a_cachear = serializar_cache_new_pokemon(mensaje, size_stream);

	cachear_mensaje(size_stream, id_mensaje, tipo_mensaje, mensaje_a_cachear);

	liberar_stream(mensaje_a_cachear);
}
