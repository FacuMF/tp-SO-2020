#include "team.h"
// Inicio de conexion de ambos modulos
int iniciar_conexion_con_broker() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	//log_trace(logger, "Ip BROKER Leida : %s Puerto BROKER Leido : %s\n",ip_broker, puerto_broker);

	int conexion = iniciar_conexion(ip_broker, puerto_broker);

	return conexion;
}

void iniciar_conexion_con_gameboy() {
	char * ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	char * puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");
	//log_trace(logger, "Ip Gameboy Leida : %s Puerto Gameboy Leido : %s\n", ip_gameboy, puerto_gameboy);

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy, puerto_gameboy);

	while (1) {
		log_trace(logger, "Esperando Cliente");
		esperar_cliente(socket_gameboy);
	}
}

// Funciones comunicacion generales
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
	// Si no lo descarté y agregue algo a la lista de pokemones a atrapar, lanzar el hilo planificador
	// el hilo planificador va a chequear la lista y va a planificar los entrenadores depenendiendo del mensaje.

	log_trace(logger, "Mensaje recibido manejado.");
}
// Comunicacion con broker
void suscribirse_a_colas_necesarias() {
	enviar_suscripcion_broker(APPEARED_POKEMON);
	enviar_suscripcion_broker(LOCALIZED_POKEMON);
	enviar_suscripcion_broker(CAUGHT_POKEMON);
}

void enviar_suscripcion_broker(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_con_broker();

	while (socket_broker < 0) {
		log_info(logger, "Broker dio %d, esperando para reintentar",
				socket_broker);
		sleep(config_get_int_value(config, "RETARDO_REINTENTO_BROKER")); //TODO: Pasar a por config file
		socket_broker = iniciar_conexion_con_broker();
	}

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
void enviar_requests_pokemones(t_list *objetivo_global) { // RECONTRA LIMPIAR
	int socket_broker = iniciar_conexion_con_broker(); // mover a enviar mensaje

	void enviar_mensaje_get_aux(void *elemento) {
		enviar_mensaje_get(socket_broker, elemento);
	}

	list_iterate(objetivo_global, enviar_mensaje_get_aux);
}

void enviar_mensaje_get(int socket_broker, void*element) {
	t_objetivo * objetivo = element;
	t_get_pokemon * mensaje_get = crear_get_pokemon(objetivo->pokemon, -10);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje_get);
	enviar_mensaje(socket_broker, mensaje_serializado, GET_POKEMON);
	log_trace(logger, "Enviado get para: %s", objetivo->pokemon);
	free(mensaje_serializado);
	// espera rta con id
	//agrega id a lista de ids
	// cierra socket

}

void enviar_mensaje_catch(t_appeared_pokemon * mensaje_appeared_a_capturar) { //mismo que get
	t_catch_pokemon * mensaje_catch = crear_catch_pokemon(
			mensaje_appeared_a_capturar->pokemon,
			mensaje_appeared_a_capturar->posx,
			mensaje_appeared_a_capturar->posy, -20);
	t_buffer*mensaje_catch_serializado = malloc(sizeof(t_buffer));
	mensaje_catch_serializado = serializar_catch_pokemon(mensaje_catch);
	//enviar_mensaje(socket_broker(pasar a variable global),mensaje_catch,CATCH_POKEMON);
	log_trace(logger, "Enviado catch para: %s",
			mensaje_appeared_a_capturar->pokemon);
	free(mensaje_catch_serializado);
}

// Funciones de recepcion de mensajes
void recibir_mensaje_appeared(t_buffer * buffer) {
	char * message_data;
	t_appeared_pokemon * mensaje_appeared = deserializar_appeared_pokemon(
			buffer);
	message_data = mostrar_appeared_pokemon(mensaje_appeared);

	log_info(logger, "Se recibio un mensaje APPEARED_POKEMON, %s",
			message_data);

	handle_appeared_pokemon(mensaje_appeared);
}

void recibir_mensaje_caught(t_buffer * buffer) {
	char * message_data;
	t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(
			buffer);
	// handle_caught_pokemon(mensaje_caught_pokemon);
	message_data = mostrar_caught_pokemon(mensaje_caught_pokemon);
	log_info(logger, "Se recibio un mensaje CAUGHT_POKEMON, %s", message_data);

}

void recibir_mensaje_localized(t_buffer * buffer) {
	char * message_data;
	t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(
			buffer);
	// handle_localized_pokemon(mensaje_localized_pokemon);
	message_data = mostrar_localized(mensaje_localized_pokemon);
	log_info(logger, "Se recibio un mensaje LOCALIZED_POKEMON, %s",
			message_data);
}

void handle_appeared_pokemon(t_appeared_pokemon * mensaje_appeared) {
	/*
	 if (requiero_pokemon(mensaje_appeared)) {
	 //comenzar_planificacion_entrenadores(mensaje_appeared); // TODO Ejecutar hilo de planificacion?
	 } else {
	 log_trace(logger, "Mensaje appeared se desechara, no es requerido");
	 }
	 */
}

