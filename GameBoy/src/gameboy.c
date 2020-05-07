#include "gameboy.h"

int main(void){

	//Inicio logger
	logger = iniciar_logger("./GameBoy/config/gameboy.log","GameBoy");
	log_info(logger,"Primer log ingresado");

	// Leer configuracion
	config =leer_config("./GameBoy/config/gameboy.config");
	log_info(logger,"Config creada");

	char* ip_broker= config_get_string_value(config, "IP_BROKER");
	char* puerto_broker=config_get_string_value(config, "PUERTO_BROKER");
	log_info(logger, "Leido de config: Broker ip: %s ,puerto: %s", ip_broker, puerto_broker);
	
	char* ip_team= config_get_string_value(config, "IP_TEAM");
	char* puerto_team=config_get_string_value(config, "PUERTO_TEAM");
	log_info(logger, "Leido de config: Team ip: %s ,puerto: %s", ip_team, puerto_team);

	char* ip_gamecard= config_get_string_value(config, "IP_GAMECARD");
	char* puerto_gamecard=config_get_string_value(config, "PUERTO_GAMECARD");
	log_info(logger, "Leido de config: GameBoy ip: %s ,puerto: %s", ip_gamecard, puerto_gamecard);

}
