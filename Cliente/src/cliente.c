#include "cliente.h"

int main(void)
{
	// Iniciar variables
	int conexion;
	char* ip;
	char* puerto;
	char * mensaje_a_enviar;

	// Iniciar Logger
	logger = iniciar_logger("./cliente.log","Cliente");

	// Leer Config
	config = leer_config("/home/utnso/Documentos/tp-2020-1c-PokEbola/Cliente/src/cliente.config");
	ip = config_get_string_value(config,"IP");
	puerto = config_get_string_value(config,"PUERTO");

	// Iniciar Conexion
	conexion = crear_conexion(ip,puerto);
	log_info(logger,"Conexion Creada. IP: %s y PUERTO: %s \n",ip,puerto);

	//enviar mensaje
	printf("Ingrese el mensaje a enviar : \n ");
	scanf("%s",mensaje_a_enviar);
	enviar_mensaje(mensaje_a_enviar,conexion);
	log_info(logger,"Mensaje Enviado");

	//recibir mensaje
	char *mensaje = recibir_mensaje(conexion);
	log_info(logger, "El mensaje recibido es: %s\n",mensaje);

	//Finalizar mensaje
	terminar_programa(conexion, logger, config);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	terminar_logger(logger);

	if(config != NULL){
		config_destroy(config);
	}
	close(conexion);
}
