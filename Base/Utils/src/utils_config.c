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
