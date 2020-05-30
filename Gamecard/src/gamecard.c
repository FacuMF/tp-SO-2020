#include "gamecard.h"

void main(){
	// Leer config
	config = leer_config("./Gamecard/config/gamecard.config");
	set_base_path(config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));
	int PUERTO_BROKER = config_get_int_value(config, "PUERTO_BROKER");
	int REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	int REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");

	// Suscribirse al broker: NEW_POKEMON, CATCH_POKEMON, GET_POKEMON

	// Esperar a recibir mensajes.

	// Al recibir: 	Informar la recepción del mismo (ACK)
	//				Crear hilo que atienda solicitud
	//				Volver al estado escucha de la MQ

	// Todo archivo tiene un valor "OPEN" en su metadata
	pthread_t thread_socket, thread_dump;
	pthread_create(&thread_socket, NULL, (void*) listen, PUERTO_BROKER);

	//Al recibir NEW_POKEMON

	if(file_open()){
		// Reintentar la operación luego de REINTENTO_OPERACION
	} else {

	}

}


