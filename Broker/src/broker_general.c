#include "broker.h"

void inicializacion_broker(void){
    // Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log", "Team", LOG_LEVEL_TRACE);
	// Leer configuracion
	config = leer_config("./Broker/config/broker.config");
	// Inicializacion de las distintas colas de mensajes
	inicializacion_colas();
	log_trace(logger, "Log, Config y Colas inicializadas.");
	//Inicializacion IDs
	inizializacion_ids();
}

void lanzar_hilo_receptor_mensajes(void){
    // Lanzar hilo activador
	log_trace(logger, "Va a ejecutar hilo 'esperar_mensaje'.");
	int error;
	error = pthread_create(&(tid[0]), NULL, esperar_mensajes, NULL);
	if (error != 0) {
		log_error(logger, "Error al crear hilo Esperar_Mensajes");
		// return error; // Es void, no puede devolver
	}
}

void terminar_proceso(void){
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

void inizializacion_ids(void){
	id_new_pokemon = 0;
	id_appeared_pokemon= 0;
	id_catch_pokemon= 0;
	id_caugth_pokemon= 0;
	id_get_pokemon= 0;
	id_localized_pokemon= 0;
}

void* esperar_mensajes(void *arg) {
	int i = 0; //El while va hasta 20 para evitar que entre en un loop infinito. Hay que pasarlo a while(true)
	while (i < 20) {
		i++;
		char* ip = config_get_string_value(config, "IP_BROKER");
		char* puerto = config_get_string_value(config, "PUERTO_BROKER");

		log_trace(logger, "Se va a ejecutar 'iniciar_conexion_con_el_modulo'.");
		iniciar_conexion_con_modulo(ip, puerto);
	}
}

void* iniciar_conexion_con_modulo(char* ip, char* puerto) {
	//Set up conexion
	struct addrinfo* servinfo = obtener_server_info(ip, puerto);
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor, servinfo);
	setear_socket_reusable(socket_servidor);
	freeaddrinfo(servinfo);

	log_trace(logger, "Va a ejeutar 'listen'.");
	listen(socket_servidor, SOMAXCONN);

	while(1){
		log_trace(logger, "Va a ejecutar 'handle_cliente'.");
		handle_cliente(socket_servidor);
	}

	return 0;
}

void handle_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	log_trace(logger, "Va a ejecutar 'accept'.");
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);


	log_trace(logger, "Va a lanzar hilo 'recibir_mensaje_del_cliente'.");

	recibir_mensaje_del_cliente(&socket_cliente);

	//TODO la variable thread es una sola, esto va a dar problemas, hay que hacer que sean varias de alguna forma. Finitas o infinitas?
	//pthread_create(&(tid[1]), NULL, (void*) recibir_mensaje_del_cliente, &socket_cliente);// Crea un thread que se quede atendiendo al cliente
	//pthread_detach(tid[1]);	// Si termina el hilo, que sus recursos se liberen automaticamente

}


 void recibir_mensaje_del_cliente(int* socket_cliente) {
 int cod_op = recibir_codigo_operacion(*socket_cliente);
 (cod_op == -1)? log_error(logger, "Error en 'recibir_codigo_operacion'") :
		 	 	 log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
 handle_mensaje(cod_op, *socket_cliente);
 }

 void handle_mensaje(int cod_op, int socket_cliente){
	t_buffer * buffer;

	switch (cod_op) {
		case SUSCRIPTOR:

		log_trace(logger, "Se recibio un mensaje SUSCRIPTOR");
		buffer = recibir_mensaje(socket_cliente);
		t_subscriptor* subscripcion = deserializar_suscripcion(buffer);
		log_trace(logger, "Mensaje SUSCRIPTOR recibido.");

		subscribir(socket_cliente, subscripcion);

		//enviar_mensajes_de_suscripcion_a_cliente(subscripcion, socket_cliente);
		//TODO se necesita tener los mensajes cacheados.

		break;

		case APPEARED_POKEMON:

		log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
        buffer = recibir_mensaje(socket_cliente);
		t_appeared_pokemon* mensaje_appeared_pokemon = deserializar_appeared_pokemon(buffer);

		int id_mensaje_recibido = asignar_id_appeared_pokemon(mensaje_appeared_pokemon);
		log_trace(logger, "ID asignado a APPEARED_POKEON: %i.", id_mensaje_recibido);

		devolver_appeared_pokemon(socket_cliente ,mensaje_appeared_pokemon);
		log_trace(logger, "Se devolvio el mensaje APPEARED_POKEMON con id asignado.");

		almacenar_en_cola_appeared_pokemon(mensaje_appeared_pokemon);
		log_trace(logger, "Se almaceno el mensaje APPEARED_POKEMON en la cola.");

		//cachear_appeared_pokemon(mensaje);

		break;
		case NEW_POKEMON:
		log_trace(logger, "Se recibio un mensaje NEW_POKEMON");

		break;
		case CATCH_POKEMON:
		log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");

		break;
		case CAUGHT_POKEMON:
		log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");

		break;
		case GET_POKEMON:
		log_trace(logger, "Se recibio un mensaje GET_POKEMON");

		break;
		case LOCALIZED_POKEMON:
		log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");

		break;
 	}
 }
 
void setear_socket_reusable(int socket) {
	int activado = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

void enviar_mensaje_de_cola(void* mensaje, int ciente){

}
