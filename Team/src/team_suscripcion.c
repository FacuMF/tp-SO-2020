#include "team.h"


// SUSCRIPCION
void suscribirse_a_colas_necesarias() {
	enviar_suscripcion_broker(APPEARED_POKEMON);
	enviar_suscripcion_broker(LOCALIZED_POKEMON);
	enviar_suscripcion_broker(CAUGHT_POKEMON);
}

void enviar_suscripcion_broker(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_con_broker();

	enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);

	int* argument = malloc(sizeof(int));
	*argument = socket_broker;
	pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);

	log_trace(logger, "Suscripcion completada");
}

void enviar_mensaje_suscripcion(op_code mensaje, int conexion) {
	t_subscriptor* mensaje_suscripcion;
	mensaje_suscripcion = crear_suscripcion(mensaje, -10);

	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

	enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
	log_trace(logger, "Mensaje suscripcion enviado");
}

// RECEPCION MENSAJES

void manejar_recibo_mensajes(int conexion, op_code cod_op, int es_respuesta) {
	t_buffer * buffer = recibir_mensaje(conexion);
	int id_mensaje;

	switch (cod_op) {
	case APPEARED_POKEMON:
		;
		t_appeared_pokemon * mensaje_appeared = deserializar_appeared_pokemon(buffer);
		id_mensaje = mensaje_appeared->id_mensaje;
		log_info(logger, "Mensaje APPEARED_POKEMON: %s",mostrar_appeared_pokemon(mensaje_appeared));

		break;
	case CAUGHT_POKEMON:
		;
		t_caught_pokemon* mensaje_caught= deserializar_caught_pokemon(buffer);
		id_mensaje = mensaje_caught->id_mensaje;
		log_info(logger, "Mensaje CAUGHT_POKEMON: %s", mostrar_caught_pokemon(mensaje_caught));

		break;
	case LOCALIZED_POKEMON:
		;
		t_localized* mensaje_localized= deserializar_localized_pokemon(buffer);
		id_mensaje = mensaje_localized->id_mensaje;
		log_info(logger, "Mensaje LOCALIZED_POKEMON: %s",mostrar_localized(mensaje_localized));

		break;
	case GET_POKEMON:
		;
		t_get_pokemon* mensaje_get= deserializar_get_pokemon(buffer);
		id_mensaje = mensaje_get->id_mensaje;

		log_trace(logger,"Recepcion id_mensaje: %d",id_mensaje);

		break;
	case CATCH_POKEMON:
		;
		t_catch_pokemon* mensaje_catch= deserializar_catch_pokemon(buffer);
		id_mensaje = mensaje_catch->id_mensaje;

		log_trace(logger,"Recepcion id_mensaje: %d",id_mensaje);

		break;
	default:
		log_error(logger, "Opcode inválido.");
		break;
	}

	if (es_respuesta) {
		list_add(ids_mensajes_utiles, &id_mensaje);
	} else {
		confirmar_recepcion(conexion, cod_op, id_mensaje);

		log_trace(logger, "Recepcion confirmada: %d %d %d", conexion, cod_op, id_mensaje);
	}

	log_trace(logger, "Mensaje recibido manejado.");
}

