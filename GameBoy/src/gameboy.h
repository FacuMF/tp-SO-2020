#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

// Funciones
t_modulo string_a_modulo(char* nombre_modulo);
t_tipo_mensaje string_a_tipo_mensaje(t_modulo modulo, char* nombre_mensaje);


// Funciones Mensajes
t_buffer* crear_serializar_mensaje(t_modulo modulo,t_tipo_mensaje tipo_mensaje,char* arg);
//t_buffer* crear_serializar_appeared_pokemon(char* arg[]);

