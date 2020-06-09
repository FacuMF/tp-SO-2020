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
void setear_socket_reusable(int socket);
void* iniciar_conexion_con_modulo(char* ip, char* puerto);
void handle_cliente(int socket_servidor);
void recibir_mensaje_del_cliente(int* socket);
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

// CATCH_POKEMON

// CAUGHT_POKEMON

// GET_POKEMON

// LOCALIZED_POKEMON

//CONFIRMACION
void confirmar_cliente_recibio(t_confirmacion* mensaje_confirmacion, int socket_cliente);
t_queue* get_cola_segun_tipo(int tipo_mensaje);
void si_coincide_cliente_agregar_id_recibido(t_suscriptor_queue* suscriptor, int socket_suscriptor, int id_mensaje_recibido);


#endif
