#include "team.h"

int counter;

int main(int argv,char*archivo_config[]) {

	iniciar_team(archivo_config);

	t_list * head_entrenadores = cargar_entrenadores();

	t_list * pokemones_repetidos = obtener_pokemones(head_entrenadores);

	t_list * objetivo_global = formar_objetivo(pokemones_repetidos);

	// TO DO: LANZAMIENTO DE MENSAJE GET_POKEMON POR CADA ESPECIE DE POKEMON EN EL OBJETIVO GLOBAL.

	log_info(logger, "Esto es un mensaje obligatorio");

	lanzar_hilos(head_entrenadores);

	// Suscribir  a cola broker

	// Atender mensajes

	// TT - To Test
	t_entrenador * entrenador_cercano = hallar_entrenador_mas_cercano(head_entrenadores,1,3);


	// Conectar_con_gameboy

	finalizar_team();
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

// MANEJO DE HILOS
void lanzar_hilos(t_list *head_entrenadores){
	list_iterate(head_entrenadores, lanzar_hilo_entrenador);
	log_trace(logger, "Hilos lanzados");
}

int objetivo_cumplido(t_entrenador *entrenador){
	return list_is_empty(entrenador->pokemones_por_capturar);
}
void lanzar_hilo_entrenador(void*element) {
	t_entrenador * entrenador = element;

	pthread_t hilo_entrenador;
	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,(void*)entrenador);
	if (result != 0)
		log_error(logger, "Error lanzando el hilo"); //TODO: revisar manejo de errores

	result = pthread_join(hilo_entrenador, NULL);
	if (result != 0)
		log_error(logger, "Error joineando el hilo"); //TODO: revisar manejo de errores

	return;

}

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while(!(objetivo_cumplido(entrenador)))
	{
		pthread_mutex_lock(&(entrenador->sem_est));
		log_trace(logger, "Data Entrenador: Posicion %i %i", entrenador->posicion[0],
					entrenador->posicion[1]);
		//ir a buscar pokemon, etc

	}
}

double distancia(t_entrenador * entrenador, double posx, double posy){
	double distancia_e= sqrt(suma_de_distancias_al_cuadrado(entrenador,posx,posy));
	return distancia_e;
}
double suma_de_distancias_al_cuadrado(t_entrenador*entrenador, double posx, double posy){
	double suma = pow(distancia_en_eje(entrenador,posx,1),2) + pow(distancia_en_eje(entrenador,posy,2),2);
	return suma;
}
double distancia_en_eje(t_entrenador *entrenador, double pose, int pos){
	double resta= entrenador->posicion[pos] - pose;
	return resta;
}

// Funciones para planificacion del entrenador

t_entrenador * hallar_entrenador_mas_cercano(t_list * head_entrenadores,double posx, double posy){

	bool menor_distancia(void*elemento_1,void*elemento_2){
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return distancia(entrenador_1,posx,posy) > distancia(entrenador_2,posx,posy);
	}

	t_list * entrenadores_mas_cercanos = list_sorted(head_entrenadores, menor_distancia);
	t_entrenador * entrenador_mas_cercano = list_get(entrenadores_mas_cercanos,0);

	log_trace(logger, "Posicion entrenador cercano: Posicion %i %i", entrenador_mas_cercano->posicion[0], entrenador_mas_cercano->posicion[1]);
	return entrenador_mas_cercano;
}


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
	while (posiciones[i] != NULL) {	// TODO: Cambiar a for
		t_entrenador * entrenador = malloc(sizeof(t_entrenador));
		pthread_mutex_init(&entrenador->sem_est, NULL);
		pthread_mutex_lock(&(entrenador->sem_est));	//TODO: Ver como inicializar hilo en 0 sin hacer lock inmediatamente

		entrenador->posicion = de_string_a_posicion(posiciones[i]);
		entrenador->pokemones_capturados = string_a_pokemon_list(pokemones_capturados[i]);
		entrenador->pokemones_por_capturar = string_a_pokemon_list(objetivos[i]);
		list_add(head_entrenadores, entrenador);
		i++;
	}

	log_trace(logger, "--- Entrenadores cargados ---");
	mostrar_entrenadores(head_entrenadores);

	return (head_entrenadores);
}

void mostrar_entrenadores(t_list * head_entrenadores) {
	list_iterate(head_entrenadores, mostrar_data_entrenador);
	log_trace(logger, "--- Entrenadores mostrados ---");
}

void mostrar_data_entrenador(void * element) {
	t_entrenador * entrenador = element;
	log_trace(logger, "Data Entrenador: Posicion %i %i", entrenador->posicion[0],
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
	list_iterate(objetivo_global, mostrar_objetivo);

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
	list_iterate(pokemones_repetidos, mostrar_kokemon);
	return pokemones_repetidos;
}

void mostrar_kokemon(void*elemento) {
	log_trace(logger, elemento);
}

void mostrar_objetivo(void *elemento) {
	log_trace(logger, "Data de objetivo!");

	t_objetivo *objetivo = elemento;
	log_trace(logger, "objetivo: %s", objetivo->pokemon);
	log_trace(logger, "objetivo: %i", objetivo->cantidad);

}
