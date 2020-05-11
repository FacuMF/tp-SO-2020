#include "broker.h"

int main(void) {
	// Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log","Team");
	log_trace(logger,"--- Log inicializado ---");

	// Leer configuracion
	config =leer_config("./Broker/config/broker.config");
	log_info(logger,"Config creada");





	terminar_logger(logger);
	config_destroy(config);

}
