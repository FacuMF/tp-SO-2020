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
#include "utils.h"


t_log* logger;

t_log* iniciar_logger(void);
void finalizar_ejecucion(t_log* );

#endif /* SERVIDOR_H_ */
