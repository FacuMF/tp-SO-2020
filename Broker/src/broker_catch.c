#include "broker.h"

void manejar_mensaje_catch(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
		int socket_cliente= combo->conexion;
	t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(buffer);

			int id_mensaje_recibido = asignar_id_catch_pokemon(mensaje_catch_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola CATCH_POKEON",
					id_mensaje_recibido);

			devolver_catch_pokemon(socket_cliente, mensaje_catch_pokemon);
			log_trace(logger,
					"Se devolvio el mensaje CATCH_POKEMON con id asignado.");

			almacenar_en_cola_catch_pokemon(mensaje_catch_pokemon);
			log_trace(logger, "Se almaceno el mensaje CATCH_POKEMON en la cola.");

			//cachear_catch_pokemon(mensaje);

			//free (liberar memoria)
}

 int asignar_id_catch_pokemon(t_catch_pokemon* mensaje){
	 int id = get_id_mensajes();
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_catch_pokemon(int socket_cliente ,t_catch_pokemon* mensaje_catch_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_catch_pokemon(mensaje_catch_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, CATCH_POKEMON);
}


void almacenar_en_cola_catch_pokemon(t_catch_pokemon* mensaje){
	list_add(catch_pokemon->mensajes, mensaje);

	int size = list_size(catch_pokemon->mensajes);
	t_catch_pokemon* elemento_agregado = list_get(catch_pokemon->mensajes, size-1);
	log_trace(logger, "Se agrego a la cola CATCH_POKEMON el mensaje con id: %i.",
						elemento_agregado->id_mensaje);

	enviar_a_todos_los_subs_catch_pokemon(mensaje);


}

void enviar_a_todos_los_subs_catch_pokemon(t_catch_pokemon* mensaje){
	log_trace(logger, "Se van a enviar a todos los subs, el nuevo CATCH_POKEMON.");

	void enviar_catch_pokemon_a_suscriptor_aux(void* suscriptor_aux){
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_catch_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(catch_pokemon->subscriptores, enviar_catch_pokemon_a_suscriptor_aux);

	log_trace(logger, "Se va a enviar a todos los subs, el nuevo CATCH_POKEMON.");
}

void enviar_catch_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_catch_pokemon* mensaje){

	log_trace(logger, "Se va a enviar mensaje CATCH_POKEMON id: %i a sub: %i."
			, mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_catch_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, CATCH_POKEMON);

	log_info(logger, "Envio de CATCH_POKEMON %i a suscriptor %i", mensaje->id_mensaje, suscriptor->socket);

	log_trace(logger, "Se envio mensaje CATCH_POKEMON");
}


/*
 void cachear_catch_pokemon(t_mensaje_catch_pokemon mensaje){
 //TODO
 return 0;
 }
*/
