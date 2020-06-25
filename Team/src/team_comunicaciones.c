#include "team.h"

// INICIO COMUNICACION

int iniciar_conexion_con_broker() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");

	// Reintento
	int socket_broker = iniciar_conexion(ip_broker, puerto_broker);
	while (socket_broker < 0) {
		log_info(logger, "Broker dio %d, esperando para reintentar", socket_broker);
		socket_broker = iniciar_conexion(ip_broker, puerto_broker);
		sleep(config_get_int_value(config, "RETARDO_REINTENTO_BROKER"));
	}

	return socket_broker;
}

void iniciar_conexion_con_gameboy() {
	char * ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	char * puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy, puerto_gameboy);

	while (1) {
		log_trace(logger, "Esperando Cliente");
		esperar_cliente(socket_gameboy);
	}
}

// COMUNICACIONES GENERALES
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

	while (cod_op >= 0) {
		cod_op = recibir_codigo_operacion(conexion);
		(cod_op == -1) ?
				log_error(logger, "Error en 'recibir_codigo_operacion'") :
				log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);

		if (cod_op >= 0)
			manejar_recibo_mensajes(conexion, cod_op);
	}
}

// RECEPCION MENSAJES

void manejar_recibo_mensajes(int conexion, op_code cod_op) {  //TODO: pending
	t_buffer * buffer_recibido = recibir_mensaje(conexion);
	switch (cod_op) {
	case APPEARED_POKEMON:
		pthread_create(&thread, NULL, (void*) recibir_mensaje_appeared,
				buffer_recibido);
		break;
	case CAUGHT_POKEMON:
		pthread_create(&thread, NULL, (void*) recibir_mensaje_caught,
				buffer_recibido);
		// Esperar a que termine de recibir, no hace falta hilo
		break;
	case LOCALIZED_POKEMON:
		pthread_create(&thread, NULL, (void*) recibir_mensaje_localized,
				buffer_recibido);
		break;
	default:
		log_error(logger, "Opcode inválido.");
		break;
	}

	// Responder que recibi el mensaje
	// Agregar a lista planif
	// Aviso de nuevo msj a planif para q planif

	log_trace(logger, "Mensaje recibido manejado.");
}


void recibir_mensaje_appeared(t_buffer * buffer) {
	t_appeared_pokemon * mensaje_appeared = deserializar_appeared_pokemon(buffer);
	log_info(logger, "Mensaje APPEARED_POKEMON: %s",mostrar_appeared_pokemon(mensaje_appeared));
}

void recibir_mensaje_caught(t_buffer * buffer) {
	t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);
	log_info(logger, "Mensaje CAUGHT_POKEMON: %s", mostrar_caught_pokemon(mensaje_caught_pokemon));
}

void recibir_mensaje_localized(t_buffer * buffer) {
	t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);
	log_info(logger, "Mensaje LOCALIZED_POKEMON: %s",mostrar_localized(mensaje_localized_pokemon));
}

// SUSCRIPCION
void suscribirse_a_colas_necesarias() {
	enviar_suscripcion_broker(APPEARED_POKEMON);
	enviar_suscripcion_broker(LOCALIZED_POKEMON);
	enviar_suscripcion_broker(CAUGHT_POKEMON);
}

void enviar_suscripcion_broker(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_con_broker();

	enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);

	int* argument = malloc(sizeof(int));
	*argument = socket_broker;
	pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);

	log_trace(logger, "Suscripcion completada");
}

void enviar_mensaje_suscripcion(op_code mensaje, int conexion) {
	t_subscriptor* mensaje_suscripcion;
	mensaje_suscripcion = crear_suscripcion(mensaje, -10);

	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

	enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
	log_trace(logger, "Mensaje suscripcion enviado");
}

// ENVIO DE MENSAJES

void enviar_requests_pokemones() { // RECONTRA LIMPIAR
	list_iterate(objetivo_global, enviar_mensaje_get);
}

void enviar_mensaje_get(void*element) {

	int socket_broker = iniciar_conexion_con_broker();

	t_objetivo * objetivo = element;
	t_get_pokemon * mensaje_get = crear_get_pokemon(objetivo->pokemon, -10);

	t_buffer* mensaje_serializado = serializar_get_pokemon(mensaje_get);

	enviar_mensaje(socket_broker, mensaje_serializado, GET_POKEMON);

	log_trace(logger, "Enviado get para: %s", objetivo->pokemon);

	free(mensaje_serializado);

	//TODO: espera rta con id
	//agrega id a lista de ids
	// cierra socket

}

void enviar_mensaje_catch(t_appeared_pokemon * mensaje_appeared_a_capturar) { //mismo que get
	int socket_broker = iniciar_conexion_con_broker();

	t_catch_pokemon * mensaje_catch = crear_catch_pokemon(
			mensaje_appeared_a_capturar->pokemon,
			mensaje_appeared_a_capturar->posx,
			mensaje_appeared_a_capturar->posy, -20);

	t_buffer*mensaje_catch_serializado = serializar_catch_pokemon(mensaje_catch);

	enviar_mensaje(socket_broker,mensaje_catch,CATCH_POKEMON);

	log_trace(logger, "Enviado catch para: %s",mensaje_appeared_a_capturar->pokemon);
	free(mensaje_catch_serializado);

	//TODO: espera rta con id
	//agrega id a lista de ids
	// cierra socket
}

