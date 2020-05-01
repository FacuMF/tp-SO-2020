
#ifndef TEAM_H_
#define TEAM_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>

#include<string.h>
#include<pthread.h>
#include<unistd.h>



typedef struct pokemones {
	char * pokemon;
    struct pokemones * next;
}pokemones;


typedef struct node {
    int posX;
    int posY;
    pokemones pokemonesObtenido;
    pokemones pokemonesPorObtener;
    struct node * next;
} entrenador;




void* doSomeThing(void *arg);
void iniciarListaEntrenador(entrenador *);
int length(entrenador *);


#endif /* TEAM_H_ */

