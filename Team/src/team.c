#include "team.h"

int counter;
pthread_mutex_t lock;

int main(void){

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
	log_info(logger,"Entrenadores cargados");
	//TBR
	mostrar_entrenadores(head_entrenadores);
	log_info(logger,"Entrenadores mostrados");


	// Crear objetivo global - Lista a partir de los pokemones_a_capturar de cada entrenador
	t_list * objetivo_global = list_create();
	t_list * pokemones_repetidos = obtener_pokemones(head_entrenadores);

	//TBR
	char*kokemon;
	for(int i = 0;(kokemon= list_get(pokemones_repetidos,i))!=NULL;i++)log_info(logger,kokemon);



	// Si el pokemon ya existe, sumarle la cantidad

	// Si el pokemon no existe, agregarlo a la lista con cantidad 1

	// Suscribirse a la msj queue. Puede funcar sin broker.


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
	t_entrenador * entrenador = malloc(sizeof(t_entrenador));

	for(int i = 0;(entrenador = list_get(head_entrenadores,i))!=NULL;i++){
		log_info(logger,"Data Entrenador %i: Posicion %i %i", i,entrenador->posicion[0],entrenador->posicion[1]);
		char*pokemon;
		for(int j = 0;(pokemon = list_get(entrenador->pokemones_capturados,j))!=NULL;j++) log_info(logger,"%s",pokemon);
		for(int j = 0;(pokemon = list_get(entrenador->pokemones_por_capturar,j))!=NULL;j++) log_info(logger,"%s",pokemon);
		i++;
	}
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
