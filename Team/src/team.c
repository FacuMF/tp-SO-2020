#include "team.h"

int counter;

int main(int argv,char*archivo_config[]) {

	iniciar_team(archivo_config);

	t_list * head_entrenadores = cargar_entrenadores();

	t_list * pokemones_con_repetidos = obtener_pokemones(head_entrenadores);

	t_list * objetivo_global = formar_objetivo(pokemones_con_repetidos);

	//lanzar_hilos(head_entrenadores);

	iniciar_conexion_con_gameboy();	//TO DO

	//iniciar_conexion_con_broker();

	//enviar_requests_pokemones()

	// Atender mensajes


	// TT - To Test
	t_entrenador * entrenador_cercano = hallar_entrenador_mas_cercano(head_entrenadores,1,3);


	// Conectar_con_gameboy

	sleep(30); //TT
	finalizar_team();
}


// Comunicacion con broker

void enviar_requests_pokemones(t_list *objetivo_global){

	//iterar por cada elemento de la lista objetivo_global
		// Enviar mensajes

	//log_trace(logger,"Gets enviados");
}

void iniciar_conexion_con_broker(){
	// conectar con broker
	// mandar mensajes de suscripcion
}

void iniciar_conexion_con_gameboy(){
	char * ip_gameboy = config_get_string_value(config,"IP_GAMEBOY");
	char * puerto_gameboy=config_get_string_value(config,"PUERTO_GAMEBOY");
	log_trace(logger,"Ip Gameboy Leida : %s Puerto Gameboy Leido : %s\n",ip_gameboy,puerto_gameboy);

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy,puerto_gameboy);

	listen(socket_gameboy, SOMAXCONN);	// Prepara el socket para crear una conexión con el request que llegue. SOMAXCONN = numero maximo de conexiones acumulables

	while(1){
		log_trace(logger,"Esperando Cliente");
		esperar_cliente(socket_gameboy);//Queda esperando que un cliente se conecte
	}
}

void esperar_cliente(int socket_servidor) {	// Hilo coordinador
	struct sockaddr_in dir_cliente;	//contiene address de la comunicacion

	int tam_direccion = sizeof(struct sockaddr_in);

	log_trace(logger, "Va a ejecutar 'accept'.");
	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);// Acepta el request del cliente y crea el socket


	manejar_recepcion_mensaje(&socket_cliente);

	// Lanzar los hilos handlers
	//pthread_create(&thread, NULL, (void*) manejar_recepcion_mensaje, &socket_cliente);// Crea un thread que se quede atendiendo al cliente
	//pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente
}

void manejar_recepcion_mensaje(int* socket_cliente){
	int cod_op = recibir_codigo_operacion(*socket_cliente);
	log_trace(logger,"Mensaje Recibido codop: %d",cod_op);
}

// Funciones Generales
void iniciar_team(char*argumentos_iniciales[]){
	// Calcular archivo a abrir
	char * nombre_archivo_config= malloc(sizeof(argumentos_iniciales[1]));
	strcpy(nombre_archivo_config,argumentos_iniciales[1]);
	char *path_config = obtener_path(nombre_archivo_config);

	// Obtener info de config
	config = leer_config(path_config);
	free(nombre_archivo_config);

	// Leer data sobre looger del config
	string_nivel_log_minimo = config_get_string_value(config, "LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
	char* nombre_archivo_log= config_get_string_value(config,"LOG_FILE");

	char * path_log=obtener_path(nombre_archivo_log);
	logger = iniciar_logger(path_log, "Team", log_nivel_minimo);

	log_trace(logger, "--- Log inicializado ---");

}

void finalizar_team(){
	terminar_logger(logger);
	config_destroy(config);
}


// Cargar path de config y log
char *obtener_path(char *path_leido){
	 char* path=string_new();
	 string_append(&path,"./Team/config/");
	 string_append(&path,path_leido);
	return path;
}

