#include "broker.h"

void manejar_mensaje_caught(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
		int socket_cliente= combo->conexion;
	t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);

			int id_mensaje_recibido = asignar_id_caught_pokemon(mensaje_caught_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola CAUGHT_POKEON",
					id_mensaje_recibido);

			devolver_caught_pokemon(socket_cliente, mensaje_caught_pokemon);
			log_trace(logger,
					"Se devolvio el mensaje CAUGHT_POKEMON con id asignado.");

			almacenar_en_cola_caught_pokemon(mensaje_caught_pokemon);
			log_trace(logger, "Se almaceno el mensaje CAUGHT_POKEMON en la cola.");

			//cachear_caught_pokemon(mensaje);

			//free (liberar memoria)
}

 int asignar_id_caught_pokemon(t_caught_pokemon* mensaje){
	 int id = get_id_mensajes();
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_caught_pokemon(int socket_cliente ,t_caught_pokemon* mensaje_caught_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_caught_pokemon(mensaje_caught_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, CAUGHT_POKEMON);
}


void almacenar_en_cola_caught_pokemon(t_caught_pokemon* mensaje){
	list_add(caught_pokemon->mensajes, mensaje);

	int size = list_size(caught_pokemon->mensajes);
	t_caught_pokemon* elemento_agregado = list_get(caught_pokemon->mensajes, size-1);
	log_trace(logger, "Se agrego a la cola CAUGHT_POKEMON el mensaje con id: %i.",
						elemento_agregado->id_mensaje);

	enviar_a_todos_los_subs_caught_pokemon(mensaje);


}

void enviar_a_todos_los_subs_caught_pokemon(t_caught_pokemon* mensaje){
	log_trace(logger, "Se van a enviar a todos los subs, el nuevo CAUGHT_POKEMON.");

	void enviar_caught_pokemon_a_suscriptor_aux(void* suscriptor_aux){
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_caught_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(caught_pokemon->subscriptores, enviar_caught_pokemon_a_suscriptor_aux);

	log_trace(logger, "Se va a enviar a todos los subs, el nuevo CAUGHT_POKEMON.");
}

void enviar_caught_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_caught_pokemon* mensaje){

	log_trace(logger, "Se va a enviar mensaje CAUGHT_POKEMON id: %i a sub: %i."
			, mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_caught_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, CAUGHT_POKEMON);

	log_info(logger, "Envio de CAUGHT_POKEMON %i a suscriptor %i", mensaje->id_mensaje, suscriptor->socket);

	log_trace(logger, "Se envio mensaje CAUGHT_POKEMON");
}


/*
 void cachear_caught_pokemon(t_mensaje_caught_pokemon mensaje){
 //TODO
 return 0;
 }
*/
