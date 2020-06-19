#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

int es_suscriptor; //Para saber si enviar confirmacion o no

// Funciones
void inicializar_gameboy();
t_buffer* mensaje_a_enviar(t_modulo, op_code, char**);
t_modulo obtener_modulo(char**);
op_code obtener_tipo_mensaje(char**);

//Funciones Respuesta
int recibir_respuesta(int*);
void handle_respuesta(int, int);
void confirmar_si_es_suscriptor(int, int, int);

//Carga de mensajes
void cargar_parametros_appeared_pokemon(char*, int*, int*, int*, char**, int);
void cargar_parametros_new_pokemon(char*, int*, int*, int*, int*, char**, int);
void cargar_parametros_caught_pokemon(int*, int*, char**);
void cargar_parametros_catch_pokemon(char*, int*, int*, int*, char**, int);
void cargar_parametros_get_pokemon(char*, int*, char**, int);
void cargar_parametros_suscriptor(int*, int*, char**);
//Pruebas de mensajes

/*
 * ./bin/GameBoy BROKER NEW_POKEMON BULBASAUR 3 1 5
 * ./bin/GameBoy BROKER APPEARED_POKEMON BULBASAUR 3 1 2
 * ./bin/GameBoy BROKER CATCH_POKEMON PIKACHU 3 1
 * ./bin/GameBoy BROKER CAUGHT_POKEMON 5 1
 * ./bin/GameBoy BROKER GET_POKEMON SQUIRTLE
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON PIKACHU 2 1 3 4 5
 *
 * ./bin/GameBoy TEAM APPEARED_POKEMON CHARMANDER 4 6
 *
 * ./bin/GameBoy GAMECARD NEW_POKEMON BULBASAUR 3 1 5 8
 * ./bin/GameBoy GAMECARD CATCH_POKEMON PIKACHU 8 4 3
 * ./bin/GameBoy GAMECARD GET_POKEMON SQUIRTLE 3
 *
 * ./bin/GameBoy SUSCRIPTOR NEW_POKEMON -1
 * ./bin/GameBoy SUSCRIPTOR APPEARED_POKEMON -1
 * ./bin/GameBoy SUSCRIPTOR CATCH_POKEMON -1
 * ./bin/GameBoy SUSCRIPTOR CAUGHT_POKEMON -1
 * ./bin/GameBoy SUSCRIPTOR GET_POKEMON -1
 * ./bin/GameBoy SUSCRIPTOR LOCALIZED_POKEMON -1
 *
 *
 * ./bin/GameBoy SUSCRIPTOR LOCALIZED_POKEMON -1
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON PIKACHU 1 1 3
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON BULBASAUR 2 1 3 4 5
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON PIKACHU 3 1 3 4 5 3 4
 */
