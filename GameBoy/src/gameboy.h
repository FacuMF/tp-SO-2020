#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

// Funciones
void inicializar_gameboy();
t_modulo string_a_modulo(char*);
op_code string_a_tipo_mensaje(char*);


// Funciones Mensajes
t_buffer* mensaje_a_enviar(t_modulo, op_code, char**);
//t_buffer* crear_serializar_appeared_pokemon(char*);

//Funciones Respuesta
void recibir_respuesta(int* socket_broker);
void handle_respuesta(int cod_op, int socket_broker);

//Pruebas de mensajes

/*
 * BROKER NEW_POKEMON BULBASAUR 3 1 5
 * BROKER APPEARED_POKEMON BULBASAUR 3 1 2
 * BROKER CATCH_POKEMON PIKACHU 3 1
 * BROKER CAUGHT_POKEMON 5 1
 * BROKER GET_POKEMON SQUIRTLE
 *
 * TEAM APPEARED_POKEMON CHARMANDER 4 6
 *
 * GAMECARD NEW_POKEMON BULBASAUR 3 1 5 8
 * GAMECARD CATCH_POKEMON PIKACHU 8 4 3
 * GAMECARD GET_POKEMON SQUIRTLE 3
 *
 * SUSCRIPTOR APPEARED_POKEMON 10
 * SUSCRIPTOR NEW_POKEMON 10
 *
 */
