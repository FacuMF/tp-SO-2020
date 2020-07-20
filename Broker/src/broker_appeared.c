#include "broker.h"

void manejar_mensaje_appeared(t_conexion_buffer *combo) {
	t_buffer * buffer = malloc(sizeof(t_buffer));
	int socket_cliente= combo->conexion;

	memcpy(buffer, combo->buffer, sizeof(t_buffer));
	free(combo->buffer);
	free(combo);

	t_appeared_pokemon* mensaje_appeared_pokemon =
			deserializar_appeared_pokemon(buffer);


	int id_mensaje_recibido = asignar_id_appeared_pokemon(
			mensaje_appeared_pokemon);

	log_info(logger, "Llegada de mensaje nuevo %i a cola APPEARED_POKEMON",
			id_mensaje_recibido);

	devolver_appeared_pokemon(socket_cliente, mensaje_appeared_pokemon);
	log_trace(logger,
			"Se devolvio el mensaje APPEARED_POKEMON con id asignado.");

	enviar_a_todos_los_subs_appeared_pokemon(mensaje_appeared_pokemon);

	cachear_appeared_pokemon(mensaje_appeared_pokemon);

	liberar_mensaje_appeared_pokemon(mensaje_appeared_pokemon);
	pthread_exit(NULL);
}

int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje) {
	int id = get_id_mensajes();
	mensaje->id_mensaje = id;
	return id;
}


void devolver_appeared_pokemon(int socket_cliente, t_appeared_pokemon* mensaje_appeared_pokemon) {
	t_buffer* mensaje_serializado;
	mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, APPEARED_POKEMON);
}

void enviar_a_todos_los_subs_appeared_pokemon(t_appeared_pokemon* mensaje) {
	log_trace(logger,
			"Se van a enviar a todos los subs, el nuevo APPEARED_POKEMON.");

	pthread_mutex_lock(&mutex_suscribir);

	void enviar_appeared_pokemon_a_suscriptor_aux(void* suscriptor) {
		enviar_appeared_pokemon_a_suscriptor((int)suscriptor, mensaje);
	}

	list_iterate(appeared_pokemon,
			enviar_appeared_pokemon_a_suscriptor_aux);

	pthread_mutex_unlock(&mutex_suscribir);

}

void enviar_appeared_pokemon_a_suscriptor(int suscriptor,
		t_appeared_pokemon* mensaje) {

	//Envio del mensaje
	log_trace(logger, "Se va a enviar mensaje APPEARED_POKEMON id: %i a sub: %i.",
				mensaje->id_mensaje, suscriptor);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje);

	enviar_mensaje(suscriptor, mensaje_serializado, APPEARED_POKEMON);

	log_info(logger, "Envio de APPEARED_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor);
}

void cachear_appeared_pokemon(t_appeared_pokemon* mensaje){
	int size_stream = sizeof(uint32_t)*3 + mensaje-> size_pokemon;//Size se calcula aca porque lo necesita la funcion cachear_mensaje (general)

	int tipo_mensaje = APPEARED_POKEMON;
	int id_mensaje = mensaje->id_mensaje;
	void* mensaje_a_cachear = serializar_cache_appeared_pokemon(mensaje, size_stream);

	cachear_mensaje(size_stream, id_mensaje, tipo_mensaje, mensaje_a_cachear);

	liberar_stream(mensaje_a_cachear);
}

