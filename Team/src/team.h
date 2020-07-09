#ifndef TEAM_H_
#define TEAM_H_

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"
#include <sys/time.h>

t_log* logger;
t_config* config;
pthread_mutex_t entrenadores_ready;		//TODO: No deberia ser mutex
pthread_mutex_t cpu_disponible;		//TODO: No deberia ser mutex

// Listas globales de pokemones y entrenadores
t_list * head_entrenadores;
t_list * ids_mensajes_utiles;
t_list * appeared_a_asignar;
t_list * appeared_auxiliares;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

typedef enum {
	Pikachu, Squirtle, Pidgey, Charmander, Bulbasaur
} t_pokemones;

typedef enum {
	A_FIFO, A_RR, A_SJFCD, A_SJFSD
} t_algoritmo;

typedef enum {
	NEW, READY, EXEC, BLOCKED_NORMAL, BLOCKED_ESPERANDO, BLOCKED_DEADLOCK, EXIT
} t_estado;

typedef struct t_entrenador {
	pthread_mutex_t sem_est;	//TODO: No deberia ser mutex
	int * posicion;
	t_list* pokemones_capturados;
	t_list* pokemones_por_capturar;
	t_catch_pokemon * catch_pendiente;
	t_estado estado;
	struct timeval ultima_modificacion;
	int ciclos_cpu_restantes;
	double estimacion_rafaga;
} t_entrenador;

// Variables para planificacion
t_algoritmo algoritmo_elegido;
int quantum;
double estimacion_inicial;
int retardo_ciclo_cpu;
int desalojar;
double constante_estimacion;

// PLANIFICACION GENERAL
int entrenadores_en_ready();
void iniciar_planificador();
void elegir_algoritmo();

// PLANIFICACION DE ENTRENADORES
t_entrenador * obtener_entrenador_a_planificar();
t_entrenador * obtener_entrenador_fifo(t_list * entrenadores);
t_entrenador * obtener_entrenador_sjf(t_list * entrenadores);
int timeval_subtract (struct timeval *x, struct timeval *y);
void preparar_entrenador(t_entrenador * entrenador, t_appeared_pokemon * mensaje_appeared);
void ejecutar_entrenador(t_entrenador * entrenador);
void actualizar_timestamp(t_entrenador * entrenador);

int objetivo_propio_cumplido(t_entrenador *entrenador);
void cazar_pokemon(t_entrenador * entrenador);
void mover_entrenador(t_entrenador * entrenador);
void bloquear_entrenador(t_entrenador * entrenador);

// FILTRADO DE ENTRENADORES
t_entrenador * obtener_entrenador_buscado(int posx, int posy);
t_list * obtener_entrenadores_disponibles(t_list * entrenadores);
t_list * encontrar_entrenadores_en_estado(t_estado estado_buscado,t_list * entrenadores);
t_list * obtener_entrenadores_con_espacio(t_list * entrenadores);
int tiene_espacio_disponible(t_entrenador * entrenador);

// OBJETIVOS
t_list* obtener_pokemones_por_capturar();
t_list * obtener_pokemones_capturados();
t_list * obtener_pokemones_necesitados();
t_list * obtener_pokemones_necesitados_sin_repetidos();
int requiero_pokemon(char * pokemon);
int cantidad_veces_requerido(char * pokemon);
int esta_en_lista(t_list * lista_pokemones,char * pokemon);
int cantidad_repeticiones_en_lista(t_list * lista_pokemones, char *pokemon );

// Funciones generales
void iniciar_team(char*archivo_config[]);
void inicializar_listas();

void finalizar_team();
char * obtener_path(char*string);

// Funciones de comunicacion general
void esperar_cliente(int socket_servidor);
void esperar_mensajes_cola(void* input);
void manejar_recibo_mensajes(int conexion,op_code cod_op,int es_respuesta);
int necesito_mensaje(int id_mensaje);

// Funciones de inicio de conexion
int iniciar_conexion_con_broker();
void iniciar_conexion_con_gameboy();

// Funciones de envio de mensajes y suscripciones
void suscribirse_a_colas_necesarias();
void enviar_suscripcion_broker(op_code tipo_mensaje);
void enviar_mensaje_suscripcion(op_code mensaje, int conexion);

void enviar_requests_pokemones();
void enviar_mensaje_get(void*element);
void enviar_mensaje_catch(t_catch_pokemon * mensaje_catch_a_enviar);

// Funciones de recepcion de mensajes
void recibir_mensaje_appeared(t_buffer * buffer);
void recibir_mensaje_caught(t_buffer * buffer);
void recibir_mensaje_localized(t_buffer * buffer);

// Funciones de handle de mensajes
void manejar_appeared(t_appeared_pokemon * mensaje_appeared);
void manejar_caught(t_caught_pokemon* mensaje_caught);
void manejar_localized(t_localized_pokemon* mensaje_localized);

// Funciones de carga de entrenador - team_entrenadores
t_list* cargar_entrenadores();
int* de_string_a_posicion(char* string);
t_list* string_a_pokemon_list(char* string);

//Funciones de obtencion de los pokemones
t_list* obtener_pokemones_a_capturar();
t_list * obtener_pokemones_capturados();
void aniadir_pokemon(t_list *pokemones_repetidos, void * pokemones);

//Funciones de mostrado de entrenador y pokemon
void mostrar_entrenadores(t_list * head_entrenadores);
void mostrar_data_entrenador(void * element);
void mostrar_kokemon(void*elemento);


// Filtro Mensajes
int pokemon_asignado_a_entrenador(char * pokemon);
int pokemon_en_pendientes(char * pokemon);
int mensaje_repetido(t_localized_pokemon * mensaje_localized);
int necesito_mensaje(int id_mensaje);


// Funciones de Manejo de Hilos - team_planificacion
void lanzar_hilo_entrenador(void*element);
void ser_entrenador(void *element);

//Funciones de Planificacion

// Funciones de auxiliares para handles
bool es_id_necesario(int id_a_chequear);
t_catch_pokemon * encontrar_en_lista_de_catch_pokemon (char * pokemon_a_encontrar);
t_catch_pokemon * de_appeared_a_catch(t_appeared_pokemon * appeared);
t_list * lista_de_catch_a_partir_localized(t_localized_pokemon * localized_a_chequear);
t_entrenador * buscar_entrenador_segun_id_mensaje(int id_mensaje);

int distancia(t_entrenador * entrenador, int posx, int posy);
int distancia_en_eje(t_entrenador *entrenador, int pos_eje, int pos);
t_entrenador * hallar_entrenador_mas_cercano(int posx, int posy, t_list *entrenadores);
void desbloquear_entrenador(t_entrenador * entrenador);
void mover_entrenador_a_posicion(t_entrenador*entrenador,int posx, int posy);
void cambiar_posicion_entrenador(t_entrenador*entrenador,int posx, int posy);


#endif /* TEAM_H_ */

