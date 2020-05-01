
#ifndef CLIENTE_H_
#define CLIENTE_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include "../../Utils/src/utils.h"

t_log* logger;
t_config* config;


t_config* leer_config(void);
void terminar_programa(int, t_log*, t_config*);

#endif /* CLIENTE_H_ */
