/*
 * utils.h
 *
 *  Created on: 30 abr. 2020
 *      Author: utnso
 */

#ifndef UTILS_UTILS_H_
#define UTILS_UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<commons/string.h>
#include <readline/readline.h>
#include<pthread.h>
#include "utils_mensajes.h"


typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;




pthread_t thread;


// MOVER A MODULO
int iniciar_conexion_cliente(char* ip, char* puerto);
void iniciar_conexion_servidor(char*,char*);

// INICIALIZACION DE LA CONEXION
struct addrinfo* obtener_server_info(char* , char*);
int obtener_socket(struct addrinfo* );
void asignar_socket_a_puerto(int,struct addrinfo*);

// MANIPULACION MENSAJES
void enviar_mensaje(int socket, t_buffer buffer, op_code codigo_operacion);

t_paquete* generar_paquete(t_buffer* , op_code); // DE UN BUFFER Y OP CODE SACA UN PAQUETE
void* serializar_paquete(t_paquete* paquete, int *bytes); // DE UN PAQUETE OBTENGO UN STREAM Y LOS BYTES

t_buffer* recibir_mensaje(int socket, op_code *);// DE UN SOCKET, DEVUELVE UN BUFFER Y UN OPCODE POR REFERENCIA.

// TERMINAR CONEXION
void liberar_conexion(int socket_cliente);

// OTROS
void esperar_cliente(int);
void serve_client(int *socket);
void process_request(int cod_op, int cliente_fd);


//      LOGGER

t_log* iniciar_logger(char* file, char* program_name);
void terminar_logger(t_log* logger);

//		CONFIG
t_config* leer_config(char*);

#endif /* UTILS_UTILS_H_ */
