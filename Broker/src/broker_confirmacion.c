#include "broker.h"

void manejar_mensaje_confirmacion(t_conexion_buffer *combo) {
	t_buffer * buffer = combo->buffer;
	int socket_cliente = combo->conexion;

	t_confirmacion* mensaje_confirmacion = deserializar_confirmacion(buffer);

	confirmar_cliente_recibio(mensaje_confirmacion, socket_cliente);

	confirmar_recepcion_en_cache(mensaje_confirmacion, socket_cliente);

	//free(mensaje_confirmacion);
	//free(buffer);
}

void confirmar_cliente_recibio(t_confirmacion* mensaje_confirmacion, int socket_cliente) {
	log_trace(logger,
			"Se confirmara la recepcion del mensaje: Tipo: %i, ID: %i, Cliente: %i.",
			mensaje_confirmacion->tipo_mensaje, mensaje_confirmacion->mensaje,
			socket_cliente);

	void si_coincide_cliente_agregar_id_recibido_aux(
			t_suscriptor_queue* suscriptor) {
		si_coincide_cliente_agregar_id_recibido(suscriptor, socket_cliente,
				mensaje_confirmacion->mensaje);
	}

	t_queue* cola_de_mensajes = get_cola_segun_tipo(
			mensaje_confirmacion->tipo_mensaje);

	list_iterate(cola_de_mensajes->subscriptores,
			(void*) si_coincide_cliente_agregar_id_recibido_aux);

	log_trace(logger, "Se confirmo la recepcion del mensaje");

	if (mensaje_recibido_por_todos_los_subs(mensaje_confirmacion)) {
		borrar_mensaje_de_cola(mensaje_confirmacion);
	}
}

void confirmar_recepcion_en_cache(t_confirmacion* mensaje_confirmacion, int socket_cliente){

	pthread_mutex_lock(&mutex_memoria_cache);

	void confirmar_recepcion_de_cliente(void* particion){
		if(mensaje_confirmacion->mensaje == ((t_mensaje_cache*) particion)->id){

			list_add( ((t_mensaje_cache*) particion)->subscribers_recibidos, (void*) socket_cliente);
			log_trace(logger, "Se confirmo la recepcion del mensaje %i en cache, por el suscriptor %i.", (((t_mensaje_cache*) particion)->id), socket_cliente);

		}
	}

	list_iterate(struct_admin_cache, confirmar_recepcion_de_cliente);

	pthread_mutex_unlock(&mutex_memoria_cache);

}

t_queue* get_cola_segun_tipo(int tipo_mensaje) {
	t_queue* cola_de_mensajes = malloc(sizeof(t_queue));
	switch (tipo_mensaje) {
	case APPEARED_POKEMON:
		cola_de_mensajes = appeared_pokemon;
		break;
	case NEW_POKEMON:
		cola_de_mensajes = new_pokemon;
		break;
	case CAUGHT_POKEMON:
		cola_de_mensajes = caught_pokemon;
		break;
	case CATCH_POKEMON:
		cola_de_mensajes = catch_pokemon;
		break;
	case GET_POKEMON:
		cola_de_mensajes = get_pokemon;
		break;
	case LOCALIZED_POKEMON:
		cola_de_mensajes = localized_pokemon;
		break;
	}
	return cola_de_mensajes;
}

void si_coincide_cliente_agregar_id_recibido(t_suscriptor_queue* suscriptor,
		int socket_suscriptor, int id_mensaje_recibido) {
	if (suscriptor->socket == socket_suscriptor) {
		log_trace(logger, "Se encontro al suscriptor %i.", socket_suscriptor);
		list_add(suscriptor->mensajes_recibidos, (void*) id_mensaje_recibido);

		int tamano_lista = list_size(suscriptor->mensajes_recibidos);
		log_trace(logger,
				"Se agrego el ID: %i a la lista de recibidos que tiene %i elementos.",
				list_get(suscriptor->mensajes_recibidos, tamano_lista - 1),
				tamano_lista);

		log_info(logger,
				"Confirmacion de recepcion de suscriptor %i al envio del mensaje %i.",
				socket_suscriptor, id_mensaje_recibido);
	}
}

