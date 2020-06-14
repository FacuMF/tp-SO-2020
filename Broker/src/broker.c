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
	int socket_servidor = iniciar_conexion_servidor(ip, puerto);

	while (1) {
		log_trace(logger, "Va a ejecutar 'handle_cliente'.");
		handle_cliente(socket_servidor);
	}

	return 0;
}

void handle_cliente(int socket_servidor) {

	log_trace(logger, "Aceptando cliente...");
	int socket_cliente = aceptar_cliente(socket_servidor);

	log_trace(logger, "Conexion de %i al Broker.", socket_cliente);

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
		pthread_create(&thread, NULL, (void*) manejar_mensaje_new,info_mensaje_a_manejar);

		break;
	case CATCH_POKEMON:

		log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");
		pthread_create(&thread, NULL, (void*) manejar_mensaje_catch,info_mensaje_a_manejar);

		break;
	case CAUGHT_POKEMON:
		log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");
		pthread_create(&thread, NULL, (void*) manejar_mensaje_caught,info_mensaje_a_manejar);

		break;
	case GET_POKEMON:
		log_trace(logger, "Se recibio un mensaje GET_POKEMON");
		pthread_create(&thread, NULL, (void*) manejar_mensaje_get,info_mensaje_a_manejar);


		break;
	case LOCALIZED_POKEMON:
		log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");
		pthread_create(&thread, NULL, (void*) manejar_mensaje_localized,info_mensaje_a_manejar);

		break;
	case CONFIRMACION:
		log_trace(logger, "Se recibio una CONFIRMACION.");
		manejar_mensaje_confirmacion(info_mensaje_a_manejar);

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
