#include "gamecard.h"

void main(){
	// Logger
	logger = iniciar_logger("./Gamecard/config/gamecard.log", "Gamecard", LOG_LEVEL_TRACE);
	// Leer config
	config = leer_config("./Gamecard/config/gamecard.config");
	set_base_path(config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));
	REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	RETARDO_OPERACION = config_get_int_value(config,"TIEMPO_RETARDO_OPERACION");
	// Conexion socket
	int conexion = iniciar_conexion(IP_BROKER, PUERTO_BROKER);

	// Suscribirse al broker: NEW_POKEMON, CATCH_POKEMON, GET_POKEMON
	suscribirse_a(&conexion, NEW_POKEMON ); // NEW_POKEMON
	suscribirse_a(&conexion, CATCH_POKEMON ); // CATCH_POKEMON
	suscribirse_a(&conexion, GET_POKEMON ); // GET_POKEMON
	// "NEW_POKEMON" "CAUGHT_POKEMON" "GET_POKEMON" deben ser int
	// Esperar a recibir mensajes. Reintentar cada REINTENTO_CONEXION si no se logra conectar
	esperar_broker(NULL);

	// Al recibir: 	Informar la recepción del mismo (ACK)
	//				Crear hilo que atienda solicitud
	//				Volver al estado escucha de la MQ

	// Al recibir mensajes
	//recibir_mensajes_gamecard();

}