_Bool mensaje_recibido_por_todos_los_subs(t_confirmacion* confirmacion) {

	_Bool fue_enviado_y_recibido_aux(void* suscriptor) {
		return fue_enviado_y_recibido(confirmacion->mensaje, suscriptor);
	}

	//Si todos los subs, tienen el mensaje en enviado && tiene el mensaje en recibido
	return list_all_satisfy(
			get_cola_segun_tipo(confirmacion->tipo_mensaje)->subscriptores,
			fue_enviado_y_recibido_aux);
}

void borrar_mensaje_de_cola(t_confirmacion* confirmacion) {
	log_trace(logger, "Se borrara el mensaje %i de la cola %i (Size: %i).",
			confirmacion->mensaje, confirmacion->tipo_mensaje,
			list_size(
					get_cola_segun_tipo(confirmacion->tipo_mensaje)->mensajes));

	switch (confirmacion->tipo_mensaje) {
	case APPEARED_POKEMON:
		;
		_Bool mensaje_distinto_appeared(void* mensaje) {
			return (((t_appeared_pokemon*) mensaje)->id_mensaje
					!= confirmacion->mensaje);
		}

		appeared_pokemon->mensajes = list_filter(appeared_pokemon->mensajes,
				mensaje_distinto_appeared);

		break;
	case NEW_POKEMON:
		;
		_Bool mensaje_distinto_new(void* mensaje) {
			return (((t_new_pokemon*) mensaje)->id_mensaje
					!= confirmacion->mensaje);
		}

		new_pokemon->mensajes = list_filter(new_pokemon->mensajes,
				mensaje_distinto_new);

		break;
	case CATCH_POKEMON:
		;
		_Bool mensaje_distinto_catch(void* mensaje) {
			return (((t_catch_pokemon*) mensaje)->id_mensaje
					!= confirmacion->mensaje);
		}

		catch_pokemon->mensajes = list_filter(catch_pokemon->mensajes,
				mensaje_distinto_catch);

		break;
	case CAUGHT_POKEMON:
		;
		_Bool mensaje_distinto_caught(void* mensaje) {
			return (((t_caught_pokemon*) mensaje)->id_mensaje
					!= confirmacion->mensaje);
		}

		caught_pokemon->mensajes = list_filter(caught_pokemon->mensajes,
				mensaje_distinto_catch);

		break;
	case GET_POKEMON:
		;
		_Bool mensaje_distinto_get(void* mensaje) {
			return (((t_get_pokemon*) mensaje)->id_mensaje
					!= confirmacion->mensaje);
		}

		get_pokemon->mensajes = list_filter(get_pokemon->mensajes,
				mensaje_distinto_get);

		break;
	case LOCALIZED_POKEMON:
		;
		_Bool mensaje_distinto_localized(void* mensaje) {
			return (((t_localized*) mensaje)->id_mensaje
					!= confirmacion->mensaje);
		}

		localized_pokemon->mensajes = list_filter(localized_pokemon->mensajes,
				mensaje_distinto_localized);

		break;
	}

	log_trace(logger, "Se borro el mensaje %i de la cola %i (Size: %i).",
			confirmacion->mensaje, confirmacion->tipo_mensaje,
			list_size(
					get_cola_segun_tipo(confirmacion->tipo_mensaje)->mensajes));

}

_Bool fue_enviado_y_recibido(int id_mensaje, t_suscriptor_queue* suscriptor) {

	_Bool tiene_mensaje(void* un_id) {
		int* un_id_aux = un_id;
		return ((int) un_id_aux == id_mensaje);
	}

	if (list_any_satisfy(suscriptor->mensajes_enviados, tiene_mensaje)) { //Si alguno de los enviados es el id_mensaje
		return list_any_satisfy(suscriptor->mensajes_recibidos, tiene_mensaje); // Retorna true si alguno de los recibidos es el id_mensaje
	} else {
		return true; // Si directamente no fue enviado, para ese sub no importa, devuelvo true.
	}

	return NULL;
}
