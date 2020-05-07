#include "team.h"

int counter;
pthread_mutex_t lock;

int main(void){
	/* INICIALIZACION*/
	// Inicializacion
	logger = iniciar_logger("./Team/config/team.log","Team");
	log_info(logger,"Primer log ingresado");

	// Leer configuracion
	config =leer_config("./Team/config/team.config");
	log_info(logger,"Config creada");

	char ** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char ** pokemones_capturados = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char ** objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");

	// Cargado de entrenadores
	t_list * head_entrenadores =  cargar_entrenadores(posiciones, pokemones_capturados, objetivos);
	log_info(logger,"--- Entrenadores cargados ---");
	//TBR
	mostrar_entrenadores(head_entrenadores);
	log_info(logger," ---- Entrenadores mostrados ----");

	// Crear objetivo global - Lista a partir de los pokemones_a_capturar de cada entrenador
	t_list * pokemones_repetidos = obtener_pokemones(head_entrenadores);
	log_info(logger," ---- Repetidos Obtenidos ----");
	list_iterate(pokemones_repetidos,mostrarKokemon); //TBR - mostrar los repetdios

	log_info(logger," ---- Repetidos mostrados ----");

	t_list * objetivo_global = formar_objetivo(pokemones_repetidos);

	log_info(logger," ---- Objetivos Formado ----");

	list_iterate(objetivo_global,mostrarObjetivo);

	log_info(logger," ---- Objetivos Mostrados ----");


	// Suscribirse a la msj queue. Puede funcar sin broker.
		// Proceso team se conecta al broker. un connect por cada cola de mensajes.
		// Si el broker no existe tiene que funcar igual
			// Reintentar cada x tiempo

	//Enviar mensajes GET al broker
		// Uno por cada pokemon del objetivo
			// Por cada mensaje necesito un connect -- diferente de los de suscripciones.
			// Luego de enviar el mensaje, muere.

	//Conexion con gameboy
		//Socket escucha para appeard, caught y localized.
			// Por cada mensaje se recibe un accept.
			// Para testeo

	/* INICIALIZACION*/

	// MANEJO DE HILOS
	/*
	char entrenadores[3][50];
	for(i=0;i<3;i++){
			printf("La palabra es: %s",entrenadores[i]);
	}
	//cantidad_entrenadores = list_size(head_entrenadores);
	//printf("Se obtuvieron la info del config %s %s %s \n", posiciones,pokemones_capturados,objetivos);
	printoutarray(pokemones_capturados);
	// Hilos con semáforos mutex
	/*
	pthread_t tid[cantidad_entrenadores];
		int i = 0;
	    int error;
	    if (pthread_mutex_init(&lock, NULL) != 0){
	        printf("\n mutex init failed\n");
	        return 1;
	    }
	    while(i < cantidad_entrenadores){
	        error = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
	        if (error != 0)
	            printf("\ncan't create thread :[%s]", strerror(error));
	        i++;
	    }
	    for(i=0;i<cantidad_entrenadores;i++){
	    pthread_join(tid[i], NULL);
	    pthread_mutex_destroy(&lock);
	    }
	 */

	terminar_logger(logger);
	config_destroy(config);

}

// OBJETIVO GLOBAL

t_list* formar_objetivo(t_list * pokemones_repetidos){
	t_list * objetivo_global = list_create();

	void _agrego_si_no_existe(void *elemento){ // USO INNER FUNCTIONS TODO: pasar a readme
		 agrego_si_no_existe(objetivo_global,elemento);
	}

	list_iterate(pokemones_repetidos,_agrego_si_no_existe);

	return objetivo_global;
}


void agrego_si_no_existe(t_list * objetivo_global,void *nombrePokemon){
	bool _yaExiste(void *inputObjetivo){
		t_objetivo *cadaObjetivo = inputObjetivo;
		return !strcmp(cadaObjetivo->pokemon,nombrePokemon);
	}

	t_objetivo *objetivo =list_find(objetivo_global,_yaExiste);

	if(objetivo!=NULL){
		objetivo->cantidad++;
	}else{
		t_objetivo * nuevo_objetivo = malloc(sizeof(t_objetivo));
		nuevo_objetivo->cantidad = 1;
		nuevo_objetivo->pokemon = nombrePokemon;
		list_add(objetivo_global,nuevo_objetivo);
	}

}

void mostrarKokemon(void*elemento){	//TODO: Cambiar a naming convention
	log_info(logger,elemento);
}


t_list* obtener_pokemones(t_list *head_entrenadores){
	t_list * pokemones_repetidos = list_create();
	t_entrenador * entrenador;
	char * pokemon;

	for(int i = 0;(entrenador = list_get(head_entrenadores,i))!=NULL;i++){
		for(int j = 0;(pokemon = list_get(entrenador->pokemones_por_capturar,j))!=NULL;j++)
			list_add(pokemones_repetidos,pokemon);
	}
	return pokemones_repetidos;
}

void mostrarObjetivo(void *elemento){
	log_info(logger,"Data de objetivo!");
	t_objetivo * objetivo= malloc(sizeof(t_objetivo));	// Sacar esto?
	objetivo = elemento;
	log_info(logger, "objetivo: %s", objetivo->pokemon);
	log_info(logger, "objetivo: %i", objetivo->cantidad);
}

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


// MANEJO DE LISTAS

t_list* cargar_entrenadores(char** posiciones, char** pokemones_capturados,char** objetivos){

	log_info(logger,"Entre a funcion");

	t_list* head_entrenadores = list_create();

	log_info(logger,"Cree lista entrenadores");

	int i=0;
	while(posiciones[i]!= NULL){	// TODO: Cambiar a for
		t_entrenador * entrenador = malloc(sizeof(t_entrenador));
		entrenador->posicion = de_string_a_posicion(posiciones[i]);
		entrenador->pokemones_capturados = string_a_pokemon_list(pokemones_capturados[i]);
		entrenador->pokemones_por_capturar = string_a_pokemon_list(objetivos[i]);

		list_add(head_entrenadores, entrenador);

		i++;
	}
	return(head_entrenadores);
}
void mostrar_entrenadores(t_list * head_entrenadores){
	list_iterate(head_entrenadores,mostrar_data_entrenador);
}

void mostrar_data_entrenador(void * element){
	t_entrenador * entrenador = element;
	log_info(logger,"Data Entrenador: Posicion %i %i",entrenador->posicion[0],entrenador->posicion[1]);
	list_iterate(entrenador->pokemones_capturados, mostrarKokemon);
	list_iterate(entrenador->pokemones_por_capturar, mostrarKokemon);
}

// PARSERS DE INPUT DATA

int* de_string_a_posicion(char* cadena_con_posiciones) {
	char** posicion_prueba = string_split(cadena_con_posiciones, "|");

	int* posicion = malloc(sizeof(int)*3);
	posicion[0] = atoi(posicion_prueba[0]);
	posicion[1]= atoi(posicion_prueba[1]);

	return posicion;
}
t_list* string_a_pokemon_list(char* cadena_con_pokemones) {
	char** pokemones = string_split(cadena_con_pokemones, "|");

	t_list* head_pokemones = list_create();

	int i = 0;

	while(pokemones[i]!=NULL){
		list_add(head_pokemones,pokemones[i]);
		i++;
	}

	return head_pokemones;

}
