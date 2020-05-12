#include "utils.h"

//Funciones

t_log* iniciar_logger(char* file, char* program_name)
{
	t_log * logger;
	if((logger = log_create(file,program_name,true,LOG_LEVEL_TRACE)) == NULL){
		printf("Error en log\n");
		exit(1);
	}
	return logger;
}


void terminar_logger(t_log* logger){
	if(logger != NULL){
			log_destroy(logger);
		}
}
