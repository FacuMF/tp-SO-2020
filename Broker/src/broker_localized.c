#include "broker.h"

void manejar_mensaje_localized(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
		int socket_cliente= combo->conexion;
	t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);

			int id_mensaje_recibido = asignar_id_localized_pokemon(mensaje_localized_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola LOCALIZED_POKEON",
					id_mensaje_recibido);

			devolver_localized_pokemon(socket_cliente, mensaje_localized_pokemon);
			log_trace(logger,
					"Se devolvio el mensaje LOCALIZED_POKEMON con id asignado.");

			almacenar_en_cola_localized_pokemon(mensaje_localized_pokemon);
			log_trace(logger, "Se almaceno el mensaje LOCALIZED_POKEMON en la cola.");

			//cachear_localized_pokemon(mensaje);

			//free (liberar memoria)
}

 int asignar_id_localized_pokemon(t_localized* mensaje){
	 int id = get_id_mensajes();
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_localized_pokemon(int socket_cliente ,t_localized* mensaje_localized_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_localized_pokemon(mensaje_localized_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, LOCALIZED_POKEMON);
}


void almacenar_en_cola_localized_pokemon(t_localized* mensaje){
	list_add(localized_pokemon->mensajes, mensaje);

	int size = list_size(localized_pokemon->mensajes);
	t_localized* elemento_agregado = list_get(localized_pokemon->mensajes, size-1);
	log_trace(logger, "Se agrego a la cola LOCALIZED_POKEMON el mensaje con id: %i.",
						elemento_agregado->id_mensaje);

	enviar_a_todos_los_subs_localized_pokemon(mensaje);


}

void enviar_a_todos_los_subs_localized_pokemon(t_localized* mensaje){
	log_trace(logger, "Se van a enviar a todos los subs, el nuevo LOCALIZED_POKEMON.");

	void enviar_localized_pokemon_a_suscriptor_aux(void* suscriptor_aux){
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_localized_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(localized_pokemon->subscriptores, enviar_localized_pokemon_a_suscriptor_aux);

	log_trace(logger, "Se va a enviar a todos los subs, el nuevo LOCALIZED_POKEMON.");
}

void enviar_localized_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_localized* mensaje){

	log_trace(logger, "Se va a enviar mensaje LOCALIZED_POKEMON id: %i a sub: %i."
			, mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_localized_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, LOCALIZED_POKEMON);

	log_info(logger, "Envio de LOCALIZED_POKEMON %i a suscriptor %i", mensaje->id_mensaje, suscriptor->socket);

	log_trace(logger, "Se envio mensaje LOCALIZED_POKEMON");
}
