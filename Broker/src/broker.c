#include "broker.h"

int main(void) {
	inicializacion_broker();
	esperar_mensajes(NULL);
	terminar_proceso();

	return 0;
}

void inicializacion_broker(void) {
	// Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log", "Broker",
			LOG_LEVEL_TRACE);

	// Leer configuracion
	config = leer_config("./Broker/config/broker.config");

	// Inicializacion de las distintas colas de mensajes
	inicializacion_colas();
	log_trace(logger, "Log, Config y Colas inicializadas.");

	//Inicializacion IDs
	inizializacion_ids();
}

void terminar_proceso(void) {
	pthread_join(tid[0], NULL);
	terminar_logger(logger);
	config_destroy(config);
}

void inicializacion_colas(void) {
	//  NEW_POKEMON
	new_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
	//  APPEARED_POKEMON
	appeared_pokemon = malloc(sizeof(t_queue));
	appeared_pokemon->subscriptores = list_create();
	appeared_pokemon->mensajes = list_create();
	//  CATCH_POKEMON
	catch_pokemon = malloc(sizeof(t_queue));
	catch_pokemon->subscriptores = list_create();
	catch_pokemon->mensajes = list_create();
	//  CAUGHT_POKEMON
	caught_pokemon = malloc(sizeof(t_queue));
	caught_pokemon->subscriptores = list_create();
	caught_pokemon->mensajes = list_create();
	//  GET_POKEMON
	get_pokemon = malloc(sizeof(t_queue));
	get_pokemon->subscriptores = list_create();
	get_pokemon->mensajes = list_create();
	//  LOCALIZED_POKEMON
	localized_pokemon = malloc(sizeof(t_queue));
	localized_pokemon->subscriptores = list_create();
	localized_pokemon->mensajes = list_create();
}

void inizializacion_ids(void) {
	id_mensajes = 0;
}

void* esperar_mensajes(void *arg) {
	char* ip = config_get_string_value(config, "IP_BROKER");
	char* puerto = config_get_string_value(config, "PUERTO_BROKER");

	log_trace(logger, "Se va a ejecutar 'iniciar_conexion_con_el_modulo'.");
	iniciar_conexion_con_modulo(ip, puerto);

	return 0;
}

void* iniciar_conexion_con_modulo(char* ip, char* puerto) {
	int socket_servidor = iniciar_conexion_servidor(ip, puerto);

	while (1) {
		log_trace(logger, "Va a ejecutar 'handle_cliente'.");
		handle_cliente(socket_servidor);
	}

	return 0;
}

void handle_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	log_trace(logger, "Va a ejecutar 'accept'.");
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);
	log_trace(logger, "Conexion de %i al Broker.", socket_cliente);

	log_trace(logger, "Va a lanzar hilo 'recibir_mensaje_del_cliente'.");

	int* argument = malloc(sizeof(int));
	*argument = socket_cliente;
	pthread_create(&thread, NULL, (void*) recibir_mensaje_del_cliente,argument);
	//pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente

}

void recibir_mensaje_del_cliente(void* input) {
	int socket_cliente = *((int *) input);
	int cod_op = 0;

	while (cod_op>=0) { //Se tiene que repetir para que un socket pueda enviar mas de un mensaje.

		cod_op = recibir_codigo_operacion(socket_cliente);
		if (cod_op == -1) log_error(logger, "Error en 'recibir_codigo_operacion'");

		t_info_mensaje* info_mensaje = malloc(sizeof(t_info_mensaje));
		info_mensaje->op_code = cod_op;
		info_mensaje->socket_cliente = socket_cliente;

		(cod_op>=0)? handle_mensaje(info_mensaje):
				     log_warning(logger, "El cliente %i cerro el socket.", socket_cliente);
	}

}

