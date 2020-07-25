#include "broker.h"

void manejar_mensaje_caught(t_conexion_buffer *combo) {
	t_buffer * buffer = malloc(sizeof(t_buffer));
	int socket_cliente= combo->conexion;

	memcpy(buffer, combo->buffer, sizeof(t_buffer));
	free(combo->buffer);
	free(combo);

	t_caught_pokemon* mensaje_caught_pokemon =
			deserializar_caught_pokemon(buffer);


	int id_mensaje_recibido = asignar_id_caught_pokemon(
			mensaje_caught_pokemon);


	//TODO: asignar id correaltivo??

	log_info(logger, "Llegada de mensaje nuevo %i a cola CAUGHT_POKEMON",
			id_mensaje_recibido);

	devolver_caught_pokemon(socket_cliente, mensaje_caught_pokemon);
	log_trace(logger,
			"Se devolvio el mensaje CAUGHT_POKEMON con id asignado.");

	enviar_a_todos_los_subs_caught_pokemon(mensaje_caught_pokemon);

	cachear_caught_pokemon(mensaje_caught_pokemon);

	//liberar_mensaje_caught_pokemon(mensaje_caught_pokemon);
	pthread_exit(NULL);
}

int asignar_id_caught_pokemon(t_caught_pokemon* mensaje) {
	int id = get_id_mensajes();
	mensaje->id_mensaje = id;
	return id;
}


void devolver_caught_pokemon(int socket_cliente, t_caught_pokemon* mensaje_caught_pokemon) {
	t_buffer* mensaje_serializado;
	mensaje_serializado = serializar_caught_pokemon(mensaje_caught_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, CAUGHT_POKEMON);
}

void enviar_a_todos_los_subs_caught_pokemon(t_caught_pokemon* mensaje) {
	log_trace(logger,
			"Se van a enviar a todos los subs, el nuevo CAUGHT_POKEMON.");

	pthread_mutex_lock(&mutex_suscribir);

	void enviar_caught_pokemon_a_suscriptor_aux(void* suscriptor) {
		enviar_caught_pokemon_a_suscriptor((int)suscriptor, mensaje);
	}

	list_iterate(caught_pokemon,
			enviar_caught_pokemon_a_suscriptor_aux);

	pthread_mutex_unlock(&mutex_suscribir);

}

void enviar_caught_pokemon_a_suscriptor(int suscriptor,
		t_caught_pokemon* mensaje) {

	//Envio del mensaje
	log_trace(logger, "Se va a enviar mensaje CAUGHT_POKEMON id: %i, id correlativo: %d a sub: %i.",
				mensaje->id_mensaje,mensaje->id_correlativo, suscriptor);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_caught_pokemon(mensaje);

	enviar_mensaje(suscriptor, mensaje_serializado, CAUGHT_POKEMON);

	log_info(logger, "Envio de CAUGHT_POKEMON %i a suscriptor %i",
			mensaje->id_mensaje, suscriptor);
}

void cachear_caught_pokemon(t_caught_pokemon* mensaje){
	int size_stream = sizeof(uint32_t);//Size se calcula aca porque lo necesita la funcion cachear_mensaje (general)

	int tipo_mensaje = CAUGHT_POKEMON;
	int id_mensaje = mensaje->id_mensaje;
	//TODO: agregar mensaje correlativo??
	void* mensaje_a_cachear = serializar_cache_caught_pokemon(mensaje, size_stream);

	cachear_mensaje(size_stream, id_mensaje, tipo_mensaje, mensaje_a_cachear);

	liberar_stream(mensaje_a_cachear);
}
