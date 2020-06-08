#include "broker.h"

 int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje){
	 int id = id_appeared_pokemon;
	 id_appeared_pokemon++;
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_appeared_pokemon(int socket_cliente ,t_appeared_pokemon* mensaje_appeared_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, APPEARED_POKEMON);
}


void almacenar_en_cola_appeared_pokemon(t_appeared_pokemon* mensaje){
	list_add(appeared_pokemon->mensajes, mensaje);

	int size = list_size(appeared_pokemon->mensajes);
	t_appeared_pokemon* elemento_agregado = list_get(appeared_pokemon->mensajes, size-1);
	log_trace(logger, "Se agrego a la cola APPEARED_POKEMON el mensaje con id: %i.",
						elemento_agregado->id_mensaje);

	enviar_a_todos_los_subs_appeared_pokemon(mensaje); //TODO


}

void enviar_a_todos_los_subs_appeared_pokemon(t_appeared_pokemon* mensaje){
	log_trace(logger, "Se van a enviar a todos los subs, el nuevo APPEARED_POKEMON.");

	void enviar_appeared_pokemon_a_suscriptor_aux(void* suscriptor_aux){
		t_suscriptor_queue* suscriptor = suscriptor_aux;
		enviar_appeared_pokemon_a_suscriptor(suscriptor, mensaje);
	}

	list_iterate(appeared_pokemon->subscriptores, enviar_appeared_pokemon_a_suscriptor_aux);

	log_trace(logger, "Se van a envio a todos los subs, el nuevo APPEARED_POKEMON.");
}

void enviar_appeared_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_appeared_pokemon* mensaje){

	log_trace(logger, "Se va a enviar mensaje APPEARED_POKEMON id: %i a sub: %i."
			, mensaje->id_mensaje, suscriptor->socket);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje);
	enviar_mensaje(suscriptor->socket, mensaje_serializado, APPEARED_POKEMON);
	log_trace(logger, "Se envio mensaje APPEARED_POKEMON");

	recibir_mensaje_del_cliente(&(suscriptor->socket));
}


/*
 void cachear_appeared_pokemon(t_mensaje_appeared_pokemon mensaje){
 //TODO
 return 0;
 }
*/
