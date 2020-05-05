#include "team.h"

int counter;
pthread_mutex_t lock;

int main(void){
	int i;
	int cantidad_entrenadores;
	t_config* config;
	config = config =leer_config("/team.config");
	//entrenador * head_entrenadores = list_create();
	//char ** posiciones,pokemones_capturados,objetivos;
	// Obtener de config los arrays y cargar las caracteristicas de los entrenadores a mi head
	char ** posiciones = config_get_array_value(config,"POSICION_ENTRENADORES");
	char ** pokemones_capturados = config_get_array_value(config,"POKEMON_ENTRENADORES");
	char ** objetivos = config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
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
}

void printoutarray(char ** pointeratoarray){
	int i,j;
	for( i=0;i<=2;i++){
		for(j=0;j<=2;j++){
			printf("\n Element is: %s \n",&pointeratoarray[i]);

	}
}

// Función al hacer entre hilos *por completar*
void* doSomeThing(void *arg){

    pthread_mutex_lock(&lock);
    unsigned long i = 0;
    counter += 1;
    printf("\n Hilo %d started\n", counter);
    for(i=0; i<(0xFFFFFFFF);i++);
    printf("\n Hilo %d finished\n", counter);
    pthread_mutex_unlock(&lock);
    return NULL;
}


