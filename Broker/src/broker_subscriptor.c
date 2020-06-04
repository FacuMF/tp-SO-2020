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
	int* cliente_a_agregar = cliente;
	list_add(cola->subscriptores, cliente_a_agregar);
	int size = list_size(cola->subscriptores);
	int elemento_agregado = (int) list_get(cola->subscriptores, size-1);

	log_trace(logger, "Se agrego %i a la cola en la posicion %i.", elemento_agregado, size);
 }

/*
 void enviar_mensajes_de_cola(t_subscriptor subscripcion){
 //TODO
 return 0;
 }
 */
