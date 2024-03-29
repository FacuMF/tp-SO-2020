#ifndef TEAM_H_
#define TEAM_H_

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"
#include <sys/time.h>

typedef enum {
	A_FIFO, A_RR, A_SJFCD, A_SJFSD
} t_algoritmo;

typedef enum {
	NEW, READY, EXEC, BLOCKED_NORMAL, BLOCKED_ESPERANDO, BLOCKED_DEADLOCK, EXIT
} t_estado;

typedef struct t_deadlock{
	t_list * capturados_ep;
	int posx;
	int posy;
	char* pokemon_dar;
	char * pokemon_recibir;
	char id;
} t_deadlock;

typedef struct t_entrenador {
	char id;
	sem_t sem_est;
	int ciclos_cpu_totales;
	int * posicion;
	t_list* pokemones_capturados;
	t_list* pokemones_por_capturar;
	t_catch_pokemon * catch_pendiente;
	t_estado estado;
	struct timeval ultima_modificacion;
	int ciclos_cpu_restantes;
	double estimacion_rafaga;
	t_deadlock * deadlock;
} t_entrenador;

// VARIABLES COMPARTIDAS
// Generales
t_log* logger;
t_config* config;
char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;
char * ip_broker;
char * puerto_broker;

// Semaforos
sem_t entrenadores_ready;
sem_t cpu_disponible;
sem_t verificar_objetivo_global;

pthread_mutex_t chequeo_sem_suscrip;
sem_t suscripcion;

sem_t resolver_deadlock;

sem_t cpu_disponible_sjf;

sem_t sincro_deadlock;

pthread_mutex_t manejar_mensaje;
pthread_mutex_t mutex_hilos;
pthread_mutex_t mutex_pokemones_necesitados;
pthread_mutex_t mutex_ids_mensajes;

// Listas globales de pokemones y entrenadores
t_list * head_entrenadores;
t_list * ids_mensajes_utiles;
t_list * appeared_a_asignar;
t_list * appeared_auxiliares;
t_list * pokemones_recibidos;
t_list * pokemones_necesitados;


// Planificacion
t_algoritmo algoritmo_elegido;
int quantum;
double estimacion_inicial;
int retardo_ciclo_cpu;
int desalojar;
double constante_estimacion;
int tiempo_reconexion;

// Metricas
int deadlocks_resueltos;
int cambios_contexto_totales;

// METRICAS
void log_metricas();
void log_cpu_entrenador(void * element);
int obtener_ciclos_cpu_totales();

// DEADLOCK
void iniciar_deteccion_deadlock();
void planificar_entrenador_deadlock(t_entrenador * entrenador,t_deadlock * deadlock);
char * obtener_pokemon_a_dar(t_list * pok_sobrantes,t_entrenador * entrenador_ideal);
t_entrenador * obtener_entrenador_ideal(t_list * posibles_pasivos, t_entrenador * entrenador);
t_list * entrenadores_con_pokemon_sobrante(char * pokemon);
int deadlocks_pendientes();
t_list * obtener_pokemones_faltantes(t_entrenador * entrenador);
t_list * obtener_pokemones_sobrantes(t_entrenador * entrenador);

// PLANIFICACION GENERAL
void iniciar_planificador();
t_entrenador * obtener_entrenador_a_planificar();
t_entrenador * obtener_entrenador_fifo(t_list * entrenadores);
t_entrenador * obtener_entrenador_sjf(t_list * entrenadores);

// Auxiliares
int objetivo_global_completo();
int timeval_subtract (struct timeval *x, struct timeval *y);
int entrenadores_en_ready();

// LISTAS ENTRENADORES
t_entrenador * obtener_entrenador_buscado(int posx, int posy);
t_list * obtener_entrenadores_disponibles(t_list * entrenadores);
t_list * obtener_entrenadores_en_estado(t_estado estado_buscado,t_list * entrenadores);
t_list * obtener_entrenadores_con_espacio(t_list * entrenadores);
t_entrenador * obtener_entrenador_segun_id_mensaje(int id_mensaje);
t_entrenador * obtener_entrenador_mas_cercano(int posx, int posy, t_list *entrenadores);
int cantidad_entrenadores_buscando_pokemon(char * pokemon);

