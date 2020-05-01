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



	logger = iniciar_logger("./servidor.log","Server");

	config = leer_config();

	ip=config_get_string_value(config,"IP");
	puerto=config_get_string_value(config,"PUERTO");

	iniciar_servidor(ip,puerto);

	finalizar_ejecucion(logger,config);

	return EXIT_SUCCESS;
}

void finalizar_ejecucion(t_log* logger,t_config *config){
	terminar_logger(logger);

		if(config != NULL){
			config_destroy(config);
		}
}
