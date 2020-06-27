#include "broker.h"

int main(void) {
	inicializacion_broker();
	//test(); //TODO borrar
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

	//Inicializacion IDs
	inicializacion_ids();

	//Inicializacion cache;
	inicializacion_cache();

	log_trace(logger, "Inicialiazacion terminada.");
}

void terminar_proceso(void) {
	pthread_join(tid[0], NULL);
	terminar_logger(logger);
	config_destroy(config);

	//pthread_mutex_destroy(mutex_memoria_cache);
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

void inicializacion_ids(void) {
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

	/*while (cod_op>=0) { //Se tiene que repetir para que un socket pueda enviar mas de un mensaje.

		cod_op = recibir_codigo_operacion(socket_cliente);
		if (cod_op == -1) log_error(logger, "Error en 'recibir_codigo_operacion'");

		(cod_op>=0)? handle_mensaje(cod_op, socket_cliente):
				     log_warning(logger, "El cliente %i cerro el socket.", socket_cliente);
	}*/ //TODO: Ver si alguien lo necesita, si no se borra.

	cod_op = recibir_codigo_operacion(socket_cliente);
	handle_mensaje(cod_op, socket_cliente);
}

void handle_mensaje(int cod_op, int socket_cliente) { //Lanzar un hilo para manejar cada mensaje una vez deserializado?

	t_buffer * buffer= recibir_mensaje(socket_cliente);

	t_conexion_buffer * info_mensaje_a_manejar = malloc (sizeof(t_conexion_buffer));
	info_mensaje_a_manejar->conexion = socket_cliente;
	info_mensaje_a_manejar->buffer = buffer;

	log_trace(logger, "Se recibio un mensaje %s", op_code_a_string(cod_op));

	switch (cod_op) {
	case SUSCRIPTOR:

		manejar_mensaje_suscriptor(info_mensaje_a_manejar);
		break;

	case APPEARED_POKEMON:

		//pthread_create(&thread, NULL, (void*) manejar_mensaje_appeared,info_mensaje_a_manejar);
		manejar_mensaje_appeared(info_mensaje_a_manejar);
		//pthread_detach(thread);
		break;

	case NEW_POKEMON:

		pthread_create(&thread, NULL, (void*) manejar_mensaje_new,info_mensaje_a_manejar);
		break;

	case CATCH_POKEMON:

		pthread_create(&thread, NULL, (void*) manejar_mensaje_catch,info_mensaje_a_manejar);
		break;

	case CAUGHT_POKEMON:

		pthread_create(&thread, NULL, (void*) manejar_mensaje_caught,info_mensaje_a_manejar);
		break;

	case GET_POKEMON:

		pthread_create(&thread, NULL, (void*) manejar_mensaje_get,info_mensaje_a_manejar);
		break;
	case LOCALIZED_POKEMON:

		pthread_create(&thread, NULL, (void*) manejar_mensaje_localized,info_mensaje_a_manejar);
		break;
	case CONFIRMACION:

		manejar_mensaje_confirmacion(info_mensaje_a_manejar);
		break;
	default:

		log_trace(logger, "El cliente %i cerro el socket.", socket_cliente);
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
