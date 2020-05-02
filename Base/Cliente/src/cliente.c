#include "cliente.h"

int main(void) {
	// Iniciar variables
	int conexion;
	char* ip;
	char* puerto;
	char * mensaje_a_enviar = "OLA";

	// Iniciar Logger
	logger = iniciar_logger("/home/utnso/tp-2020-1c-PokEbola/Base/Servidor/config/cliente.log", "Cliente");
	log_info(logger,"");
	// Leer Config
	config =
			leer_config(
					"/home/utnso/tp-2020-1c-PokEbola/Base/Cliente/config/cliente.config");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Iniciar Conexion
	conexion = iniciar_conexion(ip, puerto);
	log_info(logger, "Conexion Creada. IP: %s y PUERTO: %s ", ip, puerto);

	// Enviar mensaje
	/*
	printf("Ingrese el mensaje a enviar : \n ");
	fflush(stdin);
	gets(mensaje_a_enviar);
	*/

	t_msjTexto* mensaje_test = crear_mensaje(mensaje_a_enviar);
	log_info(logger, "Mensaje Creado");

	t_buffer *buffer = serializar_mensaje(mensaje_test);
	log_info(logger, "Mensaje Serializado");

	enviar_mensaje(conexion, buffer, TEXTO);
	log_info(logger, "Mensaje Enviado");

	// Recibir mensaje
	op_code codigo_operacion = recibir_codigo_operacion(conexion);
	log_info(logger, "Codigo operacion recibido.");

	char str[2];
	sprintf(str,"%d",codigo_operacion); //parse codop

	log_info(logger,str);

	t_buffer * buffer_recepcion = recibir_mensaje(conexion);
	log_info(logger, "Mensaje Recibido");

	// Analizar respuesta
	switch (codigo_operacion) {
	case TEXTO:
		handler_mensaje_texto(buffer_recepcion);
		break;
	default:
		log_info(logger, "Invalid op_code\n");
	}

	//Finalizar mensaje
	terminar_programa(conexion, logger, config);
}

void handler_mensaje_texto(t_buffer* buffer_recepcion) {
	t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer_recepcion);
	log_info(logger, "Mensaje Deserializado");
	log_info(logger,mensaje_recibido->contenido);
}

int iniciar_conexion(char *ip, char* puerto) {

	//Set up conexion
	struct addrinfo *servinfo = obtener_server_info(ip, puerto); // Address info para la conexion TCP/IP
	int socket_cliente = obtener_socket(servinfo);

	// Conectarse
	if (connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(servinfo);

	return socket_cliente;
}

void terminar_programa(int conexion, t_log* logger, t_config* config) {
	terminar_logger(logger);

	if (config != NULL) {
		config_destroy(config);
	}
	close(conexion);
}
