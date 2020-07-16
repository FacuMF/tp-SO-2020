#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	//pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);

	pthread_create(&thread, NULL, (void*) enviar_requests_pokemones, NULL);

	pthread_create(&thread, NULL, (void*) iniciar_conexion_con_gameboy, NULL);

	pthread_create(&thread, NULL, (void*) iniciar_planificador, NULL);

	while (!objetivo_global_completo()) {
		sem_wait(&verificar_objetivo_global);
	}

	log_info(logger,"Inicio algoritmo de deteccion de deadlock");
	iniciar_deteccion_deadlock();

	finalizar_team();
}

void iniciar_team(char*argumentos_iniciales[]){
	// TODO: Revisar todos los hilos y hacer detach/join donde haga falta

	// Config
	iniciar_config_team(argumentos_iniciales[1]);

	// Logger
	iniciar_logger_team();

	// Planficador
	obtener_variables_planificacion();
	elegir_algoritmo();

	sem_init(&cpu_disponible,1,1);
	sem_init(&cpu_disponible_sjf,1,1);
	sem_init(&entrenadores_ready,1,1);
	sem_init(&verificar_objetivo_global,1,0);

	pthread_mutex_init(&chequeo_sem_suscrip, NULL);
	sem_init(&suscripcion,1,0);

	sem_init(&resolver_deadlock,1,0);

	pthread_mutex_init(&manejar_mensaje, NULL);

	pthread_mutex_init(&mutex_pokemones_necesitados, NULL);

	// Data Compartida
	inicializar_listas();
}

void finalizar_team() {
	// TODO: Destroy todos los semaforos
	// TODO: Finalizar todos los hilos? Ya deberian estar todos terminados

	terminar_logger(logger);
	config_destroy(config);
}
