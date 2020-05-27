#ifndef CLIENTE_H_
#define CLIENTE_H_

#include "../../Utils/src/utils.h"
#include "../../Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* log_nivel_key;
t_log_level log_nivel_minimo;

int iniciar_conexion(char* ip, char* puerto);
void handler_mensaje_texto(t_buffer* buffer_recepcion);

void terminar_programa(int, t_log*, t_config*);

#endif /* CLIENTE_H_ */
