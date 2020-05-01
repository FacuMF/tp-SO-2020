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

typedef enum
{
	MENSAJE = 1,
}op_code;

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

void* serializar_paquete_cli(t_paquete* paquete, int *bytes);
int iniciar_conexion_cliente(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
char* recibir_mensaje(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);
void liberar_conexion(int socket_cliente);

pthread_t thread;

void* recibir_buffer(int*, int);

struct addrinfo* obtener_server_info(char* , char*);
void asignar_socket_a_puerto(int,struct addrinfo*);
int obtener_socket(struct addrinfo* );
void iniciar_conexion_servidor(char*,char*);
void esperar_cliente(int);
void* recibir_mensaje_serv(int socket_cliente, int* size);
int recibir_operacion(int);
void process_request(int cod_op, int cliente_fd);
void serve_client(int *socket);
void* serializar_paquete(t_paquete* paquete, int bytes);
void devolver_mensaje(void* payload, int size, int socket_cliente);

//      LOGGER

t_log* iniciar_logger(char* file, char* program_name);
void terminar_logger(t_log* logger);

//		CONFIG
t_config* leer_config(char*);

#endif /* UTILS_UTILS_H_ */
