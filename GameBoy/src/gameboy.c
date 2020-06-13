#include "gameboy.h"

int main(int argv, char* arg[]) {

	inicializar_gameboy();

	t_modulo modulo = string_a_modulo(arg[1]);

	if(modulo == SUSCRIPTOR){es_suscriptor = 1;}else{ es_suscriptor = 0; }

	op_code tipo_mensaje;
	if(modulo == -1){
		log_error(logger, "El modulo ingresado es incorrecto.");
	}else if(modulo == SUSCRIPTOR){
		modulo = broker;
		tipo_mensaje = string_a_tipo_mensaje(arg[1]);
	} else {
		tipo_mensaje = string_a_tipo_mensaje(arg[2]);
	}

	log_trace(logger, "Caracteristicas de mensaje obtenidas");

	if (tipo_mensaje == -1)
		log_error(logger, "Tipo de mensaje invalido");
	else
		log_trace(logger,
				"Se quiere enviar un mensaje del tipo -%i- al modulo -%i-",
				tipo_mensaje, modulo);

	////// Conectar con quien corresponda (iniciar conexion) /////
	char* ip = leer_ip(modulo, config);
	char* puerto = leer_puerto(modulo, config);
	/*TBR*/log_trace(logger,
			"Leido de config por parametro %s. Ip: %s y Puerto: %s", arg[1], ip,
			puerto);

	int conexion;

	conexion = iniciar_conexion(ip, puerto);
	log_trace(logger, "Conexion Creada. Ip: %s y Puerto: %s ", ip, puerto);

	////// Crear y Serializar mensaje //////
	t_buffer* mensaje_serializado = mensaje_a_enviar(modulo, tipo_mensaje, arg);
	log_trace(logger, "El mensaje fue serializado. ");

	////// Enviar mensaje //////
	log_trace(logger, "OPERATION CODE: %i", tipo_mensaje);
	enviar_mensaje(conexion, mensaje_serializado, tipo_mensaje);
	log_trace(logger, "El mensaje fue enviado.");

	free(mensaje_serializado->stream);
	free(mensaje_serializado);

	// Esperar respuesta //

	int cod_op_respuesta = 0;

	if(tipo_mensaje == SUSCRIPTOR) {
		// Si el mensaje que se envio fue una suscripcion, quiero recibir todos los mensjaes de esa cola.
		while(cod_op_respuesta>=0){
			cod_op_respuesta = recibir_respuesta(&conexion);
		}
		log_warning(logger, "Se salio del while del socket %i.", conexion);
	} else {
		// Para el resto de mensajes, se va a recibir el mismo mensaje pero con el id asignado por el broker.
		recibir_respuesta(&conexion);
	}

}

void inicializar_gameboy() {
	// Leer configuracion
	config = leer_config("./GameBoy/config/gameboy.config");
	string_nivel_log_minimo = config_get_string_value(config,
			"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);

	//Inicio logger
	logger = iniciar_logger("./GameBoy/config/gameboy.log", "GameBoy",
			log_nivel_minimo);
}

int recibir_respuesta(int* socket_broker){
	int cod_op = recibir_codigo_operacion(*socket_broker);
	(cod_op == -1)? log_error(logger, "Error en 'recibir_codigo_operacion'") :
			 	 	 log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);

	(cod_op>=0)? handle_respuesta(cod_op, *socket_broker):
			log_warning(logger, "El broker cerro el socket %i.", *socket_broker);
	return cod_op;
}

