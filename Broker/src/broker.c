#include "broker.h"

int main(void) {
	// Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log", "Team",
			LOG_LEVEL_TRACE);
	log_trace(logger, "Log inicializado");

	// Leer configuracion
	config = leer_config("./Broker/config/broker.config");
	log_trace(logger, "Config creada");

	// Inicializacion de las distintas colas de mensajes
	inicializacion_colas();

	// Lanzar hilo activador - Pasar a otra funcion.
	log_trace(logger, "Va a ejecutar hilo");
	int error;
	error = pthread_create(&(tid[0]), NULL, esperar_mensajes, NULL);
	if (error != 0) {
		log_error(logger, "Error al crear hilo Esperar_Mensajes");
		return error;
	}

	pthread_join(tid[0], NULL);

	terminar_logger(logger);
	config_destroy(config);
}


 // Funciones especificas por mensaje
 // SUSCRIPTOR



 // APPEARED_POKEMON
