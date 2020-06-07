#include "broker.h"


 void subscribir(int cliente, t_subscriptor* subscripcion){
     switch (subscripcion->cola_de_mensaje) {
     	 	case APPEARED_POKEMON:
     		log_trace(logger, "Suscribir a %i a cola APPEARED_POKEMON.", cliente);
     		agregar_cliente_a_cola(appeared_pokemon, cliente);

     		break;

     		case NEW_POKEMON:
     		log_trace(logger, "Suscribir a %i a cola NEW_POKEMON", cliente);
     		agregar_cliente_a_cola(new_pokemon, cliente);

     		break;

     		case CATCH_POKEMON:
     		log_trace(logger, "Suscribir a %i a cola CATCH_POKEMON", cliente);
     		agregar_cliente_a_cola(catch_pokemon, cliente);

     		break;

     		case CAUGHT_POKEMON:
     		log_trace(logger, "Suscribir a %i a cola CAUGHT_POKEMON", cliente);
     		agregar_cliente_a_cola(caught_pokemon, cliente);

     		break;

     		case GET_POKEMON:
     		log_trace(logger, "Suscribir a %i a cola GET_POKEMON", cliente);
     		agregar_cliente_a_cola(get_pokemon, cliente);

     		break;

     		case LOCALIZED_POKEMON:
     		log_trace(logger, "Suscribir a %i a cola LOCALIZED_POKEMON", cliente);
     		agregar_cliente_a_cola(localized_pokemon, cliente);

     		break;

     }

	 log_trace(logger, "Adentro de suscribir");
 }

void agregar_cliente_a_cola(t_queue* cola, int cliente){
	t_suscriptor_queue* cliente_a_agregar = malloc(sizeof(t_suscriptor_queue));
	cliente_a_agregar->socket = cliente;
	list_add(cola->subscriptores, cliente_a_agregar);
	int size = list_size(cola->subscriptores);
	t_suscriptor_queue* elemento_agregado = list_get(cola->subscriptores, size-1);

	log_trace(logger, "Se agrego %i a la cola en la posicion %i.", elemento_agregado->socket, size);
 }


void enviar_mensajes_de_suscripcion_a_cliente(t_subscriptor* subscripcion,  int cliente){
	switch (subscripcion->cola_de_mensaje) {
	case APPEARED_POKEMON:
		//enviar_mensajes_de_cola_a_cliente(appeared_pokemon, cliente);
		break;

	case NEW_POKEMON:
		//enviar_mensajes_de_cola_a_cliente(new_pokemon, cliente);
		break;

	case CATCH_POKEMON:
		//enviar_mensajes_de_cola_a_cliente(catch_pokemon, cliente);
		break;

	case CAUGHT_POKEMON:
		//enviar_mensajes_de_cola_a_cliente(caught_pokemon, cliente);
		break;

	case GET_POKEMON:
		//enviar_mensajes_de_cola_a_cliente(get_pokemon, cliente);
		break;

	case LOCALIZED_POKEMON:
		//enviar_mensajes_de_cola_a_cliente(localized_pokemon, cliente);
		break;
	}
}

/*
void enviar_mensajes_de_cola_a_cliente(t_queue* cola,  int cliente){

	void enviar_mensaje_de_cola_aux(void* mensaje) {
		//enviar_mensaje_de_cola(mensaje, ciente);
	}

}
*/
