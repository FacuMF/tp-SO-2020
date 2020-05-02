/*
 * utils.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */
#include "utils.h"

// TODO: MOVER A MODULO

int iniciar_conexion_cliente(char *ip, char* puerto) {

	//Set up conexion
	struct addrinfo *servinfo = obtener_server_info(ip,puerto); // Address info para la conexion TCP/IP
	int socket_cliente = obtener_socket(servinfo);

	// Conectarse
	if (connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen)
			== -1)
		printf("error");

	freeaddrinfo(servinfo);

	return socket_cliente;
}

void iniciar_conexion_servidor(char* ip, char* puerto) {

	//Set up conexion
	struct addrinfo* servinfo = obtener_server_info(ip,puerto); // Address info para la conexion TCP/IP
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor,servinfo);

	freeaddrinfo(servinfo);

	listen(socket_servidor, SOMAXCONN);	// Prepara el socket para crear una conexión con el request que llegue. SOMAXCONN = numero maximo de conexiones acumulables

	while (1)
		esperar_cliente(socket_servidor);//Queda esperando que un cliente se conecte
}

// INICIALIZACION DE LA CONEXION

struct addrinfo* obtener_server_info(char * ip, char* puerto) {
	struct addrinfo hints, *servinfo;
	int status;

	memset(&hints, 0, sizeof(hints));		// Vacía el la estructura reservada
	hints.ai_family = AF_UNSPEC;			// No importa si es IPV4 o IPV6
	hints.ai_socktype = SOCK_STREAM;		// Es de tipo TCP stream
	hints.ai_flags = AI_PASSIVE;			// Autoconfiguracion de IP

	// Preparar las estructuras que se necesitan despues
	if ((status = getaddrinfo(ip, puerto, &hints, &servinfo)) != 0) {// hints estructura con data del socket. servinfo guarda el resultado
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));// TODO: Tirar excepcion
		exit(1);
	}

	return servinfo;

}

int obtener_socket(struct addrinfo * servinfo){
	struct addrinfo *p;
	int socket_servidor;

	// Loopear por todos los resultados y conectarse al primero que pueda
	for (p = servinfo; p != NULL; p = p->ai_next)// Por cada elemento de la lista de addr infos....
	{
		if ((socket_servidor = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1)	// Si al crear el socket me da error, me fijo con el siguient
			continue;
		break;	// Encontré el correcto
	}

	// Validar conexion exitosa
	if (p==NULL) /*TODO: Tirar excepcion*/ printf("error: conexion no exitosa");

	return socket_servidor;
}

void asignar_socket_a_puerto(int socket,struct addrinfo *p){
	if (bind(socket, p->ai_addr, p->ai_addrlen) == -1) { //asigna el socket creado a la conexion obtenida de mi maquina
		// ERROR DE BIND. eg: el puerto ya está siendo usado
		// TODO: Tirar excepcion.
		close(socket);
	}
}

// MANIPULACION MENSAJES
t_paquete* generar_paquete(t_buffer* buffer, op_code codigo_operacion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer = buffer;

	return paquete;
}

void* serializar_paquete(t_paquete* paquete, int *bytes) {
	// Calculo de tamaño de stream
	int size_serializado = sizeof(paquete->codigo_operacion)
				+ sizeof(paquete->buffer->size) + paquete->buffer->size;

	// Inicializacion variables
	void *stream_serializado = malloc(size_serializado);
	int desplazamiento = 0;

	// Cargar paquete en stream
	memcpy(stream_serializado + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(stream_serializado + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(stream_serializado + desplazamiento, paquete->buffer->stream,
			paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	// Guardar tamaño serializacion en memoria
	(*bytes) = size_serializado;

	return stream_serializado;

}

void enviar_mensaje(int socket, t_buffer* buffer, op_code codigo_operacion){
	t_paquete* paquete = generar_paquete(buffer, codigo_operacion);
	int size_serializado;
	void* serializado= serializar_paquete(paquete, &size_serializado);

	send(socket, serializado, size_serializado, 0);

	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

	free(serializado);
}

t_buffer* recibir_mensaje(int socket, op_code * operacion){
		recv(socket, operacion, sizeof(*operacion), 0);// TODO: ultimo parametro customizable?
		int buffer_size;
		recv(socket, &buffer_size, sizeof(buffer_size), 0);

		char *buffer = malloc(buffer_size);
		recv(socket, buffer, buffer_size, 0);

		return buffer;
}

// TERMINAR CONEXION

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}


// OTROS

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
	int cod_op;
	if (recv(*socket, &cod_op, sizeof(int), MSG_WAITALL) == -1)
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



