/*
 * servidor.c
 *
 *  Created on: 3 mar. 2019
 *      Author: PokEbola
 */

#include "servidor.h"

int main(void) {
	char * ip;
	char * puerto;

	// Leer config
	config = leer_config("./Base/Servidor/config/servidor.config");

	log_nivel_key = config_get_string_value(config, "LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(log_nivel_key);

	// Iniciar Logger
	logger = iniciar_logger("./Base/Servidor/config/servidor.log", "Server",
			log_nivel_minimo);
	log_info(logger, "Primer log ingresado");

	ip = config_get_string_value(config, "IP");
	puerto = config_get_string_value(config, "PUERTO");

	// Inicar conexion
	iniciar_conexion_servidor(ip, puerto);

	//Terminar
	finalizar_ejecucion(logger, config);

	return EXIT_SUCCESS;
}

void finalizar_ejecucion(t_log* logger, t_config *config) {
	terminar_logger(logger);
	config_destroy(config);
}

void iniciar_conexion_servidor(char* ip, char* puerto) {

	log_info(logger, "Servidor Inicializado");
	//Set up conexion
	struct addrinfo* servinfo = obtener_server_info(ip, puerto); // Address info para la conexion TCP/IP
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor, servinfo);
	setear_socket_reusable(socket_servidor);
	freeaddrinfo(servinfo);
	log_info(logger, "Todo bien");

	listen(socket_servidor, SOMAXCONN);	// Prepara el socket para crear una conexión con el request que llegue. SOMAXCONN = numero maximo de conexiones acumulables

	while (1)
		esperar_cliente(socket_servidor);//Queda esperando que un cliente se conecte
}

void setear_socket_reusable(int socket) {
	int activado = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));
}

void esperar_cliente(int socket_servidor) {	// Hilo coordinador
	struct sockaddr_in dir_cliente;	//contiene address de la comunicacion

	int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);// Acepta el request del cliente y crea el socket

	// Lanzar los hilos handlers
	pthread_create(&thread, NULL, (void*) serve_client, &socket_cliente);// Crea un thread que se quede atendiendo al cliente
	pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente
}

void serve_client(int* socket) {
	int cod_op = recibir_codigo_operacion(*socket);
	process_request(cod_op, *socket);
}

void process_request(int cod_op, int cliente_fd) {
	int size;
	t_buffer * buffer;
	int valor;
	char* mensaje;
	log_info(logger, "Codigo de operacion: %d", cod_op);
	switch (cod_op) {
	case TEXTO:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer);
		//log_info(logger, "Mensaje Recibido.");
		log_info(logger, mensaje_recibido->contenido);

		t_buffer *buffer = serializar_mensaje(mensaje_recibido);
		log_info(logger, "Mensaje Serializado");

		enviar_mensaje(cliente_fd, buffer, TEXTO);
		log_info(logger, "Mensaje Enviado");

		free(buffer);
		break;
	case APPEARED_POKEMON:
		break;
	case NEW_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
		log_info(logger, "Mensaje new pokemon recibido");
		log_info(logger,
				"Pokemon: %s, Posicion X: %d, Posicion Y: %d, Cantidad: %d, ID: %d",
				mensaje_new_pokemon->pokemon, mensaje_new_pokemon->posx,
				mensaje_new_pokemon->posy, mensaje_new_pokemon->cantidad,
				mensaje_new_pokemon->id_mensaje);
		free(mensaje_new_pokemon);
		free(buffer);
		break;
	case CATCH_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(
				buffer);
		log_info(logger, "Mensaje catch pokemon recibido");
		log_info(logger, "Pokemon: %s, Posicion X: %d, Posicion Y: %d, ID: %d",
				mensaje_catch_pokemon->pokemon, mensaje_catch_pokemon->posx,
				mensaje_catch_pokemon->posy, mensaje_catch_pokemon->id_mensaje);
		free(mensaje_catch_pokemon);
		free(buffer);
		break;
		break;
	case CAUGHT_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_caugth_pokemon* mensaje_caugth_pokemon = deserializar_caught_pokemon(
				buffer);
		log_info(logger, "Mensaje caugth pokemon recibido");
		log_info(logger, "ID: %d, OK/FAIL: %d",
				mensaje_caugth_pokemon->id_mensaje,
				mensaje_caugth_pokemon->ok_or_fail);
		free(mensaje_caugth_pokemon);
		free(buffer);
		break;
		break;
	case GET_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
		log_info(logger, "Mensaje new pokemon recibido");
		log_info(logger, "Pokemon: %s", mensaje_get_pokemon->pokemon);
		free(mensaje_get_pokemon);
		free(buffer);
		break;
	case SUSCRIPTOR:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_subscriptor* mensaje_suscriptor = deserializar_suscripcion(buffer);
		log_info(logger, "Mensaje de suscripcion recibido");
		log_info(logger,
				"Cola de mensajes: %d, tiempo de suscripcion: %d segundos",
				mensaje_suscriptor->cola_de_mensaje,
				mensaje_suscriptor->tiempo);
		free(mensaje_suscriptor);
		free(buffer);
		break;

	case 0:
		log_info(logger, "Codigo invalido");
		pthread_exit(NULL);
		break;
	case -1:
		pthread_exit(NULL);
		break;
	}
}
