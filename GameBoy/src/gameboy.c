#include "gameboy.h"

int main(int argv, char* arg[]) {
	printf("int: %d, uint32_t: %d \n", sizeof(int), sizeof(uint32_t));

	inicializar_gameboy();

	obtener_argumentos(arg);

	enviar_mensaje_gameboy(arg);

	cod_op_respuesta = 0;
	esperar_respuesta();
}

void obtener_argumentos(char** arg) {
	es_suscriptor = (string_a_modulo(arg[1]) == SUSCRIPTOR) ? 1 : 0;
	modulo = obtener_modulo(arg);
	tipo_mensaje = obtener_tipo_mensaje(arg);

	log_trace(logger, "Caracteristicas de mensaje obtenidas");

	log_trace(logger,
			"Se quiere enviar un mensaje del tipo -%i- al modulo -%i-",
			tipo_mensaje, modulo);

	ip = leer_ip(modulo, config);
	puerto = leer_puerto(modulo, config);
	/*TBR*/log_trace(logger,
			"Leido de config por parametro %s. Ip: %s y Puerto: %s", arg[1], ip,
			puerto);
}

void enviar_mensaje_gameboy(char** arg) {
	conexion = iniciar_conexion(ip, puerto);
	log_trace(logger, "Conexion Creada. Ip: %s y Puerto: %s ", ip, puerto);

	t_buffer* mensaje_serializado = mensaje_a_enviar(modulo, tipo_mensaje, arg);
	log_trace(logger, "El mensaje fue serializado. ");

	log_trace(logger, "OPERATION CODE: %i", tipo_mensaje);
	enviar_mensaje(conexion, mensaje_serializado, tipo_mensaje);
	log_trace(logger, "El mensaje fue enviado.");

	//free(mensaje_serializado->stream); //Esto tira seg fault con localized_pokemon
	//free(mensaje_serializado);
}

