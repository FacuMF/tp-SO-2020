#ifndef BROKER_H_
#define BROKER_H_

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

pthread_t tid[2];

typedef struct{
	t_list* subscriptores; // lista de suscriptor_queue
	t_list* mensajes; // lista de mensajes ej:t_new_pokemon
}t_queue;

typedef struct{
	int socket;
	t_list* mensajes_recibidos; // Lista de IDs (int)
}t_suscriptor_queue;

/*
typedef struct{
	op_code tipo_mensaje;
	int id;
	int id_correlacional;
	t_list subscribers_enviados; //Subscriptores a esta cola
	t_list subscribers_ack; //Subscriptores que recibieron el mensaje
}t_mensaje;
*/

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
void lanzar_hilo_receptor_mensajes(void);
void terminar_proceso(void);

void inicializacion_colas(void);
void inizializacion_ids(void);

void* esperar_mensajes(void *arg);
void* iniciar_conexion_con_modulo(char* ip, char* puerto);
void handle_cliente(int socket_servidor);
void recibir_mensaje_del_cliente(void* );
void handle_mensaje(int cod_op, int cliente_fd);

void enviar_mensaje_de_cola(void* mensaje, int ciente);

int get_id_mensajes(void);


//Funciones especificas por mensaje

//SUSCRIPTOR
void subscribir(int cliente, t_subscriptor* subscripcion);
void agregar_cliente_a_cola(t_queue* cola, int cliente);
//void enviar_mensajes_de_suscripcion_a_cliente(t_subscriptor* subscripcion,  int cliente);
//void enviar_mensajes_de_cola_a_cliente(t_queue* cola,  int cliente);

// APPEARED_POKEMON
int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje);
void devolver_appeared_pokemon(int socket_cliente ,t_appeared_pokemon* mensaje_appeared_pokemon);
void almacenar_en_cola_appeared_pokemon(t_appeared_pokemon* mensaje);

void enviar_a_todos_los_subs_appeared_pokemon(t_appeared_pokemon* mensaje);
void enviar_appeared_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_appeared_pokemon* mensaje);
//void cachear_appeared_pokemon(t_mensaje_appeared_pokemon mensaje);

// NEW_POKEMON
int asignar_id_new_pokemon(t_new_pokemon* mensaje);
void devolver_new_pokemon(int socket_cliente ,t_new_pokemon* mensaje_new_pokemon);
void almacenar_en_cola_new_pokemon(t_new_pokemon* mensaje);

void enviar_a_todos_los_subs_new_pokemon(t_new_pokemon* mensaje);
void enviar_new_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_new_pokemon* mensaje);
//void cachear_new_pokemon(t_mensaje_new_pokemon mensaje);

// CATCH_POKEMON
int asignar_id_catch_pokemon(t_catch_pokemon* mensaje);
void devolver_catch_pokemon(int socket_cliente ,t_catch_pokemon* mensaje_catch_pokemon);
void almacenar_en_cola_catch_pokemon(t_catch_pokemon* mensaje);

void enviar_a_todos_los_subs_catch_pokemon(t_catch_pokemon* mensaje);
void enviar_catch_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_catch_pokemon* mensaje);
//void cachear_catch_pokemon(t_mensaje_catch_pokemon mensaje);

// CAUGHT_POKEMON
int asignar_id_caugth_pokemon(t_caught_pokemon* mensaje);
void devolver_caugth_pokemon(int socket_cliente ,t_caught_pokemon* mensaje_caugth_pokemon);
void almacenar_en_cola_caugth_pokemon(t_caught_pokemon* mensaje);

void enviar_a_todos_los_subs_caugth_pokemon(t_caught_pokemon* mensaje);
void enviar_caugth_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_caught_pokemon* mensaje);
//void cachear_caugth_pokemon(t_mensaje_caugth_pokemon mensaje);

// GET_POKEMON
int asignar_id_get_pokemon(t_get_pokemon* mensaje);
void devolver_get_pokemon(int socket_cliente ,t_get_pokemon* mensaje_get_pokemon);
void almacenar_en_cola_get_pokemon(t_get_pokemon* mensaje);

void enviar_a_todos_los_subs_get_pokemon(t_get_pokemon* mensaje);
void enviar_get_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_get_pokemon* mensaje);
//void cachear_get_pokemon(t_mensaje_get_pokemon mensaje);

// LOCALIZED_POKEMON
int asignar_id_localized_pokemon(t_localized* mensaje);
void devolver_localized_pokemon(int socket_cliente ,t_localized* mensaje_localized_pokemon);
void almacenar_en_cola_localized_pokemon(t_localized* mensaje);

void enviar_a_todos_los_subs_localized_pokemon(t_localized* mensaje);
void enviar_localized_pokemon_a_suscriptor(t_suscriptor_queue* suscriptor, t_localized* mensaje);
//void cachear_localized_pokemon(t_mensaje_localized_pokemon mensaje);

//CONFIRMACION
void confirmar_cliente_recibio(t_confirmacion* mensaje_confirmacion, int socket_cliente);
t_queue* get_cola_segun_tipo(int tipo_mensaje);
void si_coincide_cliente_agregar_id_recibido(t_suscriptor_queue* suscriptor, int socket_suscriptor, int id_mensaje_recibido);


#endif
