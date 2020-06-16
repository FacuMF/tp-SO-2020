#include "gameboy.h"

int main(int argv, char* arg[]) {

	inicializar_gameboy();

	es_suscriptor = (string_a_modulo(arg[1]) == SUSCRIPTOR) ? 1 : 0;
	t_modulo modulo = obtener_modulo(arg);
	op_code tipo_mensaje = obtener_tipo_mensaje(arg);

	log_trace(logger, "Caracteristicas de mensaje obtenidas");

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

	if (tipo_mensaje == SUSCRIPTOR) {
		// Si el mensaje que se envio fue una suscripcion, quiero recibir todos los mensjaes de esa cola.
		while (cod_op_respuesta >= 0) {
			cod_op_respuesta = recibir_respuesta(&conexion);
		}
		log_warning(logger, "Se salio del while del socket %i.", conexion);
	} else {
		// Para el resto de mensajes, se va a recibir el mismo mensaje pero con el id asignado por el broker.
		recibir_respuesta(&conexion);
	}

}

t_modulo obtener_modulo(char** arg) {
	t_modulo modulo = string_a_modulo(arg[1]);
	if (modulo == -1) {
		log_error(logger, "El modulo ingresado es incorrecto.");
	} else if (es_suscriptor) {
		modulo = broker;
	}
	return modulo;
}

op_code obtener_tipo_mensaje(char** arg) {
	op_code tipo_mensaje;
	if (es_suscriptor) {
		tipo_mensaje = string_a_tipo_mensaje(arg[1]);
	} else {
		tipo_mensaje = string_a_tipo_mensaje(arg[2]);
	}
	if (tipo_mensaje == -1)
		log_error(logger, "Tipo de mensaje invalido");
	return tipo_mensaje;
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

int recibir_respuesta(int* socket_broker) {
	int cod_op = recibir_codigo_operacion(*socket_broker);
	(cod_op == -1) ?
			log_error(logger, "Error en 'recibir_codigo_operacion'") :
			log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);

	(cod_op >= 0) ?
			handle_respuesta(cod_op, *socket_broker) :
			log_warning(logger, "El broker cerro el socket %i.",
					*socket_broker);
	return cod_op;
}

void handle_respuesta(int cod_op, int socket_broker) {
	t_buffer * buffer = recibir_mensaje(socket_broker);
	switch (cod_op) {
	case APPEARED_POKEMON:

		log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
		t_appeared_pokemon* mensaje_appeared_pokemon =
				deserializar_appeared_pokemon(buffer);
		if (es_suscriptor) {
			log_trace(logger, "Se confirmara la recepcion.");
			confirmar_recepcion(socket_broker, cod_op,
					mensaje_appeared_pokemon->id_mensaje);
			log_trace(logger, "Recepcion confirmada.");
		}
		break;
	case CAUGHT_POKEMON:

		log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");
		t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(
				buffer);
		if (es_suscriptor) {
			log_trace(logger, "Se confirmara la recepcion.");
			confirmar_recepcion(socket_broker, cod_op,
					mensaje_caught_pokemon->id_mensaje);
			log_trace(logger, "Recepcion confirmada.");
		}

		break;
	case LOCALIZED_POKEMON:

		log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");
		t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(
				buffer);
		if (es_suscriptor) {
			log_trace(logger, "Se confirmara la recepcion.");
			confirmar_recepcion(socket_broker, cod_op,
					mensaje_localized_pokemon->id_mensaje);
			log_trace(logger, "Recepcion confirmada.");
		}

		break;
	case NEW_POKEMON:

		log_trace(logger, "Se recibio un mensaje NEW_POKEMON");
		t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
		if (es_suscriptor) {
			log_trace(logger, "Se confirmara la recepcion.");
			confirmar_recepcion(socket_broker, cod_op,
					mensaje_new_pokemon->id_mensaje);
			log_trace(logger, "Recepcion confirmada.");
		}
		break;
	case GET_POKEMON:

		log_trace(logger, "Se recibio un mensaje GET_POKEMON");
		t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
		if (es_suscriptor) {
			log_trace(logger, "Se confirmara la recepcion.");
			confirmar_recepcion(socket_broker, cod_op,
					mensaje_get_pokemon->id_mensaje);
			log_trace(logger, "Recepcion confirmada.");
		}
		break;
	case CATCH_POKEMON:

		log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");
		t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(
				buffer);
		if (es_suscriptor) {
			log_trace(logger, "Se confirmara la recepcion.");
			confirmar_recepcion(socket_broker, cod_op,
					mensaje_catch_pokemon->id_mensaje);
			log_trace(logger, "Recepcion confirmada.");
		}
		break;
	default:
		log_error(logger, "Opcode inválido.");
		break;
	}
	log_trace(logger, "Mensaje recibido manejado.");
}

