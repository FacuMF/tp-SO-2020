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
	char* ip = leer_ip(modulo,config);
	char* puerto = leer_puerto(modulo,config);
	/*TBR*/ log_info(logger, "Leido de config por parametro %s. Ip: %s y Puerto: %s", arg[1], ip, puerto);

	int conexion;
	conexion = iniciar_conexion(ip,puerto);
	log_info(logger, "Conexion Creada. Ip: %s y Puerto: %s ", ip, puerto);

	////// Crear y Serializar mensaje //////
	t_buffer* mensaje_serializado =  crear_serializar_mensaje( modulo, tipo_mensaje,arg);
	log_info(logger, "El mensaje fue serializado. ");

	////// Enviar mensaje //////
	enviar_mensaje(conexion, mensaje_serializado, APPEARED_POKEMON);
	log_info(logger, "El mensaje fue enviado ;)");


}

t_buffer* crear_serializar_mensaje(t_modulo modulo,t_tipo_mensaje tipo_mensaje,void* arg){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	switch(modulo){
		case team: ////MODULO TEAM////
			switch(tipo_mensaje){
				case msg_appeared_pokemon:
					mensaje_serializado = crear_serializar_appeared_pokemon(arg);
				break;
			}
		break;
		case broker: ////MODULO BROKER////
			//TODO
		break;
		case gamecard:////MODULO GAMECARD////
			//TODO
		break;
	}
	return mensaje_serializado;
}

t_buffer* crear_serializar_appeared_pokemon(char* arg[]){

	////// CREAR MENSAJE //////

	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));
	mensaje->size_pokemon = strlen(arg[3]);
	mensaje->pokemon = malloc( sizeof(arg[3]) + 1 );
	mensaje->pokemon = arg[3];
	mensaje->posx = atoi(arg[4]);
	mensaje->posy = atoi(arg[5]);

	log_info(logger,"Se creo un mensaje APPEARED_POKEMON. POKEMON: %s , X: %i , Y: %i .",
			mensaje->pokemon, mensaje->posx, mensaje->posy );

	//////  SERIALIZAR MENSAJE  //////

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = 	sizeof(mensaje->pokemon)+1     +
					sizeof(mensaje->size_pokemon)  +
					sizeof(mensaje->posx)          +
					sizeof(mensaje->posy);

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream+offset, &(mensaje->size_pokemon), sizeof(mensaje->size_pokemon));
	offset += (sizeof(mensaje->size_pokemon) +1);
	memcpy(stream+offset, mensaje->pokemon, sizeof(mensaje->pokemon));
	offset += sizeof(mensaje->pokemon);
	memcpy(stream+offset, &(mensaje->posx), sizeof(mensaje->posx));
	offset += sizeof(mensaje->posx);
	memcpy(stream+offset, &(mensaje->posy), sizeof(mensaje->posy));
	offset += sizeof(mensaje->posy);

	buffer->stream = stream;
	log_info(logger, "El buffer fue cargado. ");

	//free(mensaje->pokemon);

	return buffer;

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
	return -1;
}
