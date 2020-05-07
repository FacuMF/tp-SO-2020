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


// Funciones
t_modulo string_a_modulo(char* nombre_modulo);
char* leer_ip(t_modulo modulo);
char* leer_puerto(t_modulo modulo);

// Funciones Sockets
int iniciar_conexion(char *ip, char* puerto);
