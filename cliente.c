#include "cliente.h"
#include "utilsClient.h"


int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/
	int conexion;
	char* ip;
	char* puerto;

	t_log* logger;
	t_config* config;

	logger = iniciar_logger();

	config = leer_config();
	ip = config_get_string_value(config,"IP");
	puerto = config_get_string_value(config,"PUERTO");
	conexion = crear_conexion(ip,puerto);
	log_info(logger, "IP: %s y PUERTO: %s \n",ip,puerto);

	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	//crear conexion
	conexion = crear_conexion(ip, puerto);

	//enviar mensaje
	enviar_mensaje("Hola bro",conexion);

	//recibir mensaje
	char *mensaje = recibir_mensaje(conexion);

	//loguear mensaje recibido
	log_info(logger, "El mensaje recibido es: %s\n",mensaje);
	terminar_programa(conexion, logger, config);
}

t_log* iniciar_logger(void)
{
	t_log * logger;
	if((logger = log_create("cliente.c","Cliente",true,LOG_LEVEL_INFO)) == NULL){
		printf("Error en log\n");
		exit(1);
	}
	return logger;
}

t_config* leer_config(void)
{
	t_config *config;
	if((config = config_create("cliente.config"))==NULL){
		printf("Error en config.\n");
		exit(2);
	}
	return config;
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	if(logger != NULL){
		log_destroy(logger);
	}

	if(config != NULL){
		config_destroy(config);
	}
}
