#include "team.h"

int counter;
pthread_mutex_t lock;

int main(int argv,char*archivo_config[]) {
	/* INICIALIZACION*/
	// Leer configuracion

	char * archivo_leido= malloc(sizeof(archivo_config[1]));
	strcpy(archivo_leido,archivo_config[1]);
	char *path_log=obtener_path(archivo_leido);
	config = leer_config(path_log);
	free(archivo_leido);


	log_nivel_key = config_get_string_value(config, "LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(log_nivel_key);
	char* log_leido= config_get_string_value(config,"LOG_FILE");

	char * log_path=obtener_path(log_leido);
	logger = iniciar_logger(log_path, "Team", log_nivel_minimo);
	log_trace(logger, "--- Log inicializado ---");

	log_trace(logger, "Config creada");

	char ** posiciones = config_get_array_value(config,
			"POSICIONES_ENTRENADORES");
	char ** pokemones_capturados = config_get_array_value(config,
			"POKEMON_ENTRENADORES");
	char ** objetivos = config_get_array_value(config,
			"OBJETIVOS_ENTRENADORES");

	// Cargado de entrenadores
	t_list * head_entrenadores = cargar_entrenadores(posiciones,
			pokemones_capturados, objetivos);
	mostrar_entrenadores(head_entrenadores);

	// Crear objetivo global - Lista a partir de los pokemones_a_capturar de cada entrenador
	t_list * pokemones_repetidos = obtener_pokemones(head_entrenadores);
	list_iterate(pokemones_repetidos, mostrar_kokemon);

	t_list * objetivo_global = formar_objetivo(pokemones_repetidos);
	list_iterate(objetivo_global, mostrar_objetivo);

	//WAIT = pthread_mutex_lock(&lock);
	//SIGNAL = pthread_mutex_unlock(/*semaforo*/);

	/* MANEJO DE HILOS (con semáforos mutex)*/
	pthread_mutex_init(&lock, NULL);

	pthread_mutex_lock(&lock);

	list_iterate(head_entrenadores, lanzar_hilos);
	log_trace(logger, "Hilos lanzados");

	//Conectar con gameboy

	/*char* ip_gameboy = "127.0.0.2";
	 char* puerto_gameboy = "5002";

	 iniciar_conexion_servidor(ip_gameboy,puerto_gameboy);*/

	//Terminar
	pthread_mutex_destroy(&lock);
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

// MANEJO DE HILOS

void lanzar_hilos(void*element) {
	t_entrenador * entrenador = element;
	// Inicializo mutex de cada entrenador en 0
	pthread_mutex_lock(&(entrenador->sem_est));

	/* Inicializar el mutex de este entrenador en 0.
	 * Cuando el entrenador arranque y tire wait va a pasar a -1 y se bloquea.
	 * Cuando haya un localizado que el es el mas cercano le tire signal, pasa a 0 y se desbloquea.
	 * Cuando lo atrapa, vuelve a hacer wait (por el while no haya terminado su objetivo) y se bloquea de nuevo.
	 * Antes de tirar el signal (o al localizar, filtrar aquellos que su sem esta < 1), chequear si el sem < 0 significa que esta esperando. Si es 0 esta laburando, no me sirve.
	 */

	pthread_t hilo_entrenador;
	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,
	NULL);
	if (result != 0)
		log_error(logger, "Error lanzando el hilo"); //TODO: revisar manejo de errores

	result = pthread_join(hilo_entrenador, NULL);
	if (result != 0)
		log_error(logger, "Error joineando el hilo"); //TODO: revisar manejo de errores

	return;

}

void ser_entrenador(/*t_entrenador* entrenador*/) {
	//While(objetivo_cumplido(entrenador)!=1){
	//pthread_mutex_lock(&(entrenador->sem_est);
	//El entrenador se bloquea al
	//tirar un wait a su semaforo,
	//el cual se inicializo en 0 y queda en -1 y espera a que el orchestrato( Cuando se Encuentra el entrenador mas cercano)
	//le tire un signal

	/*
	 * Tirar un wait a la lista de localizados // otro semaforo
	 * va a buscar a la lista de localizados el pokemon para moverse y mandar mensaje CAUGHT
	 * Tirar un signal a la lista de localizados
	 * ...
	 * */

	//}
	// Tiro return
	// El mismo NO se desbloquea, esto no va
	// Para tests
	pthread_mutex_lock(&lock);
	printf("Holis \n");
	pthread_mutex_unlock(&lock);
}

// CARGAR ENTRENADORES

t_list* cargar_entrenadores(char** posiciones, char** pokemones_capturados,
		char** objetivos) {
	t_list* head_entrenadores = list_create();

	int i = 0;
	while (posiciones[i] != NULL) {	// TODO: Cambiar a for
		t_entrenador * entrenador = malloc(sizeof(t_entrenador));
		pthread_mutex_init(&entrenador->sem_est, NULL);
		entrenador->posicion = de_string_a_posicion(posiciones[i]);
		entrenador->pokemones_capturados = string_a_pokemon_list(
				pokemones_capturados[i]);
		entrenador->pokemones_por_capturar = string_a_pokemon_list(
				objetivos[i]);
		list_add(head_entrenadores, entrenador);
		i++;
	}

	log_trace(logger, "--- Entrenadores cargados ---");
	return (head_entrenadores);
}

void mostrar_entrenadores(t_list * head_entrenadores) {
	list_iterate(head_entrenadores, mostrar_data_entrenador);
	log_trace(logger, "--- Entrenadores mostrados ---");
}

void mostrar_data_entrenador(void * element) {
	t_entrenador * entrenador = element;
	log_info(logger, "Data Entrenador: Posicion %i %i", entrenador->posicion[0],
			entrenador->posicion[1]);
	list_iterate(entrenador->pokemones_capturados, mostrar_kokemon);
	list_iterate(entrenador->pokemones_por_capturar, mostrar_kokemon);
}

// PARSERS DE INPUT DATA

int* de_string_a_posicion(char* cadena_con_posiciones) {
	char** posicion_prueba = string_split(cadena_con_posiciones, "|");

	int* posicion = malloc(sizeof(int) * 3);
	posicion[0] = atoi(posicion_prueba[0]);
	posicion[1] = atoi(posicion_prueba[1]);

	return posicion;
}

t_list* string_a_pokemon_list(char* cadena_con_pokemones) {
	char** pokemones = string_split(cadena_con_pokemones, "|");

	t_list* head_pokemones = list_create();

	int i = 0;

	while (pokemones[i] != NULL) {
		list_add(head_pokemones, pokemones[i]);
		i++;
	}

	return head_pokemones;

}

// OBJETIVO GLOBAL
void aniadir_pokemon(t_list *pokemones_repetidos, void * pokemones) {
	list_add(pokemones_repetidos, pokemones);
}

t_list* formar_objetivo(t_list * pokemones_repetidos) {
	t_list * objetivo_global = list_create();
	void agrego_si_no_existe_aux(void *elemento) { // USO INNER FUNCTIONS TODO: pasar a readme
		agrego_si_no_existe(objetivo_global, elemento);
	}

	list_iterate(pokemones_repetidos, agrego_si_no_existe_aux);

	log_trace(logger, " ---- Objetivos Formados ----");
	return objetivo_global;
}

void agrego_si_no_existe(t_list * objetivo_global, void *nombrePokemon) {
	bool _yaExiste(void *inputObjetivo) {
		t_objetivo *cadaObjetivo = inputObjetivo;
		return !strcmp(cadaObjetivo->pokemon, nombrePokemon);
	}

	t_objetivo *objetivo = list_find(objetivo_global, _yaExiste);

	if (objetivo != NULL) {
		objetivo->cantidad++;
	} else {
		t_objetivo * nuevo_objetivo = malloc(sizeof(t_objetivo));
		nuevo_objetivo->cantidad = 1;
		nuevo_objetivo->pokemon = nombrePokemon;
		list_add(objetivo_global, nuevo_objetivo);
	}

}

t_list* obtener_pokemones(t_list *head_entrenadores) {
	t_list * pokemones_repetidos = list_create();
	void aniadir_pokemon_aux(void *pokemones) {
		aniadir_pokemon(pokemones_repetidos, pokemones);
	}
	void buscar_pokemon(void * head) {
		t_entrenador *entrenador = head;
		list_iterate(entrenador->pokemones_por_capturar, aniadir_pokemon_aux);
	}

	list_iterate(head_entrenadores, buscar_pokemon);

	log_trace(logger, "--- Kokemones obtenidos ---");
	return pokemones_repetidos;
}

void mostrar_kokemon(void*elemento) {
	log_info(logger, elemento);
}

void mostrar_objetivo(void *elemento) {
	log_info(logger, "Data de objetivo!");

	t_objetivo *objetivo = elemento;
	log_info(logger, "objetivo: %s", objetivo->pokemon);
	log_info(logger, "objetivo: %i", objetivo->cantidad);

}

//Chequear objetivo personal cumplido
/*
 bool objetivo_cumplido(t*entrenador entrenador){
 return list_is_empty(entrenador->pokemones_por_capturar) ;
 }
 */

//MANEJO DE HILOS
/*
 void* doSomeThing(void *arg){

 pthread_mutex_lock(&lock);
 unsigned long i = 0;
 counter += 1;
 printf("\n Hilo %d started\n", counter);
 for(i=0; i<(0xFFFFFFFF);i++);
 printf("\n Hilo %d finished\n", counter);
 pthread_mutex_unlock(&lock);
 }
 */

// Socket servidor para conectar con gameboy
void iniciar_conexion_servidor(char* ip, char* puerto) {

	log_info(logger, "Servidor Inicializado");
	//Set up conexion
	struct addrinfo* servinfo = obtener_server_info(ip, puerto); // Address info para la conexion TCP/IP
	int socket_servidor = obtener_socket(servinfo);
	asignar_socket_a_puerto(socket_servidor, servinfo);
	setear_socket_reusable(socket_servidor);
	freeaddrinfo(servinfo);
	log_info(logger, "Esperando conexion.");
	listen(socket_servidor, SOMAXCONN);	// Prepara el socket para crear una conexión con el request que llegue. SOMAXCONN = numero maximo de conexiones acumulables
	log_info(logger, "Conectado, esperando cliente.");
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

	log_info(logger, "Conexion acceptada, activo hilos.");

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
	switch (cod_op) {
	case APPEARED_POKEMON:
		buffer = recibir_mensaje(cliente_fd);
		t_appeared_pokemon* mensaje_appeared_pokemon =
				deserializar_appeared_pokemon(buffer);

		//TBR
		log_info(logger,
				"Se deserializo el mensaje APPEARED_POKEMON.Tamanio : %i Pokemon: %s , Pos: (%i,%i) .",
				mensaje_appeared_pokemon->size_pokemon,
				mensaje_appeared_pokemon->pokemon,
				mensaje_appeared_pokemon->posx, mensaje_appeared_pokemon->posy);

		log_info(logger, "Mensaje Recibido.");

		free(mensaje_appeared_pokemon);

		break;
	case TEXTO:
		buffer = recibir_mensaje(cliente_fd);
		t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer);
		log_info(logger, "Mensaje Recibido.");
		log_info(logger, mensaje_recibido->contenido);

		t_buffer *buffer = serializar_mensaje(mensaje_recibido);
		log_info(logger, "Mensaje Serializado");

		enviar_mensaje(cliente_fd, buffer, TEXTO);
		log_info(logger, "Mensaje Enviado");

		free(buffer);
		break;
	case 0:
		pthread_exit(NULL);
	case -1:
		pthread_exit(NULL);
	}
}
