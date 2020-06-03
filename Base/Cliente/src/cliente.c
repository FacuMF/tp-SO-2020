#include "cliente.h"

int main(void) {
	// Iniciar variables
	int conexion;
	char* ip;
	char* puerto;
	char * mensaje_a_enviar = "ESTE ES EL MENSAJE XD";

	// Leer Config
	config = leer_config("./Base/Cliente/config/cliente.config");
	string_nivel_log_minimo = config_get_string_value(config,
			"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);

	// Iniciar Logger
	logger = iniciar_logger("./Base/Cliente/config/cliente.log", "Cliente",
			log_nivel_minimo);
	log_trace(logger, "Primer log ingresado");

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Iniciar Conexion
	conexion = iniciar_conexion(ip, puerto);
	log_trace(logger, "Conexion Creada. IP: %s y PUERTO: %s ", ip, puerto);

	// Enviar mensaje
	//Prueba mjeTexto
	//t_msjTexto* mensaje_test = crear_mensaje(mensaje_a_enviar);
	//t_buffer *buffer = serializar_mensaje(mensaje_test);

	//Prueba new pokemon
	/*t_new_pokemon* mensaje_new = crear_new_pokemon("PIKACHU", 3, 2, 5, 1);
	 t_buffer* buffer_new = serializar_new_pokemon(mensaje_new);*/

	//Prueba appeared pokemon
	/*t_appeared_pokemon* mensaje_appeared = crear_appeared_pokemon("PIKACHU", 3, 2, 1);
	 t_buffer* buffer_appeared = serializar_appeared_pokemon(mensaje_appeared);*/

	//Prueba catch pokemon
	/*t_catch_pokemon* mensaje_catch = crear_catch_pokemon("PIKACHU", 3, 2, 5);
	 t_buffer* buffer_catch = serializar_catch_pokemon(mensaje_catch);*/

	//Prueba caugth pokemon
	/*t_caught_pokemon* mensaje_caught = crear_caught_pokemon(1, 0);
	 t_buffer* buffer_caught = serializar_caught_pokemon(mensaje_caught);*/

	//Prueba get pokemon
	/*t_get_pokemon *mensaje_get = crear_get_pokemon("Pikachu", 5);
	 t_buffer* buffer_get = serializar_get_pokemon(mensaje_get);*/

	//Prueba suscripcion
	t_subscriptor* mensaje_suscriptor = crear_suscripcion(NEW_POKEMON, 10);
	t_buffer* buffer_suscriptor = serializar_suscripcion(mensaje_suscriptor);
	log_trace(logger, "Mensaje Creado");

	//enviar_mensaje(conexion, buffer_new, NEW_POKEMON);
	//enviar_mensaje(conexion, buffer_appeared, APPEARED_POKEMON);
	//enviar_mensaje(conexion, buffer_catch, CATCH_POKEMON);
	//enviar_mensaje(conexion, buffer_caught, CAUGHT_POKEMON);
	//enviar_mensaje(conexion, buffer_get, GET_POKEMON);
	enviar_mensaje(conexion, buffer_suscriptor, SUSCRIPTOR);
	log_trace(logger, "Mensaje Serializado");
	log_trace(logger, "Mensaje Enviado");

	//TBR Recibir mensaje
	//Lo que hace es quedarse esperando que el server le envie un mensaje. Solo sabe recibir el mensaje SUBSCRIPCION. Es para hacer pruebas.
	recibir_mensaje_del_server(&conexion);

	//Finalizar mensaje
	terminar_programa(conexion, logger, config);

	return 0;
}

void handler_mensaje_texto(t_buffer* buffer_recepcion) {
	t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer_recepcion);
	log_trace(logger, "Mensaje Deserializado");
	log_trace(logger, mensaje_recibido->contenido);
}

void terminar_programa(int conexion, t_log* logger, t_config* config) {
	terminar_logger(logger);

	if (config != NULL) {
		config_destroy(config);
	}
	close(conexion);
}

//TBR 
int recibir_mensaje_del_server(int* socket) {
	log_trace(logger, "Recibir mensaje del server...");
	int cod_op = recibir_codigo_operacion(*socket);
	process_request(cod_op, *socket);
}

//TBR   
void process_request(int cod_op, int cliente_fd) {
	log_trace(logger, "Cod_op recibido, recibir mensaje del server...");
	int size;
	t_buffer * buffer;
	int valor;
	char* mensaje;
	log_trace(logger, "Codigo de operacion: %d", cod_op);
	switch (cod_op) {
	case SUSCRIPTOR:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_subscriptor* mensaje_suscriptor = deserializar_suscripcion(buffer);
		log_trace(logger, "Mensaje de suscripcion recibido");
		log_trace(logger,
				"Cola de mensajes: %d, tiempo de suscripcion: %d segundos",
				mensaje_suscriptor->cola_de_mensaje,
				mensaje_suscriptor->tiempo);
		free(mensaje_suscriptor);
		free(buffer);
		break;

	case 0:
		log_trace(logger, "Codigo invalido");
		pthread_exit(NULL);
		break;
	case -1:
		pthread_exit(NULL);
		break;
	}
}
