/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: PokEbola
 */

#include "servidor.h"

int main(void)
{
	logger = malloc(sizeof(t_log));

	logger = iniciar_logger();

	iniciar_servidor("127.0.0.1","4444");

	finalizar_ejecucion(logger);

	return EXIT_SUCCESS;
}


t_log* iniciar_logger(void)
{
	return log_create("/home/utnso/Documentos/tp-2020-1c-PokEbola/Base/Base.log","Server",true,LOG_LEVEL_INFO);
}

void finalizar_ejecucion(t_log* logger){
	log_destroy(logger);
}
