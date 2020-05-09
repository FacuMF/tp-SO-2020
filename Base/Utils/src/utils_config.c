#include "utils.h"

t_config* leer_config(char* ubicacion_relativa)
{
	t_config *config;
	config = config_create(ubicacion_relativa);
	if(config == NULL){
		printf("Error en config.\n");
		exit(2);
	}
	return config;
}

char* leer_ip(t_modulo modulo, t_config* config){
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

char* leer_puerto(t_modulo modulo, t_config* config){
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
