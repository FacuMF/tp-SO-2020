#ifndef CLIENTE_H_
#define CLIENTE_H_

#include "../../Utils/src/utils.h"
#include "../../Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

int iniciar_conexion(char* ip, char* puerto);
void handler_mensaje_texto(t_buffer* buffer_recepcion);

void terminar_programa(int, t_log*, t_config*);

//TBR
int recibir_mensaje_del_server(int* socket);
void process_request(int cod_op, int cliente_fd);

#endif /* CLIENTE_H_ */
