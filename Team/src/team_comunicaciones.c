#include "team.h"
// Inicio de conexion de ambos modulos
int iniciar_conexion_con_broker() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	log_trace(logger, "Ip BROKER Leida : %s Puerto BROKER Leido : %s\n",
			ip_broker, puerto_broker);

	int conexion = iniciar_conexion(ip_broker, puerto_broker);

	return conexion;
}

void iniciar_conexion_con_gameboy() {
	char * ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	char * puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");
	log_trace(logger, "Ip Gameboy Leida : %s Puerto Gameboy Leido : %s\n",
			ip_gameboy, puerto_gameboy);

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy, puerto_gameboy);

	while (1) {
		log_trace(logger, "Esperando Cliente");
		esperar_cliente(socket_gameboy); //Queda esperando que un cliente se conecte
	}
}

// Funciones comunicacion generales
void esperar_cliente(int socket_servidor) {	// Hilo coordinador
	struct sockaddr_in dir_cliente;	//contiene address de la comunicacion

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);// Acepta el request del cliente y crea el socket
	log_trace(logger, "Conexion de %i al Broker.", socket_cliente);

	int * argument = malloc(sizeof(int));
	*argument = socket_cliente;
	pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);
}
void esperar_mensajes_cola(void* input) {
	int conexion = *((int *) input);
	int cod_op = 1;

	log_trace(logger, "Esperando que aparezcan mensajes en %d", conexion);

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
		pthread_create(&thread, NULL, (void*) recibir_mensaje_appeared,buffer_recibido);
		break;
	case CAUGHT_POKEMON:
		pthread_create(&thread, NULL, (void*) recibir_mensaje_caught,buffer_recibido);
		break;
	case LOCALIZED_POKEMON:
		pthread_create(&thread, NULL, (void*) recibir_mensaje_localized,buffer_recibido);
		break;
	default:
		log_error(logger, "Opcode inválido.");
		break;
	}

	log_trace(logger, "Mensaje recibido manejado.");
}
// Funciones de comunicacion con Broker particularmente
void suscribirse_a_colas_necesarias() {
	enviar_suscripcion_broker(APPEARED_POKEMON);
	enviar_suscripcion_broker(LOCALIZED_POKEMON);
	enviar_suscripcion_broker(CAUGHT_POKEMON);
}
void enviar_suscripcion_broker(op_code tipo_mensaje) {

	int socket_broker = iniciar_conexion_con_broker();
	enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);

	log_trace(logger, "socket a esperar %d", socket_broker);

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
	int socket_broker = iniciar_conexion_con_broker();

	void enviar_mensaje_get_aux(void *elemento) { // USO INNER FUNCTIONS TODO: pasar a readme
		enviar_mensaje_get(socket_broker, elemento);
	}

	list_iterate(objetivo_global, enviar_mensaje_get_aux);
}

void enviar_mensaje_get(int socket_broker, void*element) {
	t_objetivo *objetivo = element;
	t_get_pokemon* mensaje_get = crear_get_pokemon(objetivo->pokemon, -10);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje_get);
	enviar_mensaje(socket_broker, mensaje_serializado, GET_POKEMON);
	log_trace(logger, "Enviado get para: %s", objetivo->pokemon);

}
// Funciones de recepcion de mensajes
void recibir_mensaje_appeared(t_buffer * buffer){
	char * message_data;
	t_appeared_pokemon * mensaje_appeared = deserializar_appeared_pokemon(buffer);
	// handle_appeared_pokemon(mensaje_appeared);
	message_data = mostrar_appeared_pokemon(mensaje_appeared);
	log_info(logger, "Se recibio un mensaje APPEARED_POKEMON, %s", message_data);
}
void recibir_mensaje_caught(t_buffer * buffer){
	char * message_data;
	t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);
	// handle_caught_pokemon(mensaje_caught_pokemon);
	message_data = mostrar_caught_pokemon(mensaje_caught_pokemon);
	log_info(logger, "Se recibio un mensaje CAUGHT_POKEMON, %s", message_data);

}
void recibir_mensaje_localized(t_buffer * buffer){
	char * message_data;
	t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);
	// handle_localized_pokemon(mensaje_localized_pokemon);
	message_data = mostrar_localized(mensaje_localized_pokemon);
	log_info(logger, "Se recibio un mensaje LOCALIZED_POKEMON, %s", message_data);
}
