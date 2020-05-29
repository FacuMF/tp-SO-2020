#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

typedef struct{
	t_list subscriptores;
	t_list mensajes; // lista de t_mensajes
}t_queue;

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

pthread_t tid[2];
