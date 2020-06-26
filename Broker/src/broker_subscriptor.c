#include "broker.h"

void manejar_mensaje_suscriptor(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
	int socket_cliente= combo->conexion;

	t_subscriptor* suscripcion = deserializar_suscripcion(buffer);

	log_trace(logger, "Suscripcion Cola: %i , Tiempo: %i.", suscripcion->cola_de_mensaje, suscripcion->tiempo);

	log_info(logger, "Suscripcion de %i a la cola %i.", socket_cliente,suscripcion->cola_de_mensaje);

	subscribir(socket_cliente, suscripcion);

	enviar_mensajes_cacheados_a_cliente(suscripcion, socket_cliente);

	desuscribir(socket_cliente, suscripcion);

	//free (liberar memoria)
}

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

	 log_trace(logger, "Cliente %i suscripto.", cliente);
 }

void agregar_cliente_a_cola(t_queue* cola, int cliente){
	t_suscriptor_queue* sub_a_agregar = malloc(sizeof(t_suscriptor_queue));
	sub_a_agregar->socket = cliente;
	sub_a_agregar->mensajes_enviados = list_create();
	sub_a_agregar->mensajes_recibidos = list_create();
	list_add(cola->subscriptores, sub_a_agregar);
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

void desuscribir(int cliente, t_subscriptor* suscripcion){

	if((suscripcion->tiempo) >= 0){ //Desuscribir si el parametro 'tiempo' es > a 0, si es menor, dejar suscripto para siempre

		log_trace(logger, "Adentro de desuscribir");

		for(int i = (suscripcion->tiempo); i>0 ; i--){
			log_trace(logger, "Desuscribir en %i...", i);
			sleep(1);
		}

		switch (suscripcion->cola_de_mensaje) {
			case APPEARED_POKEMON:
				log_trace(logger, "Suscribir a %i a cola APPEARED_POKEMON.", cliente);
				sacar_cliente_a_cola(appeared_pokemon, cliente);

				break;

			case NEW_POKEMON:
				log_trace(logger, "Suscribir a %i a cola NEW_POKEMON", cliente);
				sacar_cliente_a_cola(new_pokemon, cliente);

				break;

			case CATCH_POKEMON:
				log_trace(logger, "Suscribir a %i a cola CATCH_POKEMON", cliente);
				sacar_cliente_a_cola(catch_pokemon, cliente);

				break;

			case CAUGHT_POKEMON:
				log_trace(logger, "Suscribir a %i a cola CAUGHT_POKEMON", cliente);
				sacar_cliente_a_cola(caught_pokemon, cliente);

				break;

			case GET_POKEMON:
				log_trace(logger, "Suscribir a %i a cola GET_POKEMON", cliente);
				sacar_cliente_a_cola(get_pokemon, cliente);

				break;

			case LOCALIZED_POKEMON:
				log_trace(logger, "Suscribir a %i a cola LOCALIZED_POKEMON", cliente);
				sacar_cliente_a_cola(localized_pokemon, cliente);

				break;
		}

		log_trace(logger, "Ya esta desuscripto.");

	}
}

void sacar_cliente_a_cola(t_queue* cola, int cliente){

	_Bool es_el_mismo_cliente(void* argumento){
		t_suscriptor_queue* cliente_en_cola = argumento;

		int rtn = (cliente == (cliente_en_cola->socket));
		if(rtn) log_trace(logger, "Cliente encontrado.");
		return rtn;
	}

	list_remove_by_condition( (cola->subscriptores), es_el_mismo_cliente);
	log_trace(logger, "Se desuscribio.");
 }

