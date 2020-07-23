#include "gamecard.h"


void enviar_mensaje_suscripcion_gamecard(op_code mensaje, int conexion){
	t_subscriptor* mensaje_suscripcion;
		mensaje_suscripcion = crear_suscripcion(mensaje, -10);

		t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
		mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

		enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
		log_trace(logger, "Mensaje suscripcion enviado");
}

void enviar_appeared_pokemon_a_broker( t_appeared_pokemon* appeared_a_enviar) {
	log_trace(logger, "Mensaje APPEARED a enviar: %s.", mostrar_appeared_pokemon(appeared_a_enviar) );

	int socket_broker = iniciar_conexion_broker_gamecard();

	if (socket_broker > 0) {
		t_buffer*mensaje_appeared_serializado = serializar_appeared_pokemon(appeared_a_enviar);

		enviar_mensaje(socket_broker, mensaje_appeared_serializado, APPEARED_POKEMON);

		log_trace(logger, "Enviado appeared para: %s",
				appeared_a_enviar->pokemon);

		manejar_recibo_respuesta(socket_broker, recibir_codigo_operacion(socket_broker));

		close(socket_broker);
	} else {
		log_info(logger, "Error en comunicacion al intentar enviar appeared. Se Continua ejecucion");
	}
}


void enviar_caught_pokemon_a_broker( t_caught_pokemon* element) {
	t_caught_pokemon * caught_a_enviar = element;
	int socket_broker = iniciar_conexion_broker_gamecard();
		if (socket_broker > 0) {
			t_buffer*mensaje_caught_serializado = serializar_caught_pokemon(caught_a_enviar);

			enviar_mensaje(socket_broker, mensaje_caught_serializado, CAUGHT_POKEMON);

			log_trace(logger, "Enviado resultado de caught: %B",
					caught_a_enviar->ok_or_fail);

			manejar_recibo_respuesta(socket_broker, recibir_codigo_operacion(socket_broker));

			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar caught. Se efectuara operacion default");
		}
}

void enviar_localized_pokemon_a_broker( t_localized_pokemon* element) {
	t_localized_pokemon * localized_a_enviar = element;
	int socket_broker = iniciar_conexion_broker_gamecard();
		if (socket_broker > 0) {
			t_buffer*mensaje_localized_serializado = serializar_localized_pokemon(localized_a_enviar);

			enviar_mensaje(socket_broker, mensaje_localized_serializado, LOCALIZED_POKEMON);

			log_trace(logger, "Enviado localized para: %s. Cantidad: %d", // posiciones?
					localized_a_enviar->pokemon, localized_a_enviar->cantidad_posiciones);

			manejar_recibo_respuesta(socket_broker, recibir_codigo_operacion(socket_broker));

			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar localized. Se efectuara operacion default");
			//TODO: CHEQUEAR FUNCION DEFAULT
		}
}

void manejar_recibo_respuesta(int socket_broker, int cod_op){
	t_buffer * buffer = recibir_mensaje(socket_broker);
	char * cadena_mensaje;

	switch (cod_op) {
		case APPEARED_POKEMON:
			;
			t_appeared_pokemon * mensaje_appeared = deserializar_appeared_pokemon(buffer);

			cadena_mensaje = mostrar_appeared_pokemon(mensaje_appeared);
			break;

		case CAUGHT_POKEMON:
			;
			t_caught_pokemon* mensaje_caught= deserializar_caught_pokemon(buffer);

			cadena_mensaje =mostrar_caught_pokemon(mensaje_caught);
			break;

		case LOCALIZED_POKEMON:
			;
			t_localized_pokemon* mensaje_localized= deserializar_localized_pokemon(buffer);

			cadena_mensaje = mostrar_localized(mensaje_localized);
			break;

		default:
			log_error(logger, "Opcode inválido.");
			break;
	}

	log_trace(logger, "Mensaje %s llego correctamente al broker: %s. ", op_code_a_string(cod_op), cadena_mensaje);
}



