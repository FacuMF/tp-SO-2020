/*
 * servidor.h
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#ifndef SERVIDOR_H_		// Si el header del servidor no fue definido previamente...
#define SERVIDOR_H_		// hacer todo lo que sigue

#include "../../Utils/src/utils.h"
#include "../../Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* log_nivel_key;
t_log_level log_nivel_minimo;

void iniciar_conexion_servidor(char* ip, char* puerto);
void finalizar_ejecucion(t_log*, t_config*);

// OTROS
void setear_socket_reusable(int);
void esperar_cliente(int);
void serve_client(int *socket);
void process_request(int cod_op, int cliente_fd);

#endif /* SERVIDOR_H_ */
