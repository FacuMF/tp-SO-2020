#include "broker.h"

int main(void) {
	inicializacion_broker();
	lanzar_hilo_receptor_mensajes();
	terminar_proceso();

	return 0;
}

void inicializacion_broker(void){
    // Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log", "Broker", LOG_LEVEL_TRACE);
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
	id_mensajes = 0;
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

	log_trace(logger, "Conexion de %i al Broker.", socket_cliente);

	log_trace(logger, "Va a lanzar hilo 'recibir_mensaje_del_cliente'.");


	int* argument = malloc(sizeof(int));
	*argument = socket_cliente;

	pthread_create(&thread, NULL, (void*) recibir_mensaje_del_cliente, argument);// Crea un thread que se quede atendiendo al cliente
	//pthread_detach(tid[1]);	// Si termina el hilo, que sus recursos se liberen automaticamente

}


 void recibir_mensaje_del_cliente(void* input) {
	 int socket_cliente = *((int *)input);
	 int cod_op = recibir_codigo_operacion(socket_cliente);
	 (cod_op == -1)? log_error(logger, "Error en 'recibir_codigo_operacion'") :
					 log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
	 handle_mensaje(cod_op, socket_cliente);
 }

 void handle_mensaje(int cod_op, int socket_cliente){
	t_buffer * buffer;
	int id_mensaje_recibido;

	switch (cod_op) {
		case SUSCRIPTOR:

			log_trace(logger, "Se recibio un mensaje SUSCRIPTOR");
			buffer = recibir_mensaje(socket_cliente);
			t_subscriptor* subscripcion = deserializar_suscripcion(buffer);
			log_trace(logger, "Mensaje SUSCRIPTOR recibido.");

			log_info(logger,"Suscripcion de %i a la cola %i.", socket_cliente, subscripcion->cola_de_mensaje);

			subscribir(socket_cliente, subscripcion);

			//enviar_mensajes_de_suscripcion_a_cliente(subscripcion, socket_cliente);
			//TODO se necesita tener los mensajes cacheados.

			//free (liberar memoria)

			break;

		case APPEARED_POKEMON:

			log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
			buffer = recibir_mensaje(socket_cliente);
			t_appeared_pokemon* mensaje_appeared_pokemon = deserializar_appeared_pokemon(buffer);

			id_mensaje_recibido = asignar_id_appeared_pokemon(mensaje_appeared_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola APPEARED_POKEON", id_mensaje_recibido);

			devolver_appeared_pokemon(socket_cliente ,mensaje_appeared_pokemon);
			log_trace(logger, "Se devolvio el mensaje APPEARED_POKEMON con id asignado.");

			almacenar_en_cola_appeared_pokemon(mensaje_appeared_pokemon);
			log_trace(logger, "Se almaceno el mensaje APPEARED_POKEMON en la cola.");

			//cachear_appeared_pokemon(mensaje);

			//free (liberar memoria)

			break;
		case NEW_POKEMON:
			log_trace(logger, "Se recibio un mensaje NEW_POKEMON");
			buffer = recibir_mensaje(socket_cliente);
			t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);

			id_mensaje_recibido = asignar_id_new_pokemon(mensaje_new_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola NEW_POKEON", id_mensaje_recibido);

			devolver_new_pokemon(socket_cliente ,mensaje_new_pokemon);
			log_trace(logger, "Se devolvio el mensaje NEW_POKEMON con id asignado.");

			almacenar_en_cola_new_pokemon(mensaje_new_pokemon);
			log_trace(logger, "Se almaceno el mensaje NEW_POKEMON en la cola.");

			//cachear_new_pokemon(mensaje);

			//free (liberar memoria)

			break;
		case CATCH_POKEMON:
			log_trace(logger, "Se recibio un mensaje CATCH_POKEMON");
			buffer = recibir_mensaje(socket_cliente);
			t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(buffer);

			id_mensaje_recibido = asignar_id_catch_pokemon(mensaje_catch_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola CATCH_POKEMON", id_mensaje_recibido);

			devolver_catch_pokemon(socket_cliente ,mensaje_catch_pokemon);
			log_trace(logger, "Se devolvio el mensaje CATCH_POKEMON con id asignado.");

			almacenar_en_cola_catch_pokemon(mensaje_catch_pokemon);
			log_trace(logger, "Se almaceno el mensaje CATCH_POKEMON en la cola.");

			//cachear_catch_pokemon(mensaje);

			//free (liberar memoria)


			break;
		case CAUGHT_POKEMON:
			log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");
			buffer = recibir_mensaje(socket_cliente);
			t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);

			id_mensaje_recibido = asignar_id_caught_pokemon(mensaje_caught_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola CAUGTH_POKEON", id_mensaje_recibido);

			devolver_caught_pokemon(socket_cliente ,mensaje_caught_pokemon);
			log_trace(logger, "Se devolvio el mensaje CAUGTH_POKEMON con id asignado.");

			almacenar_en_cola_caught_pokemon(mensaje_caught_pokemon);
			log_trace(logger, "Se almaceno el mensaje CAUGTH_POKEMON en la cola.");

			//cachear_caught_pokemon(mensaje);

			//free (liberar memoria)

			break;
		case GET_POKEMON:
			log_trace(logger, "Se recibio un mensaje GET_POKEMON");
			buffer = recibir_mensaje(socket_cliente);
			t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);

			id_mensaje_recibido = asignar_id_get_pokemon(mensaje_get_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola GET_POKEON", id_mensaje_recibido);

			devolver_get_pokemon(socket_cliente ,mensaje_get_pokemon);
			log_trace(logger, "Se devolvio el mensaje GET_POKEMON con id asignado.");

			almacenar_en_cola_get_pokemon(mensaje_get_pokemon);
			log_trace(logger, "Se almaceno el mensaje GET_POKEMON en la cola.");

			//cachear_get_pokemon(mensaje);

			//free (liberar memoria)

			break;
		case LOCALIZED_POKEMON:
			log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");
			buffer = recibir_mensaje(socket_cliente);
			t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);

			id_mensaje_recibido = asignar_id_localized_pokemon(mensaje_localized_pokemon);

			log_info(logger, "Llegada de mensaje nuevo %i a cola APPEARED_POKEON", id_mensaje_recibido);

			devolver_localized_pokemon(socket_cliente ,mensaje_localized_pokemon);
			log_trace(logger, "Se devolvio el mensaje localized_pokemon con id asignado.");

			almacenar_en_cola_localized_pokemon(mensaje_localized_pokemon);
			log_trace(logger, "Se almaceno el mensaje localized_pokemon en la cola.");

			//cachear_localized_pokemon(mensaje);

			//free (liberar memoria)

			break;
		case CONFIRMACION:
			log_trace(logger, "Se recibio una CONFIRMACION.");
			buffer = recibir_mensaje(socket_cliente);
			t_confirmacion* mensaje_confirmacion = deserializar_confirmacion(buffer);

			confirmar_cliente_recibio(mensaje_confirmacion, socket_cliente);

			//free(mensaje_confirmacion);
			//free(buffer);

			break;
 	}
 }

void enviar_mensaje_de_cola(void* mensaje, int ciente){

}

int get_id_mensajes(void) {
	 //TODO MUTEX
	 int id = id_mensajes;
	 id_mensajes++;
	 return id;
}
