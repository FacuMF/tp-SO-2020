#include "broker.h"

void confirmar_cliente_recibio(t_confirmacion* mensaje_confirmacion, int socket_cliente){
	log_trace(logger, "Se confirmara la recepcion del mensaje: Tipo: %i, ID: %i, Cliente: %i.",
			mensaje_confirmacion->tipo_mensaje, mensaje_confirmacion->mensaje, socket_cliente);

	void si_coincide_cliente_agregar_id_recibido_aux(t_suscriptor_queue* suscriptor){
		si_coincide_cliente_agregar_id_recibido(suscriptor, socket_cliente, mensaje_confirmacion->mensaje);
	}

	t_queue* cola_de_mensajes = get_cola_segun_tipo(mensaje_confirmacion->tipo_mensaje);

	list_iterate( cola_de_mensajes->subscriptores, si_coincide_cliente_agregar_id_recibido_aux );

	log_trace(logger, "Se confirmo la recepcion del mensaje");
	// TODO
	// Borrar los mensajes que todos sus subs confirmaron. (Chekear que onda los cacheados,
	// se confirma tambien su envio? cada vez que confirmo el envio de un msj tengo que
	// modificar tambien la info del cacheado? Etc.
}

t_queue* get_cola_segun_tipo(int tipo_mensaje){
	t_queue* cola_de_mensajes = malloc(sizeof(t_queue));
	switch(tipo_mensaje){
		case APPEARED_POKEMON:
			cola_de_mensajes = appeared_pokemon;
			break;
		case NEW_POKEMON:
			cola_de_mensajes =  new_pokemon;
			break;
		case CAUGHT_POKEMON:
			cola_de_mensajes =  caught_pokemon;
			break;
		case CATCH_POKEMON:
			cola_de_mensajes =  catch_pokemon;
			break;
		case GET_POKEMON:
			cola_de_mensajes =  get_pokemon;
			break;
		case LOCALIZED_POKEMON:
			cola_de_mensajes =  localized_pokemon;
			break;
	}
	return cola_de_mensajes;
}

void si_coincide_cliente_agregar_id_recibido(t_suscriptor_queue* suscriptor, int socket_suscriptor, int id_mensaje_recibido) {
	if(suscriptor->socket == socket_suscriptor){
		log_trace(logger, "Se encontro al suscriptor %i.", socket_suscriptor);
		list_add(suscriptor->mensajes_recibidos, id_mensaje_recibido);
		log_trace(logger, "Se confirmo que el suscriptor %i recibio el mensaje ID: %i.", socket_suscriptor, id_mensaje_recibido);
	}
}