void handle_respuesta(int cod_op, int socket_broker){
	t_buffer * buffer = recibir_mensaje(socket_broker);
	switch (cod_op) {
		case APPEARED_POKEMON:

			log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
			t_appeared_pokemon* mensaje_appeared_pokemon = deserializar_appeared_pokemon(buffer);
			if ( es_suscriptor ) {
				log_trace(logger, "Se confirmara la recepcion.");
				confirmar_recepcion(socket_broker, cod_op, mensaje_appeared_pokemon->id_mensaje);
				log_trace(logger, "Recepcion confirmada.");
			}
		break;
		case CAUGHT_POKEMON:

			log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");
			t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);
			if ( es_suscriptor ) {
				log_trace(logger, "Se confirmara la recepcion.");
				confirmar_recepcion(socket_broker, cod_op, mensaje_caught_pokemon->id_mensaje);
				log_trace(logger, "Recepcion confirmada.");
			}

		break;
		case LOCALIZED_POKEMON:

			log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");
			t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);
			if ( es_suscriptor ) {
				log_trace(logger, "Se confirmara la recepcion.");
				confirmar_recepcion(socket_broker, cod_op, mensaje_localized_pokemon->id_mensaje);
				log_trace(logger, "Recepcion confirmada.");
			}

			break;
		case NEW_POKEMON:

			log_trace(logger, "Se recibio un mensaje NEW_POKEMON");
			t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
			if ( es_suscriptor ) {
				log_trace(logger, "Se confirmara la recepcion.");
				confirmar_recepcion(socket_broker, cod_op, mensaje_new_pokemon->id_mensaje);
				log_trace(logger, "Recepcion confirmada.");
			}
			break;
		case GET_POKEMON:

			log_trace(logger, "Se recibio un mensaje GET_POKEMON");
			t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
			if ( es_suscriptor ) {
				log_trace(logger, "Se confirmara la recepcion.");
				confirmar_recepcion(socket_broker, cod_op, mensaje_get_pokemon->id_mensaje);
				log_trace(logger, "Recepcion confirmada.");
			}
			break;
		case CATCH_POKEMON:

			log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");
			t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(buffer);
			if ( es_suscriptor ) {
				log_trace(logger, "Se confirmara la recepcion.");
				confirmar_recepcion(socket_broker, cod_op, mensaje_catch_pokemon->id_mensaje);
				log_trace(logger, "Recepcion confirmada.");
			}
			break;
		default:
			log_error(logger,"Opcode inválido.");
			break;
	}
	log_trace(logger,"Mensaje recibido manejado.");
}
	/*
	switch (cod_op) {
			case APPEARED_POKEMON:

				log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
				buffer = recibir_mensaje(socket_broker);
				t_appeared_pokemon* mensaje_appeared_pokemon = deserializar_appeared_pokemon(buffer);
				log_trace(logger, "ID asignado a APPEARED_POKEMON: %i.", mensaje_appeared_pokemon->id_mensaje);

				//Confirmar Recepcion si es Suscripcion //TODO pasar a otra funcion.
				if ( es_suscriptor ) {
					log_trace(logger, "Se confirmara la recepcion.");
					confirmar_recepcion(socket_broker, cod_op, mensaje_appeared_pokemon->id_mensaje);
					log_trace(logger, "Recepcion confirmada.");
				}

			break;
			case NEW_POKEMON:

				log_trace(logger, "Se recibio un mensaje NEW_POKEMON");
				buffer = recibir_mensaje(socket_broker);
				t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
				log_trace(logger, "ID asignado a NEW_POKEMON: %i.", mensaje_new_pokemon->id_mensaje);

				//Confirmar Recepcion si es Suscripcion //TODO pasar a otra funcion.
				if ( es_suscriptor ) {
					log_trace(logger, "Se confirmara la recepcion.");
					confirmar_recepcion(socket_broker, cod_op, mensaje_new_pokemon->id_mensaje);
					log_trace(logger, "Recepcion confirmada.");
				}

			break;
			case CATCH_POKEMON:

				log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");
				buffer = recibir_mensaje(socket_broker);
				t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(buffer);
				log_trace(logger, "ID asignado a CATCH_POKEMON: %i.", mensaje_catch_pokemon->id_mensaje);

				//Confirmar Recepcion si es Suscripcion //TODO pasar a otra funcion.
				if ( es_suscriptor ) {
					log_trace(logger, "Se confirmara la recepcion.");
					confirmar_recepcion(socket_broker, cod_op, mensaje_catch_pokemon->id_mensaje);
					log_trace(logger, "Recepcion confirmada.");
				}

			break;
			case CAUGHT_POKEMON:

				log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");
				buffer = recibir_mensaje(socket_broker);
				t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);
				log_trace(logger, "ID asignado a CAUGHT_POKEMON: %i.", mensaje_caught_pokemon->id_mensaje);

				//Confirmar Recepcion si es Suscripcion //TODO pasar a otra funcion.
				if ( es_suscriptor ) {
					log_trace(logger, "Se confirmara la recepcion.");
					confirmar_recepcion(socket_broker, cod_op, mensaje_caught_pokemon->id_mensaje);
					log_trace(logger, "Recepcion confirmada.");
				}

			break;
			case GET_POKEMON:

				log_trace(logger, "Se recibio un mensaje GET_POKEMON");
		        buffer = recibir_mensaje(socket_broker);
				t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
				log_trace(logger, "ID asignado a GET_POKEMON: %i.", mensaje_get_pokemon->id_mensaje);

				//Confirmar Recepcion si es Suscripcion //TODO pasar a otra funcion.
				if ( es_suscriptor ) {
					log_trace(logger, "Se confirmara la recepcion.");
					confirmar_recepcion(socket_broker, cod_op, mensaje_get_pokemon->id_mensaje);
					log_trace(logger, "Recepcion confirmada.");
				}

			break;
			case LOCALIZED_POKEMON:

				log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");
		        buffer = recibir_mensaje(socket_broker);
				t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);
				log_trace(logger, "ID asignado a LOCALIZED_POKEMON: %i.", mensaje_localized_pokemon->id_mensaje);

				//Confirmar Recepcion si es Suscripcion //TODO pasar a otra funcion.
				if ( es_suscriptor ) {
					log_trace(logger, "Se confirmara la recepcion.");
					confirmar_recepcion(socket_broker, cod_op, mensaje_localized_pokemon->id_mensaje);
					log_trace(logger, "Recepcion confirmada.");
				}

			break;
	}
	*/



