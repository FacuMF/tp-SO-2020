/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: PokEbola
 */

#include "servidor.h"

int main(void)
{
	char * ip ;
	char * puerto ;

	// Iniciar Logger
	logger = iniciar_logger("./servidor.log","Server");

	// Leer config
	config = leer_config("/home/utnso/Documentos/tp-2020-1c-PokEbola/Cliente/src/cliente.config");
	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Inicar conexion
	iniciar_conexion_servidor(ip,puerto);

	//Terminar
	finalizar_ejecucion(logger,config);

	return EXIT_SUCCESS;
}

void finalizar_ejecucion(t_log* logger,t_config *config){
	terminar_logger(logger);
	config_destroy(config);
}

void iniciar_conexion_servidor(char* ip, char* puerto) {

	log_info(logger,"Servidor Inicializado");
	//Set up conexion
	struct addrinfo* servinfo = obtener_server_info(ip,puerto); // Address info para la conexion TCP/IP
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor,servinfo);
	setear_socket_reusable(socket_servidor);
	freeaddrinfo(servinfo);

	listen(socket_servidor, SOMAXCONN);	// Prepara el socket para crear una conexión con el request que llegue. SOMAXCONN = numero maximo de conexiones acumulables

	while (1)
		esperar_cliente(socket_servidor);//Queda esperando que un cliente se conecte
}

void setear_socket_reusable(int socket){
	int activado = 1;
	setsockopt(socket, SOL_SOCKET,SO_REUSEADDR, &activado,sizeof(activado));
}

void esperar_cliente(int socket_servidor) {	// Hilo coordinador
	struct sockaddr_in dir_cliente;	//contiene address de la comunicacion

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);// Acepta el request del cliente y crea el socket

	// Lanzar los hilos handlers
	pthread_create(&thread, NULL, (void*) serve_client, &socket_cliente);// Crea un thread que se quede atendiendo al cliente
	pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente
}

void serve_client(int* socket) {
	int cod_op = recibir_codigo_operacion(*socket);
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	t_buffer * buffer;
	switch (cod_op) {
	case TEXTO:
		buffer = recibir_mensaje(cliente_fd);
		t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer);
		log_info(logger, "Mensaje Recibido.");
		log_info(logger,mensaje_recibido->contenido);

		t_buffer *buffer = serializar_mensaje(mensaje_recibido);
		log_info(logger, "Mensaje Serializado");

		enviar_mensaje(cliente_fd, buffer, TEXTO);
		log_info(logger, "Mensaje Enviado");

		free(buffer);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
}
