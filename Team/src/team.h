
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


typedef enum{
	Pikachu,
	Squirtle,
	Pidgey,
	Charmander,
	Bulbasaur
}t_pokemones;

//TODO: chequear si va abajo o arriba o ambos
typedef struct t_objetivo{
	char * pokemon;
	int cantidad;
}t_objetivo;

typedef struct t_entrenador {
    int * posicion;
    t_list* pokemones_capturados;
    t_list* pokemones_por_capturar;
} t_entrenador;

t_list* formar_objetivo(t_list * pokemones_repetidos);
void mostrarObjetivo(void *elemento);
void agrego_si_no_existe(t_list * objetivo_global,void *elemento);
void mostrarKokemon(void*elemento);
void mostrar_entrenadores(t_list * head_entrenadores);
t_list* cargar_entrenadores(char** posiciones, char** pokemones_capturados,char** objetivos);
int* de_string_a_posicion(char* string);
t_list* string_a_pokemon_list(char* string);
t_list* obtener_pokemones(t_list *head_entrenadores);

#endif /* TEAM_H_ */

