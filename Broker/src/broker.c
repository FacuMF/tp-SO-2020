#include "broker.h"

int main(void) {
	// Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log","Team", LOG_LEVEL_ERROR);
	log_trace(logger,"--- Log inicializado ---");

	// Leer configuracion
	config =leer_config("./Broker/config/broker.config");
	log_trace(logger,"Config creada");

	// Inicializacion de las distintas colas de mensajes
		//  NEW_POKEMON
	t_queue* new_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  APPEARED_POKEMON
	t_queue* appeared_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  CATCH_POKEMON
	t_queue* catch_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  CAUGHT_POKEMON
	t_queue* caught_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  GET_POKEMON
	t_queue* get_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  LOCALIZED_POKEMON
	t_queue* localizad_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();

	int error;
	error = pthread_create(&(tid[0]), NULL, esperar_mensajes, NULL);
	if (error != 0){
		log_error(logger, "Error al crear hilo Esperar_Mensajes");
		return error;
	}

	/*
		if( Mensaje ){
			deserializar_mensjae()
			asignar_id()
			informar_id_a_cliente()
			almacenar_en_cola()
			enviar_a_todos_los_subs() \\Y esperar confirmacion
			cachear_mensaje()
		}
	}
	*/

	terminar_logger(logger);
	config_destroy(config);
}

void* esperar_mensajes(void *arg){
		while(1) {
			char* ip = config_get_string_value(config, "IP");
			char* puerto = config_get_string_value(config, "PUERTO");
			iniciar_conexion(ip, puerto);
		}
}

void iniciar_conexion(char* ip, char* puerto) {

	log_info(logger, "Servidor Inicializado");
	//Set up conexion
	struct addrinfo* servinfo = obtener_server_info(ip, puerto); 
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor, servinfo);
	setear_socket_reusable(socket_servidor);
	freeaddrinfo(servinfo);

	listen(socket_servidor, SOMAXCONN);	

	while (1)
		handle_cliente(socket_servidor);
}

void handle_cliente(int socket_servidor) {
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);

	//TODO la variable thread es una sola, esto va a dar problemas, hay que hacer que sean varias de alguna forma. Finitas o infinitas?
	pthread_create(&thread, NULL, (void*) recibir_mensaje_del_cliente, &socket_cliente);// Crea un thread que se quede atendiendo al cliente
	pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente
}

void recibir_mensaje_del_cliente(int* socket) {
	int cod_op = recibir_codigo_operacion(*socket);
	handle_mensaje(cod_op, *socket);
}

void handle_mensaje(int cod_op, int cliente_fd){
	t_buffer * buffer;
	
	switch (cod_op) {
	case SUSCRIPTOR:
		log_trace(logger, "Se recibio un mensaje SUSCRIPTOR");
		buffer = recibir_mensaje(cliente_fd);
		t_cliente cliente = deserializar_cliente(buffer);
		t_subscriptor subscripcion = deserializar_suscripcion(buffer);
		log_trace(logger, "Mensaje SUSCRIPTOR recibido.");

		suscribir(cliente, subscripcion);
		enviar_mensajes_de_cola(subscripcion);

		break;

	/*  PSEUDOCODIGO PARA TODOS LOS MENSAJES
		if( Mensaje ){
			deserializar_mensjae()
			asignar_id()
			informar_id_a_cliente()
			almacenar_en_cola()
			enviar_a_todos_los_subs() \\Y esperar confirmacion
			cachear_mensaje()
		}   */

	case APPEARED_POKEMON:
		log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
		buffer = recibir_mensaje(cliente_fd);
		t_cliente cliente = deserializar_cliente(buffer);
		buffer = buffer_sin_cliente(buffer);
		t_new_pokemon* mensaje_new_pokemon = deserializar_appeared_pokemon(buffer);

		char* id_mensaje_recibido = asignar_id_appeared_pokemon(mensaje_new_pokemon);
		informar_id_a_cliente(cliente ,id_mensaje_recibido);// Definir como pasarle este mensaje solo a este cliente
		almacenar_en_cola_appeared_pokemon(mensaje);
		cachear_appeared_pokemon(mensaje);

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

// Funciones Generales 

t_buffer buffer_sin_cliente(t_buffer buffer){

}


// Funciones especificas por mensaje
	// SUSCRIPTOR

void suscribir(t_cliente cliente, t_subscriptor subscripcion){
	//TODO
	return 0;
}

void enviar_mensajes_de_cola(t_subscriptor subscripcion){
	//TODO
	return 0;
}

	// APPEARED_POKEMON

char* asignar_id_appeared_pokemon(t_mensaje_appeared_pokemon mensaje){
	char* id;
	//TODO
	return id;
}
void informar_id_a_cliente(t_cliente cliente ,id_mensaje_recibido){
	//TODO
	return 0;
}
void almacenar_en_cola_appeared_pokemon(t_mensaje_appeared_pokemon mensaje){
	
	//TODO
	enviar_a_todos_los_subs_appeared_pokemon(mensaje);
	return 0;
}
void enviar_a_todos_los_subs_appeared_pokemon(mensaje){
	//TODO
	return 0;
}
void cachear_appeared_pokemon(t_mensaje_appeared_pokemon mensaje){
	//TODO
	return 0;
}



//  REQUERIMIENTOS 
	//  Administrar Subsciptores

		// Espera solicitudes de distintos modulos.

		// Lista de subscriptores por cada cola que administra.

		// Enviar a nuevos subscriptores, los mensajes cacheados



	//  Administrar recepcion, envio y confirmacion de mensajes

		//  Recibir mensajes

			//  Analizar a que cola pertenece.

			//  Identificar unoquibocamente el mensaje (ID)

			//  Almacenar en dicha cola.

			//  Cachear mensajes

	//  Enviar a todos los subscriptores

			//  Todo mensaje debe permanecer en la cola hasta que todos sus
			//  subs lo reciban

			//  Notificacion de recepcion: Todo mnesaje debe ser confirmado
			//  por cada subscriptor, para no volver a enviarlo al mismo.

			//  La recepcion y notificacion de mensajes puede diferir en
			//  el tiempo



	//  Mantener un registro de los ultimos mensajes recibidos para
	//  futuros subs

	//  Mantener e infomrar en todo momento los estados de las colas
	//  con sus mensajes y subscriptores.

		//  Mantener su estado

		//  Borrar mensajes que fueron entregados a todos los subs.
