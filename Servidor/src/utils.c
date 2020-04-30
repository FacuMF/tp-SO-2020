/*
 * conexiones.c
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#include"utils.h"

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
			msg = recibir_mensaje(cliente_fd, &size);
			devolver_mensaje(msg, size, cliente_fd);
			free(msg);
			break;
		case 0:
			pthread_exit(NULL);
		case -1:
			pthread_exit(NULL);
		}
}

void* recibir_mensaje(int socket_cliente, int* size)
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

