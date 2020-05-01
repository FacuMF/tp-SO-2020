/*
 * servidor.h
 *
 *  Created on: 3 mar. 2019
 *      Author: utnso
 */

#ifndef SERVIDOR_H_		// Si el header del servidor no fue definido previamente...
#define SERVIDOR_H_		// hacer todo lo que sigue

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>
#include "../../Utils/src/utils.h"


t_log* logger;

void finalizar_ejecucion(t_log*,t_config*);

#endif /* SERVIDOR_H_ */