//Armado de mensajes del Gameboy
t_buffer* mensaje_a_enviar(t_modulo modulo, op_code tipo_mensaje, char* arg[]) {
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	char* pokemon;
	int pos_x, pos_y, cantidad, id_mensaje, cola_de_mensajes,
			tiempo_suscripcion, id_mensaje_correlativo;
	bool ok_fail;
	switch (modulo) {
	case team: ////MODULO TEAM////
		;
		t_appeared_pokemon* mensaje_appeared;
		if (tipo_mensaje == APPEARED_POKEMON) {
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			pos_x = atoi(arg[4]);
			pos_y = atoi(arg[5]);
			mensaje_appeared = crear_appeared_pokemon(pokemon, pos_x, pos_y,
					-1);
			mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared);
		}
		break;
	case broker: ////MODULO BROKER////
		switch (tipo_mensaje) {
		case SUSCRIPTOR:
			;
			t_subscriptor* mensaje_suscripcion;
			cola_de_mensajes = string_a_tipo_mensaje(arg[2]);
			tiempo_suscripcion = atoi(arg[3]);
			mensaje_suscripcion = crear_suscripcion(cola_de_mensajes,
					tiempo_suscripcion);
			mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);
			break;
		case NEW_POKEMON:
			;
			t_new_pokemon* mensaje_new;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			pos_x = atoi(arg[4]);
			pos_y = atoi(arg[5]);
			cantidad = atoi(arg[6]);
			mensaje_new = crear_new_pokemon(pokemon, pos_x, pos_y, cantidad,
					-1);
			mensaje_serializado = serializar_new_pokemon(mensaje_new);
			break;
		case APPEARED_POKEMON:
			;
			t_appeared_pokemon* mensaje_appeared;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			pos_x = atoi(arg[4]);
			pos_y = atoi(arg[5]);
			id_mensaje_correlativo = atoi(arg[6]);
			mensaje_appeared = crear_appeared_pokemon(pokemon, pos_x, pos_y,
					id_mensaje_correlativo);
			mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared);
			break;
		case CATCH_POKEMON:
			;
			t_catch_pokemon* mensaje_catch;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			pos_x = atoi(arg[4]);
			pos_y = atoi(arg[5]);
			mensaje_catch = crear_catch_pokemon(pokemon, pos_x, pos_y, -1);
			mensaje_serializado = serializar_catch_pokemon(mensaje_catch);
			break;
		case CAUGHT_POKEMON:
			;
			t_caught_pokemon* mensaje_caught;
			id_mensaje_correlativo = atoi(arg[3]);
			ok_fail = atoi(arg[4]);
			mensaje_caught = crear_caught_pokemon(id_mensaje_correlativo,
					ok_fail);
			mensaje_serializado = serializar_caught_pokemon(mensaje_caught);
			break;
		case GET_POKEMON:
			;
			t_get_pokemon* mensaje_get;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			mensaje_get = crear_get_pokemon(pokemon, -1);
			mensaje_serializado = serializar_get_pokemon(mensaje_get);
			break;
		}
		break;
	case gamecard:			////MODULO GAMECARD////
		switch (tipo_mensaje) {
		case NEW_POKEMON:
			;
			t_new_pokemon* mensaje_new;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			pos_x = atoi(arg[4]);
			pos_y = atoi(arg[5]);
			cantidad = atoi(arg[6]);
			id_mensaje = atoi(arg[7]);
			mensaje_new = crear_new_pokemon(pokemon, pos_x, pos_y, cantidad,
					id_mensaje);
			mensaje_serializado = serializar_new_pokemon(mensaje_new);
			break;
		case CATCH_POKEMON:
			;
			t_catch_pokemon* mensaje_catch;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			pos_x = atoi(arg[4]);
			pos_y = atoi(arg[5]);
			id_mensaje = atoi(arg[6]);
			mensaje_catch = crear_catch_pokemon(pokemon, pos_x, pos_y,
					id_mensaje);
			mensaje_serializado = serializar_catch_pokemon(mensaje_catch);
			break;
		case GET_POKEMON:
			;
			t_get_pokemon* mensaje_get;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon, arg[3]);
			id_mensaje = atoi(arg[4]);
			mensaje_get = crear_get_pokemon(pokemon, id_mensaje);
			mensaje_serializado = serializar_get_pokemon(mensaje_get);
			break;
		case LOCALIZED_POKEMON: //No es un bug, es una feature. No esta en la consigna, pero para probar el broker lo agrego.
			;
			t_localized* mensaje_localized;
			pokemon = malloc(sizeof(arg[3]));
			strcpy(pokemon,arg[3]);
			int cantidad_de_pociciones = atoi(arg[4]);
			t_list* lista_posiciones;

			for (int i = cantidad_de_pociciones; i<0; i--){
				t_posicion* posicion_a_agregar = malloc(sizeof(t_posicion));
				posicion_a_agregar->x = atoi(arg[ 4 + (i*2) - 1 ]);
				posicion_a_agregar->y = atoi(arg[ 4 + (i*2) ]);

				list_add(lista_posiciones, (void*) posicion_a_agregar);
			}

			mensaje_localized = crear_localized_pokemon(-1, pokemon, lista_posiciones);
			mensaje_serializado = serializar_localized_pokemon(mensaje_localized);

			break;
		}
		break;
	}
	return mensaje_serializado;
}
