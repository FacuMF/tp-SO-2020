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
	t_list * subscriptores; // lista de t_cliente
	t_list * mensajes; // lista de t_mensajes
}t_queue;

//Declaracion de queues
t_queue* new_pokemon;
t_queue* appeared_pokemon;
t_queue* catch_pokemon;
t_queue* caught_pokemon;
t_queue* get_pokemon;
t_queue* localized_pokemon;

//Declaracion id
int id_new_pokemon = 0;
int id_appeared_pokemon = 0;
int id_catch_pokemon = 0;
int id_caugth_pokemon = 0;
int id_get_pokemon = 0;
int id_localized_pokemon = 0;

typedef struct{
	op_code tipo_mensaje;
	int id;
	int id_correlacional;
	t_list subscribers_enviados; //Subscriptores a esta cola
	t_list subscribers_ack; //Subscriptores que recibieron el mensaje
}t_mensaje;

//Funciones Generales
void inicializacion_broker(void);
void lanzar_hilo_receptor_mensajes(void);
void terminar_proceso(void);

void inicializacion_colas(void);
void* esperar_mensajes(void *arg);
void setear_socket_reusable(int socket);
void* iniciar_conexion_con_modulo(char* ip, char* puerto);
void handle_cliente(int socket_servidor);
void recibir_mensaje_del_cliente(int* socket);
void handle_mensaje(int cod_op, int cliente_fd);

void enviar_mensaje_de_cola(void* mensaje, int ciente);


//Funciones especificas por mensaje

//SUSCRIPTOR
void subscribir(int cliente, t_subscriptor* subscripcion);
void agregar_cliente_a_cola(t_queue* cola, int cliente);
void enviar_mensajes_de_suscripcion_a_cliente(t_subscriptor subscripcion,  int cliente);
void enviar_mensajes_de_cola_a_cliente(t_queue* cola,  int cliente);

// APPEARED_POKEMON
int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje);
void devolver_appeared_pokemon(int socket_cliente ,t_appeared_pokemon mensaje_appeared_pokemon);
//void almacenar_en_cola_appeared_pokemon(t_mensaje_appeared_pokemon mensaje);
//void enviar_a_todos_los_subs_appeared_pokemon(mensaje);
//void cachear_appeared_pokemon(t_mensaje_appeared_pokemon mensaje);

// NEW_POKEMON

// CATCH_POKEMON

// CAUGHT_POKEMON

// GET_POKEMON

// LOCALIZED_POKEMON


#endif
