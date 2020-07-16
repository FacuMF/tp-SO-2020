#include "team.h"

// CARGAR ENTRENADORES

t_list* cargar_entrenadores() {
	t_list* head_entrenadores = list_create();

	char ** posiciones = config_get_array_value(config,
			"POSICIONES_ENTRENADORES");
	char ** pokemones_capturados = config_get_array_value(config,
			"POKEMON_ENTRENADORES");
	char ** objetivos = config_get_array_value(config,
			"OBJETIVOS_ENTRENADORES");


	int i = 0;

	while (posiciones[i] != NULL) {
		t_entrenador * entrenador = malloc(sizeof(t_entrenador));
		sem_init(&entrenador->sem_est,1,0);
		entrenador -> id = i + 65;
		entrenador->posicion = de_string_a_posicion(posiciones[i]);

		entrenador->pokemones_capturados = list_create();

		entrenador->pokemones_por_capturar = string_a_pokemon_list(
				objetivos[i]);
		entrenador->catch_pendiente = NULL;
		entrenador->deadlock = NULL;
		entrenador->estado = NEW;
		entrenador->estimacion_rafaga = estimacion_inicial;
		list_add(head_entrenadores, entrenador);
		i++;
	}

	agregar_capturados(head_entrenadores,pokemones_capturados);
	log_trace(logger, "Entrenadores cargados");
	//mostrar_entrenadores(head_entrenadores);

	return (head_entrenadores);
}

void agregar_capturados(t_list * lista_head,char ** pokemones_capturados){

	// [pikachu,NULL]
	// [pikachu|sqirtle,pidgey|etc,NULL]
	// entrenador->pokemones_capturados = string_a_pokemon_list(
	//pokemones_capturados[i]);

	int j = 0;

	while(pokemones_capturados[j]!=NULL){
		t_entrenador * entrenador = list_get(lista_head,j);
		t_list * pokemones_a_agregar = string_a_pokemon_list(pokemones_capturados[j]);
		list_add_all(entrenador->pokemones_capturados,pokemones_a_agregar);
		j++;
	}
}


void lanzar_hilo_entrenador(void*element) {
	t_entrenador * entrenador = element;
	pthread_t hilo_entrenador;

	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,
			(void*) entrenador);

	(result != 0) ?
			log_error(logger, "Error lanzando el hilo") :
			log_trace(logger, "Entrenador lanzado: Pos %i %i",
					entrenador->posicion[0], entrenador->posicion[1]);
}

void inicializar_listas(){
	//TODO: en lugares que haga falta, mutex
	head_entrenadores = cargar_entrenadores();
	ids_mensajes_utiles = list_create();
	appeared_a_asignar = list_create();
	appeared_auxiliares = list_create();
	pokemones_recibidos = list_create();

	// Metricas
	deadlocks_totales = 0;
}

void reintento_suscripcion_si_aplica(){
	pthread_mutex_lock(&chequeo_sem_suscrip);
	int val_semaforo;
	sem_getvalue(&suscripcion, &val_semaforo);
	if(val_semaforo < 1)
		sem_post(&suscripcion);
	pthread_mutex_unlock(&chequeo_sem_suscrip);
}

// CONFIG
void iniciar_config_team(char* nombre_config) {
	char *path_config = obtener_path(nombre_config);
	config = leer_config(path_config);
}

char *obtener_path(char *path_leido) {
	char* path = string_new();
	string_append(&path, "./Team/config/");
	string_append(&path, path_leido);
	return path;
}

void obtener_variables_planificacion(){
	quantum = config_get_int_value(config, "QUANTUM");
	estimacion_inicial = config_get_double_value(config, "ESTIMACION_INICIAL");
	retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	constante_estimacion = config_get_double_value(config, "CONSTANTE_ESTIMACION");//TODO: Puede ser con coma, testear
	desalojar = 0;
}

void elegir_algoritmo() {
	char* algoritmo = config_get_string_value(config,
			"ALGORITMO_PLANIFICACION");

	if (strcmp(algoritmo, "FIFO") == 0) {
		algoritmo_elegido = A_FIFO;
	} else if (strcmp(algoritmo, "RR") == 0) {
		algoritmo_elegido = A_RR;
	} else if (strcmp(algoritmo, "SJF-CD") == 0) {
		algoritmo_elegido = A_SJFCD;
	} else if (strcmp(algoritmo, "SJF-SD") == 0) {
		algoritmo_elegido = A_SJFSD;
	} else {
		log_error(logger, "Algoritmo invalido");
	}
}

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

// LOGGER
void iniciar_logger_team(){
	string_nivel_log_minimo = config_get_string_value(config,
				"LOG_NIVEL_MINIMO");
		log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
		char* nombre_archivo_log = config_get_string_value(config, "LOG_FILE");

		char * path_log = obtener_path(nombre_archivo_log);
		logger = iniciar_logger(path_log, "Team", log_nivel_minimo);

		log_trace(logger, "Log inicializado");
}

void mostrar_entrenadores(t_list * head_entrenadores) {
	list_iterate(head_entrenadores, mostrar_data_entrenador);
}

void mostrar_data_entrenador(void * element) {
	t_entrenador * entrenador = element;
	log_trace(logger, "Data Entrenador %c: Posicion %i %i",entrenador->id,
			entrenador->posicion[0], entrenador->posicion[1]);
	log_trace(logger, "Estado: %d",entrenador->estado);
	list_iterate(entrenador->pokemones_capturados, mostrar_kokemon);
	list_iterate(entrenador->pokemones_por_capturar, mostrar_kokemon);
}

void mostrar_kokemon(void*elemento) {
	log_trace(logger,elemento);
}