void handle_mensaje(void* stream) { //Lanzar un hilo para manejar cada mensaje una vez deserializado?
	// TODO: Sacar, no hace falta que sea stream, no es iun hilo
	t_info_mensaje* info_mensaje = stream;
	int cod_op = info_mensaje->op_code;
	int socket_cliente = info_mensaje->socket_cliente;

	t_buffer * buffer= recibir_mensaje(socket_cliente);

	// TODO: sacar, no hace falta
	int id_mensaje_recibido;

	// Nueva metodologia
	t_conexion_buffer * info_mensaje_a_manejar = malloc (sizeof(t_conexion_buffer));
	info_mensaje_a_manejar->conexion = socket_cliente;
	info_mensaje_a_manejar->buffer = buffer;


	switch (cod_op) {
	case SUSCRIPTOR:

		log_trace(logger, "Se recibio un mensaje SUSCRIPTOR");
		t_subscriptor* subscripcion = deserializar_suscripcion(buffer);

		log_info(logger, "Suscripcion de %i a la cola %i.", socket_cliente,subscripcion->cola_de_mensaje);

		subscribir(socket_cliente, subscripcion);

		//enviar_mensajes_de_suscripcion_a_cliente(subscripcion, socket_cliente);
		//TODO se necesita tener los mensajes cacheados.

		//free (liberar memoria)

		break;

	case APPEARED_POKEMON:

		log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
		pthread_create(&thread, NULL, (void*) manejar_mensaje_appeared,info_mensaje_a_manejar);

		break;
	case NEW_POKEMON:
		log_trace(logger, "Se recibio un mensaje NEW_POKEMON");

		t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);

		id_mensaje_recibido = asignar_id_new_pokemon(mensaje_new_pokemon);

		log_info(logger, "Llegada de mensaje nuevo %i a cola NEW_POKEON",
				id_mensaje_recibido);

		devolver_new_pokemon(socket_cliente, mensaje_new_pokemon);
		log_trace(logger,
				"Se devolvio el mensaje NEW_POKEMON con id asignado.");

		almacenar_en_cola_new_pokemon(mensaje_new_pokemon);
		log_trace(logger, "Se almaceno el mensaje NEW_POKEMON en la cola.");

		//cachear_new_pokemon(mensaje);

		//free (liberar memoria)

		break;
	case CATCH_POKEMON:
		log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");

		t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(
				buffer);

		id_mensaje_recibido = asignar_id_catch_pokemon(mensaje_catch_pokemon);

		log_info(logger, "Llegada de mensaje nuevo %i a cola CATCH_POKEMON",
				id_mensaje_recibido);

		devolver_catch_pokemon(socket_cliente, mensaje_catch_pokemon);
		log_trace(logger,
				"Se devolvio el mensaje CATCH_POKEMON con id asignado.");

		almacenar_en_cola_catch_pokemon(mensaje_catch_pokemon);
		log_trace(logger, "Se almaceno el mensaje CATCH_POKEMON en la cola.");

		//cachear_catch_pokemon(mensaje);

		//free (liberar memoria)

		break;
	case CAUGHT_POKEMON:
		log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");

		t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(
				buffer);

		id_mensaje_recibido = asignar_id_caught_pokemon(mensaje_caught_pokemon);

		log_info(logger, "Llegada de mensaje nuevo %i a cola CAUGTH_POKEON",
				id_mensaje_recibido);

		devolver_caught_pokemon(socket_cliente, mensaje_caught_pokemon);
		log_trace(logger,
				"Se devolvio el mensaje CAUGTH_POKEMON con id asignado.");

		almacenar_en_cola_caught_pokemon(mensaje_caught_pokemon);
		log_trace(logger, "Se almaceno el mensaje CAUGTH_POKEMON en la cola.");

		//cachear_caught_pokemon(mensaje);

		//free (liberar memoria)

		break;
	case GET_POKEMON:
		log_trace(logger, "Se recibio un mensaje GET_POKEMON");
		pthread_create(&thread, NULL, (void*) manejar_mensaje_get,info_mensaje_a_manejar);


		break;
	case LOCALIZED_POKEMON:
		log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");

		t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(
				buffer);

		id_mensaje_recibido = asignar_id_localized_pokemon(
				mensaje_localized_pokemon);

		log_info(logger, "Llegada de mensaje nuevo %i a cola APPEARED_POKEON",
				id_mensaje_recibido);

		devolver_localized_pokemon(socket_cliente, mensaje_localized_pokemon);
		log_trace(logger,
				"Se devolvio el mensaje localized_pokemon con id asignado.");

		almacenar_en_cola_localized_pokemon(mensaje_localized_pokemon);
		log_trace(logger,
				"Se almaceno el mensaje localized_pokemon en la cola.");

		//cachear_localized_pokemon(mensaje);

		//free (liberar memoria)

		break;
	case CONFIRMACION:
		log_trace(logger, "Se recibio una CONFIRMACION.");

		t_confirmacion* mensaje_confirmacion = deserializar_confirmacion(
				buffer);

		confirmar_cliente_recibio(mensaje_confirmacion, socket_cliente);

		//free(mensaje_confirmacion);
		//free(buffer);

		break;
	default:
		log_warning(logger, "El cliente %i cerro el socket.", socket_cliente);
		break;

	}

}

void enviar_mensaje_de_cola(void* mensaje, int ciente) {

}

int get_id_mensajes(void) {
	//TODO MUTEX
	int id = id_mensajes;
	id_mensajes++;
	return id;
}
