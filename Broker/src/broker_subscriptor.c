#include "broker.h"

void manejar_mensaje_suscriptor(t_conexion_buffer *combo) {
	t_buffer * buffer = malloc(sizeof(t_buffer));
	int socket_cliente= combo->conexion;

	memcpy(buffer, combo->buffer, sizeof(t_buffer));
	free(combo->buffer);
	free(combo);


	t_subscriptor* suscripcion = deserializar_suscripcion(buffer);

	log_trace(logger, "Suscripcion Cola: %i , Tiempo: %i.", suscripcion->cola_de_mensaje, suscripcion->tiempo);

	log_info(logger, "Suscripcion de %i a la cola %i.", socket_cliente,suscripcion->cola_de_mensaje);

	subscribir(socket_cliente, suscripcion);

	enviar_mensajes_cacheados_a_cliente(suscripcion, socket_cliente);

	log_debug(logger, "Se va a intentar desuscribir. ");

	desuscribir(socket_cliente, suscripcion);

	liberar_suscripcion(suscripcion);


}

 void subscribir(int cliente, t_subscriptor* subscripcion){
	 char* tipo_mensaje = op_code_a_string(subscripcion->cola_de_mensaje);
	log_trace(logger, "Suscribir a %i a cola %s.", cliente, tipo_mensaje);
	free(tipo_mensaje);

	pthread_mutex_lock(&mutex_suscribir);

	switch (subscripcion->cola_de_mensaje) {
     	 	case APPEARED_POKEMON:
     	 		agregar_cliente_a_cola(appeared_pokemon, cliente);
     	 		break;
     		case NEW_POKEMON:
     			agregar_cliente_a_cola(new_pokemon, cliente);
     			break;
			case CATCH_POKEMON:
				agregar_cliente_a_cola(catch_pokemon, cliente);
				break;
     		case CAUGHT_POKEMON:
     			agregar_cliente_a_cola(caught_pokemon, cliente);
     			break;
     		case GET_POKEMON:
     			agregar_cliente_a_cola(get_pokemon, cliente);
     			break;
     		case LOCALIZED_POKEMON:
     			agregar_cliente_a_cola(localized_pokemon, cliente);
     			break;
     }

	pthread_mutex_unlock(&mutex_suscribir);

	 log_trace(logger, "Cliente %i suscripto.", cliente);
 }

void agregar_cliente_a_cola(t_list* cola, int cliente){
	int sub_a_agregar= cliente;

	list_add(cola, (void*)sub_a_agregar);
	int size = list_size(cola);
	int elemento_agregado = (int) list_get(cola, size-1);
	log_trace(logger, "Se agrego %i a la cola en la posicion %i.", elemento_agregado, size);
 }

void enviar_fin_de_mensajes(int socket_cliente){
	int fin = -2;
	void* fin_serializado = malloc(sizeof(int));
	memcpy(fin_serializado, &fin, sizeof(int));
	int b = send(socket_cliente, fin_serializado, sizeof(int),0);
	log_trace(logger, "Los bytes transmitidos fueron %d", b);
	free(fin_serializado);
}

void desuscribir(int cliente, t_subscriptor* suscripcion){

	char* tipo_mensaje = op_code_a_string(suscripcion->cola_de_mensaje);
	if((suscripcion->tiempo) >= 0){ //Desuscribir si el parametro 'tiempo' es > a 0, si es menor, dejar suscripto para siempre
		for(int i = (suscripcion->tiempo); i>0 ; i--){
			log_trace(logger, "Desuscribir en %i...", i);
			sleep(1);
		}
		enviar_fin_de_mensajes(cliente);
		log_trace(logger, "Desuscribir a %i de cola %s.", cliente, tipo_mensaje);
		free(tipo_mensaje);
		switch (suscripcion->cola_de_mensaje) {
			case APPEARED_POKEMON:
				sacar_cliente_a_cola(appeared_pokemon, cliente);
				break;
			case NEW_POKEMON:
				sacar_cliente_a_cola(new_pokemon, cliente);
				break;
			case CATCH_POKEMON:
				sacar_cliente_a_cola(catch_pokemon, cliente);
				break;
			case CAUGHT_POKEMON:
				sacar_cliente_a_cola(caught_pokemon, cliente);
				break;
			case GET_POKEMON:
				sacar_cliente_a_cola(get_pokemon, cliente);
				break;
			case LOCALIZED_POKEMON:
				sacar_cliente_a_cola(localized_pokemon, cliente);
				break;
		}
		log_trace(logger, "Ya esta desuscripto.");
	}
}

void sacar_cliente_a_cola(t_list* cola, int cliente){

	_Bool es_el_mismo_cliente(void* cliente_en_cola){

		int rtn = (cliente == ((int)cliente_en_cola));
		if(rtn) log_trace(logger, "Cliente encontrado.");
		return rtn;
	}

	list_remove_by_condition( (cola), es_el_mismo_cliente);
	log_trace(logger, "Se desuscribio.");
 }

