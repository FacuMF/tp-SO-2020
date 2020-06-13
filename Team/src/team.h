#ifndef TEAM_H_
#define TEAM_H_

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"


t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
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
	// tarea
} t_entrenador;

// TT
void manejar_recepcion_mensaje(int* socket_cliente);
void esperar_cliente(int socket_servidor);
void esperar_mensajes_cola(void* input);
void manejar_mensaje_cola(void* input);

// Funciones generales
void iniciar_team(char*archivo_config[]);
void finalizar_team();
char * obtener_path(char*string);

// Comunicacion Broker
void suscribirse_a_colas_necesarias();
void enviar_suscripcion_broker(op_code tipo_mensaje);
void enviar_mensaje_suscripcion(op_code mensaje, int conexion);
int iniciar_conexion_con_broker();

void enviar_requests_pokemones(t_list *objetivo_global);
void enviar_mensaje_get(int socket_broker, void*element);

//Comunicacion con Gameboy
void iniciar_conexion_con_gameboy();
t_appeared_pokemon * obtener_appeared_recibido(int socket_cliente);

// Funciones de carga de entrenador - team_entrenadores
t_list* cargar_entrenadores();
int* de_string_a_posicion(char* string);
t_list* string_a_pokemon_list(char* string);

//Funciones de obtencion de los pokemones
t_list* obtener_pokemones(t_list *head_entrenadores);
void aniadir_pokemon(t_list *pokemones_repetidos, void * pokemones);

//Funciones de mostrado de entrenador y pokemon
void mostrar_entrenadores(t_list * head_entrenadores);
void mostrar_data_entrenador(void * element);
void mostrar_kokemon(void*elemento);


//Funciones de objetivo general - team_objetivos
t_list* formar_objetivo(t_list * pokemones_repetidos);
void agrego_si_no_existe(t_list * objetivo_global, void *elemento);
void mostrar_objetivo(void *elemento);
bool objetivo_cumplido(t_entrenador *entrenador);
bool pokemon_igual(char *un_pokemon, char * otro_pokemon);
bool esta_en_lista(char * pokemon,t_list * lista_pokemones);
int cantidad_repeticiones_en_lista(char *pokemon, t_list * lista_pokemones);
bool es_repetido(char *pokemon, t_list *lista_pokemones);


// Funciones de Manejo de Hilos - team_planificacion
void lanzar_hilos(t_list *head_entrenadores);
void lanzar_hilo_entrenador(void*element);
void ser_entrenador(void *element);

//Funciones de Planificacion
int distancia(t_entrenador * entrenador, int posx, int posy);
//double suma_de_distancias_al_cuadrado(t_entrenador*entrenador, double posx, double posy);
int distancia_en_eje(t_entrenador *entrenador, int pos_eje, int pos);
void comenzar_planificacion_entrenadores(t_appeared_pokemon * appeared_recibido,t_list * head_entrenadores);
t_entrenador * hallar_entrenador_mas_cercano_segun_appeared(t_appeared_pokemon * appeared_recibido,t_list * head_entrenadores);
t_entrenador * hallar_entrenador_mas_cercano(t_list * head_entrenadores, int posx, int posy);
void desbloquear_entrenador(t_entrenador * entrenador);
void mover_entrenador_a_posicion(t_entrenador*entrenador,int posx, int posy);
void cambiar_posicion_entrenador(t_entrenador*entrenador,int posx, int posy);
// atrapar_pokemon(entrenador,appeared_pokemon);

#endif /* TEAM_H_ */