// Auxiliares
int distancia(t_entrenador * entrenador, int posx, int posy);
int tiene_espacio_disponible(t_entrenador * entrenador);

// OBJETIVO
t_list * obtener_pokemones_necesitados_sin_repetidos();

// Auxiliares
void eliminar_si_esta(t_list * lista, char * pokemon);
int requiero_pokemon(char * pokemon);
int pokemon_en_lista(t_list * lista_pokemones,char * pokemon);
int cantidad_repeticiones_en_lista(t_list * lista_pokemones, char *pokemon );

// MANEJO MENSAJES
void manejar_appeared(t_appeared_pokemon * mensaje_appeared);
void manejar_appeared_aux(void * element);
void manejar_caught(t_caught_pokemon* mensaje_caught,t_entrenador * entrenador);
void manejar_localized(t_localized_pokemon* mensaje_localized);

// Auxiliares - Pokemon
void verificar_pendientes(void);
int pokemon_en_auxiliares(char * pokemon);
int pokemon_asignado_a_entrenador(char * pokemon);

// Auxiliares - Mensajes
int necesito_mensaje(int id_mensaje);
t_appeared_pokemon * obtener_auxiliar_de_lista(char * pokemon);
void eliminar_de_lista_appeared(t_list * appeared_auxiliares, char * pokemon);
t_list * de_localized_a_lista_appeared(t_localized_pokemon * localized_a_chequear);
t_catch_pokemon * de_appeared_a_catch(t_appeared_pokemon * appeared);

// ENTRENADOR
void ser_entrenador(void *element);
void cazar_pokemon(t_entrenador * entrenador);
void realizar_intercambio(t_entrenador * entrenador);
void mover_entrenador(t_entrenador * entrenador);

// Cambio Estado
void log_entrenador_ser(t_entrenador * entrenador, char * estado);
void bloquear_entrenador(t_entrenador * entrenador);
void ejecutar_entrenador(t_entrenador * entrenador);
void planificar_entrenador(t_entrenador * entrenador, t_appeared_pokemon * mensaje_appeared);
void preparar_entrenador(t_entrenador * entrenador, t_appeared_pokemon * mensaje_appeared);
void actualizar_timestamp(t_entrenador * entrenador);

// Auxiliares
int objetivo_propio_cumplido(t_entrenador *entrenador);

// COMUNICACION GENERAL
int iniciar_conexion_con_broker();
int iniciar_conexion_con_broker_reintento();
void iniciar_conexion_con_gameboy();

void esperar_cliente(int socket_servidor);
void esperar_mensajes_cola(void* input);
void esperar_mensaje_gameboy(void* input);
int manejar_recibo_mensajes(int conexion,op_code cod_op,int es_respuesta);

// COMUNICACION ENVIO
void suscribirse_a_colas_necesarias();
void enviar_suscripcion_broker(op_code tipo_mensaje);
void enviar_mensaje_suscripcion(op_code mensaje, int conexion);

void enviar_requests_pokemones();
void enviar_mensaje_get(void*element);
void enviar_mensaje_catch(void * element);

// GENERALES - Main
void iniciar_team(char*archivo_config[]);
void finalizar_team();
void liberar_entrenador(void * element);

// GENERALES - INICIALIZACION
void inicializar_listas();
void cargar_pokemones_necesitados();
void reintento_suscripcion_si_aplica();
void cargar_entrenadores();
void agregar_capturados(char ** pokemones_capturados);
void lanzar_hilo_entrenador(void*element);

// Config
void iniciar_config_team(char* args);
char * obtener_path(char*string);
void obtener_variables_planificacion();
void elegir_algoritmo();
int* de_string_a_posicion(char* string);
t_list* string_a_pokemon_list(char* string);

// Logger
void iniciar_logger_team();
void mostrar_entrenadores(t_list * head_entrenadores);
void mostrar_data_entrenador(void * element);
void mostrar_kokemon(void*elemento);

#endif /* TEAM_H_ */

