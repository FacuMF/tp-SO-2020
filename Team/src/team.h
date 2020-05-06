
#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<commons/collections/list.h>
#include<readline/readline.h>

#include<pthread.h>
#include<unistd.h>

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"


t_log* logger;
t_config* config;



typedef struct pokemones {
	char * pokemon;
}pokemones;


typedef struct node {
    int posicion[2];
    char** pokemones_capturados;
    char** pokemones_por_capturar;
} entrenador;




void* doSomeThing(void *arg);
void iniciarListaEntrenador(entrenador *);
int length(entrenador *);

void cargar_y_localizar_entrenadores(t_list* head_entrenadores, char** posiciones, char** pokemones_capturados,char** objetivos);
int* de_string_a_posicion(char* string);
char** de_string_a_pokemones(char* string);


#endif /* TEAM_H_ */

