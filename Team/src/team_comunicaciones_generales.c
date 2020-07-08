#include "team.h"

// INICIO COMUNICACION

int iniciar_conexion_con_broker() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");

	// Reintento
	int socket_broker = iniciar_conexion(ip_broker, puerto_broker);
	while (socket_broker < 0) {
		log_info(logger, "Broker dio %d, esperando para reintentar",
				socket_broker);
		socket_broker = iniciar_conexion(ip_broker, puerto_broker);
		sleep(config_get_int_value(config, "RETARDO_REINTENTO_BROKER"));
	}

	return socket_broker;
}

void iniciar_conexion_con_gameboy() {
	log_trace(logger, "Iniciar conexion con gameboy");
	char * ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	char * puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy, puerto_gameboy);

	while (1) {
		log_trace(logger, "Esperando Cliente");
		esperar_cliente(socket_gameboy);
	}
}

// ESPERAS COMUNICACION
void esperar_cliente(int socket_servidor) {	// Hilo coordinador

	int socket_cliente = aceptar_cliente(socket_servidor);
	log_trace(logger, "Cliente aceptado: %d.", socket_cliente);

	int * argument = malloc(sizeof(int));
	*argument = socket_cliente;
	pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);
}

void esperar_mensajes_cola(void* input) {
	int conexion = *((int *) input);
	int cod_op = 1;

	while (cod_op > 0) {
		cod_op = recibir_codigo_operacion(conexion);
		if (cod_op == -1) {
			log_error(logger, "Error en 'recibir_codigo_operacion'");
		} else if (cod_op >= 0)
			log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);

		if (cod_op >= 0)
			manejar_recibo_mensajes(conexion, cod_op, 0);
	}
}

// ENVIO DE MENSAJES

void enviar_requests_pokemones() { // RECONTRA LIMPIAR
	list_iterate(objetivo_global, enviar_mensaje_get);
}

void enviar_mensaje_get(void*element) {

	int socket_broker = iniciar_conexion_con_broker();
	// TODO: Si falla, comportamiento default, no reintentar

	t_objetivo * objetivo = element;
	t_get_pokemon * mensaje_get = crear_get_pokemon(objetivo->pokemon, -10);

	t_buffer* mensaje_serializado = serializar_get_pokemon(mensaje_get);

	enviar_mensaje(socket_broker, mensaje_serializado, GET_POKEMON);

	log_trace(logger, "Enviado get para: %s", objetivo->pokemon);

	free(mensaje_serializado);

	manejar_recibo_mensajes(socket_broker,
			recibir_codigo_operacion(socket_broker), 1);

	close(socket_broker);

}

void enviar_mensaje_catch(t_catch_pokemon * mensaje_catch_a_enviar) { //mismo que get
	int socket_broker = iniciar_conexion_con_broker();
	// TODO: Si falla, comportamiento default, no reintentar

	t_buffer*mensaje_catch_serializado = serializar_catch_pokemon(
			mensaje_catch_a_enviar);

	enviar_mensaje(socket_broker, mensaje_catch_serializado, CATCH_POKEMON);

	log_trace(logger, "Enviado catch para: %s",
			mensaje_catch_a_enviar->pokemon);

	free(mensaje_catch_serializado);

	manejar_recibo_mensajes(socket_broker,
			recibir_codigo_operacion(socket_broker), 1);

	close(socket_broker);
}

