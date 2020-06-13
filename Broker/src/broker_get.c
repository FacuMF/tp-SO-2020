#include "broker.h"

 int asignar_id_get_pokemon(t_get_pokemon* mensaje){
	 int id = get_id_mensajes();
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_get_pokemon(int socket_cliente ,t_get_pokemon* mensaje_get_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje_get_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, GET_POKEMON);
}


void almacenar_en_cola_get_pokemon(t_get_pokemon* mensaje){
	list_add(get_pokemon->mensajes, mensaje);

	int size = list_size(get_pokemon->mensajes);
	t_get_pokemon* elemento_agregado = list_get(get_pokemon->mensajes, size-1);
	log_trace(logger, "Se agrego a la cola GET_POKEMON el mensaje con id: %i.",
						elemento_agregado->id_mensaje);

	enviar_a_todos_los_subs_get_pokemon(mensaje);


}

void enviar_a_todos_los_subs_get_pokemon(t_get_pokemon* mensaje){
	log_trace(logger, "Se van a enviar a todos los subs, el nuevo GET_POKEMON.");

	void enviar_get_pokemon_a_suscriptor_aux(void* suscriptor_aux){
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_get_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(get_pokemon->subscriptores, enviar_get_pokemon_a_suscriptor_aux);

	log_trace(logger, "Se va a enviar a todos los subs, el nuevo GET_POKEMON.");
}

void enviar_get_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_get_pokemon* mensaje){

	log_trace(logger, "Se va a enviar mensaje GET_POKEMON id: %i a sub: %i."
			, mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, GET_POKEMON);

	log_info(logger, "Envio de GET_POKEMON %i a suscriptor %i", mensaje->id_mensaje, suscriptor->socket);

	log_trace(logger, "Se envio mensaje GET_POKEMON");

	recibir_mensaje_del_cliente(&(suscriptor->socket));
}


/*
 void cachear_get_pokemon(t_mensaje_get_pokemon mensaje){
 //TODO
 return 0;
 }
*/
