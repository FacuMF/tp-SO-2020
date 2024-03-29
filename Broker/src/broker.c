#include "broker.h"

int main(void) {
	signal(SIGUSR1, handler_senial);
	inicializacion_broker();
	esperar_mensajes();
	terminar_proceso();

	return 0;
}

void handler_senial(int signum) {
	log_trace(logger,"Guardardo datos de cache...");
	estado_actual_de_cache();
	exit(1);

}

void inicializacion_broker(void) {
	config = leer_config("./Broker/config/broker.config");


	string_nivel_log_minimo = config_get_string_value(config,
				"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
	logger = iniciar_logger("./Broker/config/broker.log", "Broker", log_nivel_minimo );

	inicializacion_colas();

	inicializacion_ids();

	inicializacion_cache();

	inicializacion_semaforos();

	log_trace(logger, "Inicialiazacion terminada.");
}

void terminar_proceso(void) {
	pthread_join(tid[0], NULL);
	terminar_logger(logger);
	config_destroy(config);

	pthread_mutex_destroy(&mutex_memoria_cache);
	pthread_mutex_destroy(&mutex_id_mensaje);
	pthread_mutex_destroy(&mutex_lru_flag);
	pthread_mutex_destroy(&mutex_suscribir);
}

void inicializacion_colas(void) {
	new_pokemon =  list_create();
	appeared_pokemon = list_create();
	catch_pokemon = list_create();
	caught_pokemon = list_create();
	get_pokemon = list_create();
	localized_pokemon = list_create();
}

void inicializacion_ids(void) {
	id_mensajes = 0;
}

void inicializacion_semaforos(){
	pthread_mutex_init(&mutex_memoria_cache, NULL);
	pthread_mutex_init(&mutex_id_mensaje, NULL);
	pthread_mutex_init(&mutex_lru_flag, NULL);
	pthread_mutex_init(&mutex_suscribir, NULL);
	pthread_mutex_init(&mutex_hilos, NULL);
}

void* esperar_mensajes() {
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
	log_trace(logger, "Conexion del socket %i al Broker.", socket_cliente);

	int* argument = malloc(sizeof(int));
	*argument = socket_cliente;
	pthread_mutex_lock(&mutex_hilos);
	pthread_create(&thread, NULL, (void*) recibir_mensaje_del_cliente,
			argument);
	pthread_detach(thread);
	pthread_mutex_unlock(&mutex_hilos);
	// Si termina el hilo, que sus recursos se liberen automaticamente

}

void recibir_mensaje_del_cliente(void* input) {
	int socket_cliente = *((int *) input);
	int cod_op = 0;
	free(input);

	while (cod_op >= 0 && cod_op<10) { //Se tiene que repetir para que un socket pueda enviar mas de un mensaje.

		cod_op = recibir_codigo_operacion(socket_cliente);
		log_trace(logger, "Recibi cod op %d.", cod_op);

		if (cod_op == -1){
			log_error(logger, "Error en 'recibir_codigo_operacion'");
		}

		if (cod_op >= 0 && cod_op<10)
			handle_mensaje(cod_op, socket_cliente);
		else{
			log_trace(logger, "El cliente %i cerro el socket.",
						socket_cliente);
		}
	}
	pthread_exit(NULL);

}

void handle_mensaje(int cod_op, int socket_cliente) { //Lanzar un hilo para manejar cada mensaje una vez deserializado?

	t_buffer * buffer = recibir_mensaje(socket_cliente);

	t_conexion_buffer * info_mensaje_a_manejar = malloc(
			sizeof(t_conexion_buffer));
	info_mensaje_a_manejar->conexion = socket_cliente;
	info_mensaje_a_manejar->buffer = buffer;

	char* tipo_mensaje = op_code_a_string(cod_op);
	log_trace(logger, "Se recibio un mensaje %s", tipo_mensaje);
	free(tipo_mensaje);

	switch (cod_op) {
	case SUSCRIPTOR:
		//pthread_create(&thread, NULL, (void*) manejar_mensaje_suscriptor,
						//info_mensaje_a_manejar);
		manejar_mensaje_suscriptor(info_mensaje_a_manejar);
		break;
	case APPEARED_POKEMON:
		pthread_mutex_lock(&mutex_hilos);
		pthread_create(&thread, NULL, (void*) manejar_mensaje_appeared,
				info_mensaje_a_manejar);
		pthread_mutex_unlock(&mutex_hilos);
		break;
	case NEW_POKEMON:
		pthread_mutex_lock(&mutex_hilos);
		pthread_create(&thread, NULL, (void*) manejar_mensaje_new,
				info_mensaje_a_manejar);
		pthread_mutex_unlock(&mutex_hilos);
		break;
	case CATCH_POKEMON:
		pthread_mutex_lock(&mutex_hilos);
		pthread_create(&thread, NULL, (void*) manejar_mensaje_catch,
				info_mensaje_a_manejar);
		pthread_mutex_unlock(&mutex_hilos);
		break;
	case CAUGHT_POKEMON:
		pthread_mutex_lock(&mutex_hilos);
		pthread_create(&thread, NULL, (void*) manejar_mensaje_caught,
				info_mensaje_a_manejar);
		pthread_mutex_unlock(&mutex_hilos);
		break;
	case GET_POKEMON:
		pthread_mutex_lock(&mutex_hilos);
		pthread_create(&thread, NULL, (void*) manejar_mensaje_get,
				info_mensaje_a_manejar);
		pthread_mutex_unlock(&mutex_hilos);
		break;
	case LOCALIZED_POKEMON:
		pthread_mutex_lock(&mutex_hilos);
		pthread_create(&thread, NULL, (void*) manejar_mensaje_localized,
				info_mensaje_a_manejar);
		pthread_mutex_unlock(&mutex_hilos);
		break;
	case CONFIRMACION:
		//pthread_create(&thread, NULL, (void*) manejar_mensaje_confirmacion,
						//info_mensaje_a_manejar);
		manejar_mensaje_confirmacion(info_mensaje_a_manejar);
		break;
	default:
		log_trace(logger, "No se reconoce el codigo de operacion %d", cod_op);
		break;

	}

	pthread_detach(thread);
}

int get_id_mensajes(void) {
	pthread_mutex_lock(&mutex_id_mensaje);

	int id = id_mensajes;
	id_mensajes++;

	pthread_mutex_unlock(&mutex_id_mensaje);

	return id;
}
