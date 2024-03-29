/*
 * utils.c
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */
#include "utils.h"
#include "utils_mensajes.h"

// INICIALIZACION DE LA CONEXION

// TBR



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

int iniciar_conexion(char *ip, char* puerto) {

	//Set up conexion
	struct addrinfo *servinfo = obtener_server_info(ip, puerto); // Address info para la conexion TCP/IP
	int socket_cliente = obtener_socket(servinfo);

	// Conectarse
	if (connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		freeaddrinfo(servinfo);
		return -1;
	}

	freeaddrinfo(servinfo);

	return socket_cliente;
}


// Inicializar conexion servidor

void setear_socket_reusable(int socket) {
	int activado = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

int iniciar_conexion_servidor(char* ip,char* puerto){

	struct addrinfo* servinfo = obtener_server_info(ip, puerto);
	int socket_servidor = obtener_socket(servinfo);
	setear_socket_reusable(socket_servidor);
	asignar_socket_a_puerto(socket_servidor, servinfo);
	freeaddrinfo(servinfo);

	listen(socket_servidor, SOMAXCONN);

	return socket_servidor;
}

int aceptar_cliente(int socket_servidor){
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
				&tam_direccion);

	return socket_cliente;
}

// MANIPULACION MENSAJES
t_paquete* generar_paquete(t_buffer* buffer, op_code codigo_operacion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo_operacion;
	paquete->buffer = malloc(sizeof(t_buffer));
	memcpy(paquete->buffer,buffer,sizeof(t_buffer));

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

	(*bytes) = size_serializado;

	return stream_serializado;

}

void enviar_mensaje(int socket, t_buffer* buffer, op_code codigo_operacion){
	t_paquete* paquete = generar_paquete(buffer, codigo_operacion);
	int size_serializado;
	void* serializado = serializar_paquete(paquete, &size_serializado);

	int b = send(socket, serializado, size_serializado, 0);
	liberar_buffer(buffer);
	free(paquete->buffer);
	free(paquete);
	free(serializado);

}

op_code recibir_codigo_operacion(int socket){
	op_code operacion;

	if (recv(socket, &operacion, sizeof(operacion), MSG_WAITALL) == -1)
				operacion = -1;

	return operacion;
}

t_buffer* recibir_mensaje(int socket){

		t_buffer *buffer = malloc(sizeof(t_buffer));

		int bytes_recibidos = recv(socket, &buffer->size, sizeof(buffer->size), 0);
		if(bytes_recibidos<0)printf("Error de serializacion");
		buffer->stream = malloc(buffer->size);
		recv(socket, buffer->stream, buffer->size, 0);
		return buffer;
}

// TERMINAR CONEXION

void liberar_conexion(int socket_cliente) {
	close(socket_cliente);
}