void esperar_respuesta(void) {
	if (tipo_mensaje == SUSCRIPTOR) {
		// Si el mensaje que se envio fue una suscripcion, quiero recibir todos los mensjaes de esa cola.
		while (cod_op_respuesta >= 0) {
			cod_op_respuesta = recibir_respuesta(&conexion);
		}
		log_warning(logger, "Se salio del while del socket %i.", conexion);
	} else if (modulo != team) {
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

void confirmar_si_es_suscriptor(int socket, int cop_op, int id_mensaje) {
	if (es_suscriptor) {
		log_trace(logger, "Se confirmara la recepcion.");
		confirmar_recepcion(socket, cop_op, id_mensaje);
		log_trace(logger, "Recepcion confirmada.");
	}
}

void handle_respuesta(int cod_op, int socket_broker) {

	t_buffer * buffer = recibir_mensaje(socket_broker);

	char* tipo_mensaje = malloc(sizeof(char) * 20);
	tipo_mensaje = op_code_a_string(cod_op);
	log_trace(logger, "Se recibio un mensaje %s", tipo_mensaje);

	switch (cod_op) {
	case APPEARED_POKEMON:
		;
		t_appeared_pokemon* mensaje_appeared_pokemon =
				deserializar_appeared_pokemon(buffer);
		log_trace(logger, mostrar_appeared_pokemon(mensaje_appeared_pokemon));
		confirmar_si_es_suscriptor(socket_broker, cod_op,
				mensaje_appeared_pokemon->id_mensaje);
		break;
	case CAUGHT_POKEMON:
		;
		t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(
				buffer);
		log_trace(logger, mostrar_caught_pokemon(mensaje_caught_pokemon));
		confirmar_si_es_suscriptor(socket_broker, cod_op,
				mensaje_caught_pokemon->id_mensaje);
		break;
	case LOCALIZED_POKEMON:
		;
		t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(
				buffer);
		log_trace(logger, mostrar_localized(mensaje_localized_pokemon));
		confirmar_si_es_suscriptor(socket_broker, cod_op,
				mensaje_localized_pokemon->id_mensaje);
		break;
	case NEW_POKEMON:
		;
		t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
		log_trace(logger, mostrar_new_pokemon(mensaje_new_pokemon));
		confirmar_si_es_suscriptor(socket_broker, cod_op,
				mensaje_new_pokemon->id_mensaje);
		break;
	case GET_POKEMON:
		;
		t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
		log_trace(logger, mostrar_get_pokemon(mensaje_get_pokemon));
		confirmar_si_es_suscriptor(socket_broker, cod_op,
				mensaje_get_pokemon->id_mensaje);
		break;
	case CATCH_POKEMON:
		;
		t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(
				buffer);
		log_trace(logger, mostrar_catch_pokemon(mensaje_catch_pokemon));
		confirmar_si_es_suscriptor(socket_broker, cod_op,
				mensaje_catch_pokemon->id_mensaje);
		break;
	default:
		log_error(logger, "Op_code inválido.");
		break;
	}
	log_trace(logger, "Mensaje recibido manejado.");
}

t_buffer* mensaje_a_enviar(t_modulo modulo, op_code tipo_mensaje, char* arg[]) {
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	char* pokemon;
	int pos_x, pos_y, cantidad, id_mensaje, cola_de_mensajes,
			tiempo_suscripcion, id_mensaje_correlativo, ok_fail;
	switch (tipo_mensaje) {
	case APPEARED_POKEMON:
		;
		t_appeared_pokemon* mensaje_appeared;
		pokemon = malloc(sizeof(arg[3]));
		cargar_parametros_appeared_pokemon(pokemon, &pos_x, &pos_y, &id_mensaje,
				arg, modulo);
		mensaje_appeared = crear_appeared_pokemon(pokemon, pos_x, pos_y,
				id_mensaje);
		mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared);
		break;
	case NEW_POKEMON:
		;
		t_new_pokemon* mensaje_new;
		pokemon = malloc(sizeof(arg[3]));
		cargar_parametros_new_pokemon(pokemon, &pos_x, &pos_y, &cantidad,
				&id_mensaje, arg, modulo);
		mensaje_new = crear_new_pokemon(pokemon, pos_x, pos_y, cantidad,
				id_mensaje);
		mensaje_serializado = serializar_new_pokemon(mensaje_new);
		break;
	case CAUGHT_POKEMON:
		;
		t_caught_pokemon* mensaje_caught;
		cargar_parametros_caught_pokemon(&id_mensaje_correlativo, &ok_fail,
				arg);
		mensaje_caught = crear_caught_pokemon(id_mensaje_correlativo, ok_fail);
		mensaje_serializado = serializar_caught_pokemon(mensaje_caught);
		break;
	case CATCH_POKEMON:
		;
		t_catch_pokemon* mensaje_catch;
		pokemon = malloc(sizeof(arg[3]));
		cargar_parametros_catch_pokemon(pokemon, &pos_x, &pos_y, &id_mensaje,
				arg, modulo);
		mensaje_catch = crear_catch_pokemon(pokemon, pos_x, pos_y, id_mensaje);
		mensaje_serializado = serializar_catch_pokemon(mensaje_catch);

		break;
	case GET_POKEMON:
		;
		t_get_pokemon* mensaje_get;
		pokemon = malloc(sizeof(arg[3]));
		cargar_parametros_get_pokemon(pokemon, &id_mensaje, arg, modulo);
		mensaje_get = crear_get_pokemon(pokemon, id_mensaje);
		mensaje_serializado = serializar_get_pokemon(mensaje_get);
		break;
	case LOCALIZED_POKEMON:
		;
		t_localized* mensaje_localized;
		pokemon = malloc(sizeof(arg[3]));
		strcpy(pokemon, arg[3]);
		int cantidad_de_pociciones = atoi(arg[4]);
		t_list* lista_posiciones = list_create();

		for (int i = cantidad_de_pociciones; i < 0; i--) {
			t_posicion* posicion_a_agregar = malloc(sizeof(t_posicion));
			posicion_a_agregar->x = atoi(arg[4 + (i * 2) - 1]);
			posicion_a_agregar->y = atoi(arg[4 + (i * 2)]);

			list_add(lista_posiciones, (void*) posicion_a_agregar);

		}

		mensaje_localized = crear_localized_pokemon(0, pokemon,
				lista_posiciones);

		mensaje_serializado = serializar_localized_pokemon(mensaje_localized);
		break;
	case SUSCRIPTOR:
		;
		t_subscriptor* mensaje_suscripcion;
		cargar_parametros_suscriptor(&cola_de_mensajes, &tiempo_suscripcion,
				arg);
		mensaje_suscripcion = crear_suscripcion(cola_de_mensajes,
				tiempo_suscripcion);
		mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);
		break;
	}
	return mensaje_serializado;
}
