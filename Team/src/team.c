#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	t_list * head_entrenadores = cargar_entrenadores();

	t_list * pokemones_con_repetidos = obtener_pokemones(head_entrenadores);

	t_list * objetivo_global = formar_objetivo(pokemones_con_repetidos);

	//suscribirse_a_colas_necesarias();
	pthread_create(&thread, NULL, (void*)suscribirse_a_colas_necesarias,NULL);

	//enviar_requests_pokemones(objetivo_global);
	pthread_create(&thread, NULL, (void*)enviar_requests_pokemones,objetivo_global);

	iniciar_conexion_con_gameboy();

	//lanzar_hilos(head_entrenadores);



	// TT - To Test
	//t_entrenador * entrenador_cercano = hallar_entrenador_mas_cercano(head_entrenadores, 1, 3);

	sleep(10000); //TT
	finalizar_team();
}

// Comunicacion con broker
void suscribirse_a_colas_necesarias() {
	enviar_suscripcion_broker(APPEARED_POKEMON);
	enviar_suscripcion_broker(LOCALIZED_POKEMON);
	enviar_suscripcion_broker(CAUGHT_POKEMON);
}

void enviar_suscripcion_broker(op_code tipo_mensaje) {

	int socket_broker = iniciar_conexion_con_broker();
	enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);

	log_trace(logger,"socket a esperar %d",socket_broker);

	int* argument = malloc(sizeof(int));
	*argument = socket_broker;
	pthread_create(&thread, NULL, (void*)esperar_mensajes_cola, argument);

	log_trace(logger,"Suscripcion completada");

}

void enviar_mensaje_suscripcion(op_code mensaje, int conexion) {
	t_subscriptor* mensaje_suscripcion;
	mensaje_suscripcion = crear_suscripcion(mensaje, -10);

	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

	enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
	log_trace(logger,"Mensaje suscripcion enviado");
}

void esperar_mensajes_cola(void* input) {
	int conexion = *((int *)input);
	int cod_op = 1;

	log_trace(logger,"Esperando que aparezcan mensajes en %d",conexion);

	while (cod_op>=0) {
		cod_op = recibir_codigo_operacion(conexion);
		(cod_op == -1) ?
				log_error(logger, "Error en 'recibir_codigo_operacion'") :
				log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);

		if(cod_op>=0)
			manejar_mensaje_cola(conexion,cod_op);
	}
}

void manejar_mensaje_cola(int conexion,op_code cod_op){  //TODO: pending

	t_buffer * buffer = recibir_mensaje(conexion);

	switch (cod_op) {
				case APPEARED_POKEMON:

					log_trace(logger, "Se recibio un mensaje APPEARED_POKEMON");
					t_appeared_pokemon* mensaje_appeared_pokemon = deserializar_appeared_pokemon(buffer);

				break;
				case CAUGHT_POKEMON:

					log_trace(logger, "Se recibio un mensaje CAUGHT_POKEMON");
					t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(buffer);

				break;
				case LOCALIZED_POKEMON:

					log_trace(logger, "Se recibio un mensaje LOCALIZED_POKEMON");
					t_localized* mensaje_localized_pokemon = deserializar_localized_pokemon(buffer);

					break;
				default:
					log_error(logger,"Opcode inválido.");
					break;
	}

	log_trace(logger,"Mensaje recibido manejado.");
}



void enviar_requests_pokemones(t_list *objetivo_global) { // RECONTRA LIMPIAR
	int socket_broker = iniciar_conexion_con_broker();

	void enviar_mensaje_get_aux(void *elemento) { // USO INNER FUNCTIONS TODO: pasar a readme
		enviar_mensaje_get(socket_broker, elemento);
	}

	list_iterate(objetivo_global,enviar_mensaje_get_aux);

	log_trace(logger,"Gets enviados");
}

void enviar_mensaje_get(int socket_broker, void*element){
	t_objetivo *objetivo = element;
	t_get_pokemon* mensaje_get= crear_get_pokemon(objetivo->pokemon, -10);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_get_pokemon(mensaje_get);

	enviar_mensaje(socket_broker, mensaje_serializado, GET_POKEMON);
	log_trace(logger, "Enviado get para: %s", objetivo->pokemon);


}

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
		esperar_cliente(socket_gameboy);//Queda esperando que un cliente se conecte
	}
}

void esperar_cliente(int socket_servidor) {	// Hilo coordinador
	struct sockaddr_in dir_cliente;	//contiene address de la comunicacion

	int tam_direccion = sizeof(struct sockaddr_in);

	log_trace(logger, "Va a ejecutar 'accept'.");
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente,
			&tam_direccion);// Acepta el request del cliente y crea el socket
	log_trace(logger, "Conexion de %i al Broker.", socket_cliente);

	log_trace(logger, "Va a lanzar hilo 'recibir_mensaje_del_cliente'.");

	int * argument = malloc(sizeof(int));
	* argument = socket_cliente;
	pthread_create(&thread, NULL, (void*)esperar_mensajes_cola, argument);
}

/*
void manejar_recepcion_mensaje(int* socket_cliente) {
	int cod_op = recibir_codigo_operacion(*socket_cliente);
	log_trace(logger, "Mensaje Recibido codop: %d", cod_op);
	// Solo recibe appeared_pokemon del gameboy-> no hace falta los cases.
	t_appeared_pokemon * appeared_recibido = obtener_appeared_recibido(*socket_cliente);
	log_trace(logger, " Appeared Recibido, Pokemon: %s  Posicion en X : %d, Posicion en Y : %d",
			 appeared_recibido->pokemon, appeared_recibido->posx, appeared_recibido->posy);
}

t_appeared_pokemon * obtener_appeared_recibido(int socket_cliente){
	t_buffer * buffer= recibir_mensaje(socket_cliente);

	t_appeared_pokemon * appeared = deserializar_appeared_pokemon(buffer);
	return appeared;
}
*/

// Funciones Generales
void iniciar_team(char*argumentos_iniciales[]) {
	// Calcular archivo a abrir
	char * nombre_archivo_config = malloc(sizeof(argumentos_iniciales[1]));
	strcpy(nombre_archivo_config, argumentos_iniciales[1]);
	char *path_config = obtener_path(nombre_archivo_config);

	// Obtener info de config
	config = leer_config(path_config);
	free(nombre_archivo_config);

	// Leer data sobre looger del config
	string_nivel_log_minimo = config_get_string_value(config,
			"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
	char* nombre_archivo_log = config_get_string_value(config, "LOG_FILE");

	char * path_log = obtener_path(nombre_archivo_log);
	logger = iniciar_logger(path_log, "Team", log_nivel_minimo);

	log_trace(logger, "--- Log inicializado ---");

}

void finalizar_team() {
	terminar_logger(logger);
	config_destroy(config);
}

// Cargar path de config y log
char *obtener_path(char *path_leido) {
	char* path = string_new();
	string_append(&path, "./Team/config/");
	string_append(&path, path_leido);
	return path;
}

