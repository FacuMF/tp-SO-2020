/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: PokEbola
 */

#include "servidor.h"

int main(void) {
	char * ip;
	char * puerto;

	// Leer config
	config = leer_config("./Base/Utils/config/cliente_servidor.config");

	string_nivel_log_minimo = config_get_string_value(config, "LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);

	// Iniciar Logger
	logger = iniciar_logger("./Base/Utils/config/servidor.log", "Server",
			log_nivel_minimo);
	log_trace(logger, "Primer log ingresado");

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");


	// Inicar conexion
	iniciar_conexion_servidor(ip, puerto);

	// TBR Enviar mensaje al 2do mensaje
	// Recibe mensajes de 3 clientes, guarda los sockets y solo le responde al 2do. Esto significa que puedo elegir a que cliente responderle determinado mensaje. Es para hacer pruebas.
	log_trace(logger, "Enviar mensaje a cliente");
	t_subscriptor* mensaje_suscriptor = crear_suscripcion(NEW_POKEMON, 10);
	t_buffer* buffer_suscriptor = serializar_suscripcion(mensaje_suscriptor);
	enviar_mensaje(sockets_mensajes[1], buffer_suscriptor, SUSCRIPTOR);
	log_trace(logger, "Mensaje enviado");

	

	//Terminar
	finalizar_ejecucion(logger, config);

	return EXIT_SUCCESS;
}

void finalizar_ejecucion(t_log* logger, t_config *config) {
	terminar_logger(logger);
	config_destroy(config);
}

