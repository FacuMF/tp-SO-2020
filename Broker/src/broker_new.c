#include "broker.h"

void manejar_mensaje_new(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
		int socket_cliente= combo->conexion;
	t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);

			int id_mensaje_recibido = asignar_id_new_pokemon(mensaje_new_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola NEW_POKEON",
					id_mensaje_recibido);

			devolver_new_pokemon(socket_cliente, mensaje_new_pokemon);
			log_trace(logger,
					"Se devolvio el mensaje NEW_POKEMON con id asignado.");

			almacenar_en_cola_new_pokemon(mensaje_new_pokemon);
			log_trace(logger, "Se almaceno el mensaje NEW_POKEMON en la cola.");

			//cachear_new_pokemon(mensaje);

			//free (liberar memoria)
}

 int asignar_id_new_pokemon(t_new_pokemon* mensaje){
	 int id = get_id_mensajes();
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_new_pokemon(int socket_cliente ,t_new_pokemon* mensaje_new_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_new_pokemon(mensaje_new_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, NEW_POKEMON);
}


void almacenar_en_cola_new_pokemon(t_new_pokemon* mensaje){
	list_add(new_pokemon->mensajes, mensaje);

	int size = list_size(new_pokemon->mensajes);
	t_new_pokemon* elemento_agregado = list_get(new_pokemon->mensajes, size-1);
	log_trace(logger, "Se agrego a la cola NEW_POKEMON el mensaje con id: %i.",
						elemento_agregado->id_mensaje);

	enviar_a_todos_los_subs_new_pokemon(mensaje);


}

void enviar_a_todos_los_subs_new_pokemon(t_new_pokemon* mensaje){
	log_trace(logger, "Se van a enviar a todos los subs, el nuevo NEW_POKEMON.");

	void enviar_new_pokemon_a_suscriptor_aux(void* suscriptor_aux){
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_new_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(new_pokemon->subscriptores, enviar_new_pokemon_a_suscriptor_aux);

	log_trace(logger, "Se va a enviar a todos los subs, el nuevo NEW_POKEMON.");
}

void enviar_new_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_new_pokemon* mensaje){

	log_trace(logger, "Se va a enviar mensaje NEW_POKEMON id: %i a sub: %i."
			, mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_new_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, NEW_POKEMON);

	log_info(logger, "Envio de NEW_POKEMON %i a suscriptor %i", mensaje->id_mensaje, suscriptor->socket);

	log_trace(logger, "Se envio mensaje NEW_POKEMON");
}


/*
 void cachear_new_pokemon(t_mensaje_new_pokemon mensaje){
 //TODO
 return 0;
 }
*/
