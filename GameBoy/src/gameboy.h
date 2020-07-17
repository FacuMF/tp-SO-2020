#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"

t_log* logger;
t_config* config;

char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

int es_suscriptor; //Para saber si enviar confirmacion o no
t_modulo modulo;
op_code tipo_mensaje;
char* ip;
char* puerto;
int conexion;
int cod_op_respuesta;

// Funciones
void inicializar_gameboy();
t_buffer* mensaje_a_enviar(t_modulo, op_code, char**);
t_modulo obtener_modulo(char**);
op_code obtener_tipo_mensaje(char**);
void obtener_argumentos(char**);
void enviar_mensaje_gameboy(char**);
void esperar_respuesta(void);

//Funciones Respuesta
int recibir_respuesta(int*);
void handle_respuesta(int, int);
void confirmar_si_es_suscriptor(int, int, int);

//Carga de mensajes
void cargar_parametros_appeared_pokemon(char*, int*, int*, int*, char**, int);
void cargar_parametros_new_pokemon(char*, int*, int*, int*, int*, char**, int);
void cargar_parametros_caught_pokemon(int*, int*, char**);
int ok_fail_to_int(char*);
void cargar_parametros_catch_pokemon(char*, int*, int*, int*, char**, int);
void cargar_parametros_get_pokemon(char*, int*, char**, int);
void cargar_parametros_suscriptor(int*, int*, char**);
//Pruebas de mensajes

/*
 * ./gameboy BROKER NEW_POKEMON BULBASAUR 3 1 5
 * ./gameboy BROKER APPEARED_POKEMON BULBASAUR 3 1 2
 * ./gameboy BROKER CATCH_POKEMON PIKACHU 3 1
 * ./gameboy BROKER CAUGHT_POKEMON 1 5
 * ./gameboy BROKER GET_POKEMON SQUIRTLE
 * ./gameboy BROKER LOCALIZED_POKEMON PIKACHU 2 1 3 4 5
 *
 * ./gameboy TEAM APPEARED_POKEMON CHARMANDER 4 6
 *
 * ./gameboy GAMECARD NEW_POKEMON BULBASAUR 3 1 5 8
 * ./gameboy GAMECARD CATCH_POKEMON PIKACHU 8 4 3
 * ./gameboy GAMECARD GET_POKEMON SQUIRTLE 3
 *
 * ./gameboy SUSCRIPTOR NEW_POKEMON -1
 * ./gameboy SUSCRIPTOR APPEARED_POKEMON -1
 * ./gameboy SUSCRIPTOR CATCH_POKEMON -1
 * ./gameboy SUSCRIPTOR CAUGHT_POKEMON -1
 * ./gameboy SUSCRIPTOR GET_POKEMON -1
 * ./gameboy SUSCRIPTOR LOCALIZED_POKEMON -1
 *
 *
 * ./gameboy SUSCRIPTOR LOCALIZED_POKEMON -1
 * ./gameboy BROKER LOCALIZED_POKEMON PIKACHU 1 1 3
 * ./gameboy BROKER LOCALIZED_POKEMON BULBASAUR 2 1 3 4 5
 * ./gameboy BROKER LOCALIZED_POKEMON PIKACHU 3 1 3 4 5 3 4
 */


