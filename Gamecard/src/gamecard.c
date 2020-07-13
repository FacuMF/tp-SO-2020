#include "gamecard.h"

void main(){
	// Logger
	logger = iniciar_logger("./Gamecard/config/gamecard.log", "Gamecard", LOG_LEVEL_TRACE);
	// Leer config
	config = leer_config("./Gamecard/config/gamecard.config");
	set_base_path(config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));

	// Suscribirse al broker: NEW_POKEMON, CATCH_POKEMON, GET_POKEMON
	suscribirse_a_colas_gamecard();
	// Esperar a recibir mensajes. Reintentar cada REINTENTO_CONEXION si no se logra conectar
	lanzar_hilo_espera_broker();

	// Al recibir: 	Informar la recepción del mismo (ACK)
	//				Crear hilo que atienda solicitud
	//				Volver al estado escucha de la MQ


}


