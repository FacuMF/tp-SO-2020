#include "team.h"

int counter;
pthread_mutex_t lock;

int main(void){
	int cantidadEntrenadores;
	t_config* config;
	entrenador * head = NULL;
	iniciarListaEntrenador(head);
	// Obtener de config los arrays y cargar las caracteristicas de los entrenadores a mi head
	//config_get_array_value(config,"POSICION_ENTRENADORES");
	//config_get_array_value(config,"POKEMON_ENTRENADORES");
	//config_get_array_value(config,"OBJETIVOS_ENTRENADORES");
	cantidadEntrenadores = length(head);

	// Hilos con semáforos mutex
	pthread_t tid[cantidadEntrenadores];
		int i = 0;
	    int err;
	    if (pthread_mutex_init(&lock, NULL) != 0){
	        printf("\n mutex init failed\n");
	        return 1;
	    }
	    while(i < cantidadEntrenadores){
	        err = pthread_create(&(tid[i]), NULL, &doSomeThing, NULL);
	        if (err != 0)
	            printf("\ncan't create thread :[%s]", strerror(err));
	        i++;
	    }
	    for(i=0;i<cantidadEntrenadores;i++){
	    pthread_join(tid[i], NULL);
	    pthread_mutex_destroy(&lock);
	    }

}



void iniciarListaEntrenador(entrenador * head){
	head = (entrenador *) malloc(sizeof(entrenador));
		if (head == NULL) {
		    printf("No se pudo crear la lista\n");
		}
}

// Largo de la lista de entrenadores
int length(entrenador * head){
	int contador = 0;
	while(head!=NULL){
		contador++;
		head = head->next;
	}
	return contador;
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


