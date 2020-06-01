#include "gamecard.h"

void main(){
	// Leer config
	config = leer_config("./Gamecard/config/gamecard.config");
	set_base_path(config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));
	char* PUERTO_BROKER = config_get_int_value(config, "PUERTO_BROKER");
	char* IP_BROKER = config_get_int_value(config, "IP_BROKER");
	int REINTENTO_CONEXION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION");
	int REINTENTO_OPERACION = config_get_int_value(config, "TIEMPO_DE_REINTENTO_OPERACION");
	// Conexion socket
	int conexion = iniciar_conexion(IP_BROKER, PUERTO_BROKER);

	// Suscribirse al broker: NEW_POKEMON, CATCH_POKEMON, GET_POKEMON
	suscribirse_a(&conexion,"NEW_POKEMON" ); // NEW_POKEMON
	suscribirse_a(&conexion,"CATCH_POKEMON" ); // CATCH_POKEMON
	suscribirse_a(&conexion,"GET_POKEMON" ); // GET_POKEMON
	// "NEW_POKEMON" "CAUGHT_POKEMON" "GET_POKEMON" deben ser int
	// Esperar a recibir mensajes. Reintentar cada REINTENTO_CONEXION si no se logra conectar
	lanzar_hilo_receptor_mensajes(); // Chequear función

	// Al recibir: 	Informar la recepción del mismo (ACK)
	//				Crear hilo que atienda solicitud
	//				Volver al estado escucha de la MQ

	/*
	pthread_t thread_socket;
	pthread_create(&thread_socket, NULL, (void*) listen, PUERTO_BROKER);
	*/

	//Al recibir NEW_POKEMON
	recibir_mensajes_gamecard();
	if(file_open()){
		// Reintentar la operación luego de REINTENTO_OPERACION
	} else {

	}

}


