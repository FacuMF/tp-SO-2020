#ifndef BROKER_H_
#define BROKER_H_

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

pthread_t tid[2];

//Memoria cache
void* memoria_cache;
t_list* struct_admin_cache;

int tamano_memoria;
int tamano_minimo_particion;
int algoritmo_memoria;
int algoritmo_remplazo;
int algoritmo_particion_libre;
int frecuencia_compactacion;


typedef struct{
	t_list* subscriptores; // lista de suscriptor_queue
	t_list* mensajes; // lista de mensajes ej:t_new_pokemon
}t_queue;

typedef struct{
	int socket;
	t_list* mensajes_enviados; // Lista de IDs (int)
	t_list* mensajes_recibidos;	// Lista de IDs (int)
}t_suscriptor_queue;

//Cache

typedef struct{
	op_code tipo_mensaje;
	int id;
	t_list subscribers_enviados; // Subscriptores a la cola a los que fue enviado
	t_list subscribers_recibidos; // Subscriptores que recibieron el mensaje
	int offset; //Bytes
	int tamanio; //Bytes
	int flags_lru;
}t_mensaje_cache;

//Structs auxiliares

typedef struct {
	int op_code;
	int socket_cliente;
}t_info_mensaje; // Para handle_mensaje.

typedef struct {
	int conexion;
	t_buffer * buffer;
}t_conexion_buffer; // Para handle_mensaje.

//Enums algoritmos

typedef enum {
	PARTICIONES = 0,
	BS = 1
}t_algoritmo_memoria;

typedef enum {
	FIFO = 0,
	LRU = 1
}t_algoritmo_remplazo;

typedef enum {
	FF=0,
	BF=1
}t_algoritmo_particion_libre;

//Declaracion de queues
t_queue* new_pokemon;
t_queue* appeared_pokemon;
t_queue* catch_pokemon;
t_queue* caught_pokemon;
t_queue* get_pokemon;
t_queue* localized_pokemon;

//Declaracion id
int id_mensajes;

//Funciones Generales
void inicializacion_broker(void);
void terminar_proceso(void);

void inicializacion_colas(void);
void inicializacion_ids(void);
void inicializacion_cache(void);

void* esperar_mensajes(void *arg);
void handle_cliente(int socket_servidor);
void recibir_mensaje_del_cliente(void* );
void handle_mensaje(int cod_op, int socket_cliente);

void enviar_mensaje_de_cola(void* mensaje, int ciente);

int get_id_mensajes(void);

//Cache

int de_string_a_alg_memoria(char* string);
int de_string_a_alg_remplazo(char* string);
int de_string_a_alg_particion_libre(char* string);

//Funciones especificas por mensaje

//SUSCRIPTOR
void manejar_mensaje_suscriptor(t_conexion_buffer *combo);

void subscribir(int cliente, t_subscriptor* subscripcion);
void agregar_cliente_a_cola(t_queue* cola, int cliente);

//void enviar_mensajes_de_suscripcion_a_cliente(t_subscriptor* subscripcion,  int cliente);
//void enviar_mensajes_de_cola_a_cliente(t_queue* cola,  int cliente);

void desuscribir(int cliente, t_subscriptor* suscripcion);
void sacar_cliente_a_cola(t_queue* cola, int cliente);

// APPEARED_POKEMON
void manejar_mensaje_appeared(t_conexion_buffer *combo);

int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje);
void devolver_appeared_pokemon(int socket_cliente ,t_appeared_pokemon* mensaje_appeared_pokemon);
void almacenar_en_cola_appeared_pokemon(t_appeared_pokemon* mensaje);

void enviar_a_todos_los_subs_appeared_pokemon(t_appeared_pokemon* mensaje);
void enviar_appeared_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_appeared_pokemon* mensaje);
void cachear_appeared_pokemon(t_appeared_pokemon* mensaje);

// NEW_POKEMON
void manejar_mensaje_new(t_conexion_buffer *combo);

int asignar_id_new_pokemon(t_new_pokemon* mensaje);
void devolver_new_pokemon(int socket_cliente ,t_new_pokemon* mensaje_new_pokemon);
void almacenar_en_cola_new_pokemon(t_new_pokemon* mensaje);

void enviar_a_todos_los_subs_new_pokemon(t_new_pokemon* mensaje);
void enviar_new_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_new_pokemon* mensaje);
//void cachear_new_pokemon(t_mensaje_new_pokemon mensaje);

// CATCH_POKEMON
void manejar_mensaje_catch(t_conexion_buffer *combo);

int asignar_id_catch_pokemon(t_catch_pokemon* mensaje);
void devolver_catch_pokemon(int socket_cliente ,t_catch_pokemon* mensaje_catch_pokemon);
void almacenar_en_cola_catch_pokemon(t_catch_pokemon* mensaje);

void enviar_a_todos_los_subs_catch_pokemon(t_catch_pokemon* mensaje);
void enviar_catch_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_catch_pokemon* mensaje);
//void cachear_catch_pokemon(t_mensaje_catch_pokemon mensaje);

// CAUGHT_POKEMON
void manejar_mensaje_caught(t_conexion_buffer *combo);

int asignar_id_caught_pokemon(t_caught_pokemon* mensaje);
void devolver_caught_pokemon(int socket_cliente ,t_caught_pokemon* mensaje_caught_pokemon);
void almacenar_en_cola_caught_pokemon(t_caught_pokemon* mensaje);

void enviar_a_todos_los_subs_caught_pokemon(t_caught_pokemon* mensaje);
void enviar_caught_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_caught_pokemon* mensaje);
//void cachear_caugth_pokemon(t_mensaje_caugth_pokemon mensaje);

// GET_POKEMON
void manejar_mensaje_get(t_conexion_buffer *combo);

int asignar_id_get_pokemon(t_get_pokemon* mensaje);
void devolver_get_pokemon(int socket_cliente ,t_get_pokemon* mensaje_get_pokemon);
void almacenar_en_cola_get_pokemon(t_get_pokemon* mensaje);

void enviar_a_todos_los_subs_get_pokemon(t_get_pokemon* mensaje);
void enviar_get_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_get_pokemon* mensaje);
//void cachear_get_pokemon(t_mensaje_get_pokemon mensaje);

// LOCALIZED_POKEMON
void manejar_mensaje_localized(t_conexion_buffer *combo);

int asignar_id_localized_pokemon(t_localized* mensaje);
void devolver_localized_pokemon(int socket_cliente ,t_localized* mensaje_localized_pokemon);
void almacenar_en_cola_localized_pokemon(t_localized* mensaje);

void enviar_a_todos_los_subs_localized_pokemon(t_localized* mensaje);
void enviar_localized_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_localized* mensaje);
//void cachear_localized_pokemon(t_mensaje_localized_pokemon mensaje);

//CONFIRMACION
void manejar_mensaje_confirmacion(t_conexion_buffer *combo);

void confirmar_cliente_recibio(t_confirmacion* mensaje_confirmacion, int socket_cliente);
t_queue* get_cola_segun_tipo(int tipo_mensaje);

void si_coincide_cliente_agregar_id_recibido(t_suscriptor_queue* suscriptor, int socket_suscriptor, int id_mensaje_recibido);
_Bool mensaje_recibido_por_todos_los_subs(t_confirmacion* confirmacion);
void borrar_mensaje_de_cola(t_confirmacion* confirmacion);
_Bool fue_enviado_y_recibido(int id_mensaje, t_suscriptor_queue* suscriptor);


#endif
