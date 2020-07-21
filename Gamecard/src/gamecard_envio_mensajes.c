#include "gamecard.h"


void enviar_mensaje_suscripcion_gamecard(op_code mensaje, int conexion){
	t_subscriptor* mensaje_suscripcion;
		mensaje_suscripcion = crear_suscripcion(mensaje, -10);

		t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
		mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

		enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
		log_trace(logger, "Mensaje suscripcion enviado");
}

void enviar_appeared_pokemon_a_broker( void *element) {
	t_appeared_pokemon * appeared_a_enviar = element;
	int socket_broker = iniciar_conexion_broker_gamecard();
		if (socket_broker > 0) {
			t_buffer*mensaje_appeared_serializado = serializar_appeared_pokemon(appeared_a_enviar);

			enviar_mensaje(socket_broker, mensaje_appeared_serializado, APPEARED_POKEMON);

			log_trace(logger, "Enviado appeared para: %s",
					appeared_a_enviar->pokemon);

			free(mensaje_appeared_serializado);
			/*
			appeared_a_enviar->id_mensaje = handle_mensajes_gamecard(
					socket_broker,recibir_codigo_operacion(socket_broker),1);
					 TODO : FIJARSE SI SE REQUIERE RESPUESTA*/
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

			free(mensaje_caught_serializado);

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
			free(mensaje_localized_serializado);

			/*localized_a_enviar->id_mensaje = handle_mensajes_gamecard(
					socket_broker,recibir_codigo_operacion(socket_broker),1);
			*/
			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar localized. Se efectuara operacion default");
			//TODO: CHEQUEAR FUNCION DEFAULT
		}
}




