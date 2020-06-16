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
int recibir_respuesta(int* socket_broker);
void handle_respuesta(int cod_op, int socket_broker);

//Pruebas de mensajes

/*./bin/GameBoy
 * BROKER NEW_POKEMON BULBASAUR 3 1 5
 * BROKER APPEARED_POKEMON BULBASAUR 3 1 2
 * BROKER CATCH_POKEMON PIKACHU 3 1
 * BROKER CAUGHT_POKEMON 5 1
 * BROKER GET_POKEMON SQUIRTLE
 * BROKER LOCALIZED_POKEMON PIKACHU 2 1 3 4 5
 *
 * TEAM APPEARED_POKEMON CHARMANDER 4 6
 *
 * GAMECARD NEW_POKEMON BULBASAUR 3 1 5 8
 * GAMECARD CATCH_POKEMON PIKACHU 8 4 3
 * GAMECARD GET_POKEMON SQUIRTLE 3
 *
 * SUSCRIPTOR NEW_POKEMON -1
 * SUSCRIPTOR APPEARED_POKEMON -1
 * SUSCRIPTOR CATCH_POKEMON -1
 * SUSCRIPTOR CAUGHT_POKEMON -1
 * SUSCRIPTOR GET_POKEMON -1
 * SUSCRIPTOR LOCALIZED_POKEMON -1
 *
 *
 * ./bin/GameBoy SUSCRIPTOR LOCALIZED_POKEMON -1
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON PIKACHU 1 1 3
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON BULBASAUR 2 1 3 4 5
 * ./bin/GameBoy BROKER LOCALIZED_POKEMON PIKACHU 3 1 3 4 5 3 4
 */
