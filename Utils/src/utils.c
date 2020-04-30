/*
 * utils.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */
#include "utils.h"

void* serializar_paquete_cli(t_paquete* paquete, int *bytes)
{
	int size_serializado = sizeof(paquete->codigo_operacion) + sizeof(paquete->buffer->size) + paquete->buffer->size;
	void *a_enviar = malloc(size_serializado);

	int bytes_escritos = 0;
	memcpy(a_enviar+ bytes_escritos, &(paquete->codigo_operacion), sizeof(paquete->codigo_operacion));
	bytes_escritos += sizeof(paquete->codigo_operacion);
	memcpy(a_enviar + bytes_escritos, &(paquete->buffer->size), sizeof(paquete->buffer->size));
	bytes_escritos += sizeof(paquete->buffer->size);
	memcpy(a_enviar + bytes_escritos, paquete->buffer->stream, paquete->buffer->size);
	bytes_escritos += paquete->buffer->size;


	(*bytes) = size_serializado;
	return a_enviar;
}

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		printf("error");

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = 1;  							 //MENSAJE
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	paquete->buffer->stream = mensaje;

	int size_serializado;
	void* serializado = serializar_paquete_cli(paquete,&size_serializado);

	send(socket_cliente, serializado, size_serializado, 0);
	free(serializado);

}

char* recibir_mensaje(int socket_cliente)
{
	op_code operacion;
	recv(socket_cliente, &operacion , sizeof(operacion) ,0);
	int buffer_size;
	recv(socket_cliente, &buffer_size, sizeof(buffer_size),0);
	//Aca esta el error
	char *buffer = malloc(buffer_size);
	recv(socket_cliente, buffer, buffer_size, 0);

	if(buffer[buffer_size - 1] != '\0'){
		printf("El buffer recibido no es un string\n");
	}
	return buffer;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void iniciar_servidor(char * ip, char* puerto)
{

	int socket_servidor;

	int status;
    struct addrinfo hints, *servinfo, *p;	// Address info paraa la conexion TCP/IP

    memset(&hints, 0, sizeof(hints));		// Vacía el la estructura reservada
    hints.ai_family = AF_UNSPEC;			// No importa si es IPV4 o IPV6
    hints.ai_socktype = SOCK_STREAM;		// Es de tipo TCP stream
    hints.ai_flags = AI_PASSIVE;			// Autoconfiguracion de IP

    // Preparar las estructuras que se necesitan despues
    if ((status  = getaddrinfo(ip, puerto, &hints, &servinfo)) != 0){		// hints estructura con data del socket. servinfo guarda el resultado
    	fprintf(stderr,"getaddrinfo error: %s\n", gai_strerror(status));	// [ADD] Muestra error
    	exit(1);
    }

    // Loopear por todos los resultados y conectarse al primero que pueda
    for (p=servinfo; p != NULL; p = p->ai_next)		// Por cada elemento de la lista de addr infos....
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)	// Si al crear el socket me da error, me fijo con el siguient
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {	//asigna el socket creado al puerto de mi maquina
            close(socket_servidor);
            continue;
        }
        break;
    }

    // Chequeo si la conexión fue exitosa
    if (p == NULL) {
    	fprintf(stderr, "server: failed to bind\n");
    	exit(1);
    }

	listen(socket_servidor, SOMAXCONN);		// Prepara el socket para crear una conexión con el request que llegue

    freeaddrinfo(servinfo);

    while(1)
    	esperar_cliente(socket_servidor);	//Queda esperando que un cliente se conecte
}

void esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;	//contiene address de la comunicacion

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);	// Acepta el request del cliente y crea el socket

	pthread_create(&thread,NULL,(void*)serve_client,&socket_cliente);	// Crea un thread que se quede atendiendo al cliente
	pthread_detach(thread);		// Si termina el hilo, que sus recursos se liberen automaticamente

}

void serve_client(int* socket)
{
	int cod_op;
	if(recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
		cod_op = -1;
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	void* msg;
		switch (cod_op) {
		case 1:
			msg = recibir_mensaje_serv(cliente_fd, &size);
			devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void* recibir_mensaje_serv(int socket_cliente, int* size)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void devolver_mensaje(void* payload, int size, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = 1;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = size;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, payload, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2*sizeof(int);

	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}



