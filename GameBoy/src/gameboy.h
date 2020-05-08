#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

typedef enum
{
	team = 1,
	gamecard = 2,
	broker = 3,
}t_modulo;

typedef enum
{
	msg_new_pokemon,
	msg_catch_pokemon,
	msg_caugth_pokemon,
	msg_get_pokemon,
	msg_appeared_pokemon,
	msg_id_new_pokemon,
	msg_id_catch_pokemon,
	msg_id_appeared_pokemon,
	msg_subscriptor,

}t_tipo_mensaje;


// Funciones
t_modulo string_a_modulo(char* nombre_modulo);
t_tipo_mensaje string_a_tipo_mensaje(t_modulo modulo, char* nombre_mensaje);
char* leer_ip(t_modulo modulo);
char* leer_puerto(t_modulo modulo);

// Funciones Mensajes
t_buffer* crear_serializar_mensaje(t_modulo modulo,t_tipo_mensaje tipo_mensaje,void* arg);
t_buffer* crear_serializar_appeared_pokemon(char* arg[]);


// Funciones Sockets
int iniciar_conexion(char *ip, char* puerto);
