/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: PokEbola
 */

#include "servidor.h"

int main(void)
{
	t_config *config;
	char * ip ;
	char * puerto ;

	logger = malloc(sizeof(t_log));

	logger = iniciar_logger();

	config = leer_config();

	ip=config_get_string_value(config,"IP");
	puerto=config_get_string_value(config,"PUERTO");

	iniciar_servidor(ip,puerto);

	finalizar_ejecucion(logger,config);

	return EXIT_SUCCESS;
}


t_log* iniciar_logger(void)
{
	return log_create("./servidor.log","Server",true,LOG_LEVEL_INFO);
}

t_config*leer_config(){
	t_config *config;
	config = config_create("/home/utnso/Documentos/tp-2020-1c-PokEbola/configGeneral.config");
		if(config ==NULL){
			printf("Error en config.\n");
			exit(3);
		}
		return config;
}

void finalizar_ejecucion(t_log* logger,t_config *config){
	if(logger != NULL){
			log_destroy(logger);
		}

		if(config != NULL){
			config_destroy(config);
		}
}
