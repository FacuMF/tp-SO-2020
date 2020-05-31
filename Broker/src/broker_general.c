#include "broker.h"

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
void* esperar_mensajes(void *arg) {
	log_trace(logger, "Esperando mensajes. ");
	int i = 0;
	while (i < 20) {
		i++;
		char* ip = config_get_string_value(config, "IP_BROKER");
		char* puerto = config_get_string_value(config, "PUERTO_BROKER");
		iniciar_conexion_con_modulo(ip, puerto);
	}
}

void* iniciar_conexion_con_modulo(char* ip, char* puerto) {

	log_trace(logger, "Servidor Inicializado");
	//Set up conexion

	struct addrinfo* servinfo = obtener_server_info(ip, puerto);
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor, servinfo);
	setear_socket_reusable(socket_servidor);
	freeaddrinfo(servinfo);
	log_info(logger, "Listen");

	listen(socket_servidor, SOMAXCONN);

	while (1) {
		log_info(logger, "Ejecutar Handle Cliente");
		handle_cliente(socket_servidor);
	}
	return 0;
}

void handle_cliente(int socket_servidor) {
	log_info(logger, "Adentro del Handle Cliente");
	struct sockaddr_in dir_cliente;

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);
	/*
	 //TODO la variable thread es una sola, esto va a dar problemas, hay que hacer que sean varias de alguna forma. Finitas o infinitas?
	 pthread_create(&thread, NULL, (void*) recibir_mensaje_del_cliente, &socket_cliente);// Crea un thread que se quede atendiendo al cliente
	 pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente
	 */
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
 
void setear_socket_reusable(int socket) {
	int activado = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}