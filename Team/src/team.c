#include "team.h"

int counter;
pthread_mutex_t lock;

int main(void){
	// ** INICIALIZACION **


//	int cantidad_entrenadores;
	int i;
	// Iniciar Logger
	logger = iniciar_logger("./Team/config/team.log","Team");
	log_info(logger,"Primer log ingresado");

	//		** Obtener los entrenadores y localizarlos **

	config =leer_config("./Team/config/team.config");
	log_info(logger,"Config creada");
	t_list * head_entrenadores = list_create();

	char ** posiciones = config_get_array_value(config,"POSICIONES_ENTRENADORES");
	char ** pokemones_capturados = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char ** objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	for(i=0; i<3; i++){
			log_info(logger,
					"Leido de config entrenador %i: Posicion: %s , Pokes capturados: %s , Pokes por capturar: %s .",
					i+1 ,posiciones[i] ,pokemones_capturados[i] ,objetivos[i] );
	}

	//cargar_y_localizar_entrenadores(head_entrenadores, posiciones, pokemones_capturados, objetivos);
















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


/*
//No funciona
void cargar_y_localizar_entrenadores(t_list* head_entrenadores, char** posiciones, char** pokemones_capturados,char** objetivos){
	entrenador* aux;
	int i;

	for (i=0; i<=10; i++){
		aux->posicion = de_string_a_posicion(posiciones[i]);

		aux->pokemones_capturados = de_string_a_pokemon(pokemones_capturados[i]);

		aux->pokemones_por_capturar = de_string_a_pokemon(objetivos[i]);

		list_add(head_entrenadores, aux);

	}

	free(aux);
}
*/

int* de_string_a_posicion(char* string) {
	char** posicion_prueba = string_split(string, "|");
	int* posicion = malloc(sizeof(int)*3);

	posicion[0] = atoi(posicion_prueba[0]);
	posicion[1]= atoi(posicion_prueba[1]);

	return posicion;
}

/*
//No Funciona
char** de_string_a_pokemones(char* string) {
	char** pokemones = string_split(string, "|");

	return pokemones;
}
*/


/*


// Función al hacer entre hilos *por completar*
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


