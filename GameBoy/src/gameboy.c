#include "gameboy.h"

int main(int argv, char* arg[]){

	//Inicio logger
	logger = iniciar_logger("./GameBoy/config/gameboy.log","GameBoy");

	// Leer configuracion
	config =leer_config("./GameBoy/config/gameboy.config");

	//////  TIPO DE MENSAJE: MODULO MENSAJE ARG1 ARG2 AGR3 ... //////
	//////                   arg[1] arg[2]  arg[3] ...         //////

	t_modulo modulo = string_a_modulo(arg[1]);
	t_tipo_mensaje tipo_mensaje = string_a_tipo_mensaje(modulo,arg[2]);
	/*TBR*/ if(tipo_mensaje == -1){
		log_info(logger, "Ese mensaje no existe.");
	}else {
		log_info(logger, "Se quiere enviar un mensaje del tipo -%i- al modulo -%i-", tipo_mensaje, modulo);
	}

	////// Conectar con quien corresponda (iniciar conexion) /////
	char* ip = leer_ip(modulo);
	char* puerto = leer_puerto(modulo);
	/*TBR*/ log_info(logger, "Leido de config por parametro %s. Ip: %s y Puerto: %s", arg[1], ip, puerto);

	int conexion;
	conexion = iniciar_conexion(ip,puerto);
	log_info(logger, "Conexion Creada. Ip: %s y Puerto: %s ", ip, puerto);

	////// Crear mensaje como corresponda //////

	////// Serializar mensaje //////

	////// Enviar mensaje //////



	/*
	///////////////////////// CLIENTE  para referencia  //////////////////////////////
	conexion = iniciar_conexion(ip, puerto);
	log_info(logger, "Conexion Creada. IP: %s y PUERTO: %s ", ip, puerto);


	t_msjTexto* mensaje_test = crear_mensaje(mensaje_a_enviar);
	log_info(logger, "Mensaje Creado");

	t_buffer *buffer = serializar_mensaje(mensaje_test);
	log_info(logger, "Mensaje Serializado");

	enviar_mensaje(conexion, buffer, TEXTO);
	log_info(logger, "Mensaje Enviado");
	*/
}

t_modulo string_a_modulo(char* nombre_modulo){
	if(string_equals_ignore_case(nombre_modulo, "TEAM")){
		return team;
	}else if(string_equals_ignore_case(nombre_modulo, "GAMECARD")){
		return gamecard;
	}else if(string_equals_ignore_case(nombre_modulo, "BROKER")){
		return broker;
	}else{
		log_info(logger, "Error: El modulo ingresado es incorrecto. ");
		return -1;
	}
}

t_tipo_mensaje string_a_tipo_mensaje(t_modulo modulo, char* nombre_mensaje){
	switch(modulo){
		case team:
			if(string_equals_ignore_case(nombre_mensaje, "APPEARED_POKEMON")){
				return msg_appeared_pokemon;
			}else{
				log_info(logger, "Error: El modulo ingresado no recibe el mesaje: %s .", nombre_mensaje);
				return -1;
			}
			break;
		case gamecard:
			if(string_equals_ignore_case(nombre_mensaje, "NEW_POKEMON")){
				return msg_new_pokemon;
			}else if(string_equals_ignore_case(nombre_mensaje, "CATCH_POKEMON")){
				return msg_id_catch_pokemon;
			}else if(string_equals_ignore_case(nombre_mensaje, "GET_POKEMON")){
				return msg_get_pokemon;
			}else{
				log_info(logger, "Error: El modulo ingresado no recibe el mesaje: %s .", nombre_mensaje);
				return -1;
			}
			break;
		case broker:
			if(string_equals_ignore_case(nombre_mensaje, "NEW_POKEMON")){
				return msg_id_new_pokemon;
			}else if(string_equals_ignore_case(nombre_mensaje, "APEARED_POKEMON")){
				return msg_id_appeared_pokemon;
			}else if(string_equals_ignore_case(nombre_mensaje, "CATCH_POKEMON")){
				return msg_catch_pokemon;
			}else if(string_equals_ignore_case(nombre_mensaje, "CAUGTH_POKEMON")){
				return msg_caugth_pokemon;
			}else if(string_equals_ignore_case(nombre_mensaje, "GET_POKEMON")){
				return msg_get_pokemon;
			}else{
				log_info(logger, "Error: El modulo ingresado no recibe el mesaje: %s .", nombre_mensaje);
				return -1;
			}
			break;
	}

}


char* leer_ip(t_modulo modulo){
	char* ip;
	switch(modulo){
		case team:
			ip = config_get_string_value(config, "IP_TEAM");
			break;
		case gamecard:
			ip = config_get_string_value(config, "IP_GAMECARD");
			break;
		case broker:
			ip = config_get_string_value(config, "IP_BROKER");
			break;
	}
	return ip;
}

char* leer_puerto(t_modulo modulo){
	char* puerto;
	switch(modulo){
		case team:
			puerto = config_get_string_value(config, "PUERTO_TEAM");
			break;
		case gamecard:
			puerto = config_get_string_value(config, "PUERTO_GAMECARD");
			break;
		case broker:
			puerto = config_get_string_value(config, "PUERTO_BROKER");
			break;
	}
	return puerto;
}

//Socket

int iniciar_conexion(char *ip, char* puerto) {

	//Set up conexion
	struct addrinfo *servinfo = obtener_server_info(ip, puerto); // Address info para la conexion TCP/IP
	int socket_cliente = obtener_socket(servinfo);

	// Conectarse
	if (connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		log_info(logger, "Error al conectarse al socket");

	freeaddrinfo(servinfo);

	return socket_cliente;
}
