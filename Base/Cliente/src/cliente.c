#include "cliente.h"

int main(void) {
	// Iniciar variables
	int conexion;
	char* ip;
	char* puerto;
	char * mensaje_a_enviar = "ESTE ES EL MENSAJE XD";

	// Leer Config
	config = leer_config("./Base/Cliente/config/cliente.config");
	string_nivel_log_minimo = config_get_string_value(config,"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);

	// Iniciar Logger
	logger = iniciar_logger("./Base/Cliente/config/cliente.log", "Cliente", log_nivel_minimo);
	log_info(logger, "Primer log ingresado");

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Iniciar Conexion
	conexion = iniciar_conexion(ip, puerto);
	log_info(logger, "Conexion Creada. IP: %s y PUERTO: %s ", ip, puerto);

	// Enviar mensaje

	//Prueba new pokemon
	/*t_new_pokemon* mensaje_new = crear_new_pokemon("PIKACHU",3,2,5,1);
	 t_buffer* buffer_new = serializar_new_pokemon(mensaje_new);*/

	//Prueba mjeTexto
	//t_msjTexto* mensaje_test = crear_mensaje(mensaje_a_enviar);
	//t_buffer *buffer = serializar_mensaje(mensaje_test);
	//Prueba get pokemon
	/*t_get_pokemon *mensaje_get = crear_get_pokemon("Pikachu");
	 t_buffer* buffer_get = serializar_get_pokemon(mensaje_get);*/

	//Prueba catch pokemon
	/*t_catch_pokemon* mensaje_catch = crear_catch_pokemon("PIKACHU", 3, 2, 5);
	 t_buffer* buffer_catch = serializar_catch_pokemon(mensaje_catch);*/

	//Prueba caugth pokemon
	/*t_caugth_pokemon* mensaje_caugth = crear_caugth_pokemon(1, 0);
	 t_buffer* buffer_caugth = serializar_caught_pokemon(mensaje_caugth);*/

	//Prueba suscripcion
	t_subscriptor* mensaje_suscriptor = crear_suscripcion(NEW_POKEMON, 10);
	t_buffer* buffer_suscriptor = serializar_suscripcion(mensaje_suscriptor);
	log_info(logger, "Mensaje Creado");

	//enviar_mensaje(conexion, buffer_new, NEW_POKEMON);
	//enviar_mensaje(conexion,buffer_get,GET_POKEMON);
	//enviar_mensaje(conexion, buffer_catch, CATCH_POKEMON);
	//enviar_mensaje(conexion, buffer_caugth, CAUGHT_POKEMON);
	enviar_mensaje(conexion, buffer_suscriptor, SUSCRIPTOR);
	log_info(logger, "Mensaje Serializado");

	log_info(logger, "Mensaje Enviado");

	//Finalizar mensaje
	terminar_programa(conexion, logger, config);

	return 0;
}

void handler_mensaje_texto(t_buffer* buffer_recepcion) {
	t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer_recepcion);
	log_info(logger, "Mensaje Deserializado");
	log_info(logger, mensaje_recibido->contenido);
}

void terminar_programa(int conexion, t_log* logger, t_config* config) {
	terminar_logger(logger);

	if (config != NULL) {
		config_destroy(config);
	}
	close(conexion);
}
