#ifndef TEAM_H_
#define TEAM_H_

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"


t_log* logger;
t_config* config;

char* log_nivel_key;
t_log_level log_nivel_minimo;

typedef enum {
	Pikachu, Squirtle, Pidgey, Charmander, Bulbasaur
} t_pokemones;

//TODO: chequear si va abajo o arriba o ambos
typedef struct t_objetivo {
	char * pokemon;
	int cantidad;
} t_objetivo;

typedef struct t_entrenador {
	pthread_mutex_t sem_est;
	int * posicion;
	t_list* pokemones_capturados;
	t_list* pokemones_por_capturar;
} t_entrenador;

// Funciones de carga de entrenador
t_list* cargar_entrenadores(char** posiciones, char** pokemones_capturados,
		char** objetivos);
int* de_string_a_posicion(char* string);
t_list* string_a_pokemon_list(char* string);
char * obtener_path(char*string);

//Funciones de mostrado de entrenador y pokemon  (TBR)
void mostrar_entrenadores(t_list * head_entrenadores);
void mostrar_data_entrenador(void * element);
void mostrar_kokemon(void*elemento);

//Funciones de obtencion de los pokemones
t_list* obtener_pokemones(t_list *head_entrenadores);
void aniadir_pokemon(t_list *pokemones_repetidos, void * pokemones);

//Funciones de objetivo general
t_list* formar_objetivo(t_list * pokemones_repetidos);
void agrego_si_no_existe(t_list * objetivo_global, void *elemento);
void mostrar_objetivo(void *elemento);

//Funciones socket con gameboy
void iniciar_conexion_servidor(char* ip, char* puerto);
void setear_socket_reusable(int);
void esperar_cliente(int);
void serve_client(int *socket);
void process_request(int cod_op, int cliente_fd);

// Funciones de Manejo de Hilos
void lanzar_hilos(void*element);
void ser_entrenador(void *element);
int objetivo_cumplido(t_entrenador*entrenador);

//Funciones de Planificacion
double distancia(t_entrenador * entrenador, double posx, double posy);
double suma_de_distancias_al_cuadrado(t_entrenador*entrenador, double posx, double posy);
double distancia_en_eje(t_entrenador *entrenador, double pos_eje, int pos);
t_entrenador * hallar_entrenador_mas_cercano(t_list * head_entrenadores, double posx, double posy);

#endif /* TEAM_H_ */

