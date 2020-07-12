#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);

	pthread_create(&thread, NULL, (void*) enviar_requests_pokemones, NULL);

	pthread_create(&thread, NULL, (void*) iniciar_conexion_con_gameboy, NULL);

	iniciar_planificador();

	sleep(1000); //TT
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
	sem_init(&entrenadores_ready,1,1);

	pthread_mutex_init(&chequeo_sem_suscrip, NULL);
	sem_init(&suscripcion,1,0);

	// Data Compartida
	inicializar_listas();
}

void finalizar_team() {
	// TODO: Destroy todos los semaforos
	// TODO: Finalizar todos los hilos? Ya deberian estar todos terminados

	terminar_logger(logger);
	config_destroy(config);
}
