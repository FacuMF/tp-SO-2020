#include "gameboy.h"

int main(int argv, char* arg[]) {

	inicializar_gameboy();

	/*  Obtener Caracteristicas Msj
	 Tipo de mensaje: MODULO MENSAJE ARG1 ARG2 AGR3 ...
	 	 	 	 	  arg[1] arg[2]  arg[3] ...
	 */

	t_modulo modulo = string_a_modulo(arg[1]);

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
	log_trace(logger, "El mensaje fue enviado ;)");

	free(mensaje_serializado->stream);
	free(mensaje_serializado);

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
			log_trace(logger, "Mensaje: SUSCRIPTOR Cola:%s, Tiempo:%s.", arg[2], arg[3]);
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
			t_caugth_pokemon* mensaje_caugth;
			id_mensaje_correlativo = atoi(arg[3]);
			ok_fail = atoi(arg[4]);
			mensaje_caugth = crear_caugth_pokemon(id_mensaje_correlativo,
					ok_fail);
			mensaje_serializado = serializar_caught_pokemon(mensaje_caugth);
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
		}
		break;
	}
	return mensaje_serializado;
}

t_modulo string_a_modulo(char* nombre_modulo) {
	if (string_equals_ignore_case(nombre_modulo, "TEAM")) {
		return team;
	} else if (string_equals_ignore_case(nombre_modulo, "GAMECARD")) {
		return gamecard;
	} else if (string_equals_ignore_case(nombre_modulo, "BROKER")) {
		return broker;
	} else if (string_equals_ignore_case(nombre_modulo, "SUSCRIPTOR")) {
		return SUSCRIPTOR;
	} else {
		return -1;
	}
}

op_code string_a_tipo_mensaje(char* nombre_mensaje) {
	if (string_equals_ignore_case(nombre_mensaje, "NEW_POKEMON")) {
		return NEW_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "APPEARED_POKEMON")) {
		return APPEARED_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "CATCH_POKEMON")) {
		return CATCH_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "CAUGTH_POKEMON")) {
		return CAUGHT_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "GET_POKEMON")) {
		return GET_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "SUSCRIPTOR")) {
		return SUSCRIPTOR;
	} else
		return -1;
}
