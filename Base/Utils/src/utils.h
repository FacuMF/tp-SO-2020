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
#include<readline/readline.h>
#include<pthread.h>
#include<math.h>

//Definiciones de modulos
typedef enum
{
	team = 1,
	gamecard = 2,
	broker = 3,
}t_modulo;

// OP CODES
typedef enum
{
	VACIO = 0, //Para las particiones en cache.
	TEXTO = 1,
	APPEARED_POKEMON = 2,
	NEW_POKEMON = 3,
	CAUGHT_POKEMON = 4,
	CATCH_POKEMON = 5,
	GET_POKEMON = 6,
	LOCALIZED_POKEMON = 7,
	SUSCRIPTOR = 8,
	CONFIRMACION = 9
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


pthread_t thread;

// INICIALIZACION DE LA CONEXION
struct addrinfo* obtener_server_info(char* , char*);
int obtener_socket(struct addrinfo* );
void asignar_socket_a_puerto(int,struct addrinfo*);

// FUNCIONES DE INICIALIZACION DE LA CONEXION PARA EL CLIENTE
int iniciar_conexion(char*, char*);

// FUNCIONES DE INICIALIZACION DE LA CONEXION PARA EL SERVIDOR
int iniciar_conexion_servidor(char* ip, char* puerto);
void setear_socket_reusable(int socket);
void esperar_cliente(int socket_servidor);
void serve_client(int* socket);
int iniciar_conexion_servidor(char* ip,char* puerto);
int aceptar_cliente(int socket_servidor);

//TBR
int sockets_mensajes[3];
int numero_mensajes;



// MANIPULACION MENSAJES
void enviar_mensaje(int , t_buffer* , op_code );

t_paquete* generar_paquete(t_buffer* , op_code); // DE UN BUFFER Y OP CODE SACA UN PAQUETE
void* serializar_paquete(t_paquete* paquete, int *bytes); // DE UN PAQUETE OBTENGO UN STREAM Y LOS BYTES

op_code recibir_codigo_operacion(int socket);
t_buffer* recibir_mensaje(int socket);// DE UN SOCKET, DEVUELVE UN BUFFER Y UN OPCODE POR REFERENCIA.


// TERMINAR CONEXION
void liberar_conexion(int socket_cliente);

//      LOGGER

t_log* iniciar_logger(char*, char*, t_log_level);
void terminar_logger(t_log*);

//		CONFIG
t_config* leer_config(char*);
char* leer_ip(t_modulo, t_config*);
char* leer_puerto(t_modulo, t_config*);

#endif /* UTILS_UTILS_H_ */
