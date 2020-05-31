#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;


char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

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

typedef struct{
	t_modulo modulo;
	int instancia;
}t_cliente;

typedef struct{
	op_code tipo_mensaje;
	int id;
	int id_correlacional;
	t_list subscribers_enviados; //Subscriptores a esta cola
	t_list subscribers_ack; //Subscriptores que recibieron el mensaje
}t_mensaje;

void inicializacion_colas(void);

void* esperar_mensajes(void *arg);
void* iniciar_conexion_con_modulo(char* ip, char* puerto);

void setear_socket_reusable(int socket);

pthread_t tid[2];
