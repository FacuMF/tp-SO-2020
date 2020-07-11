#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	//pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);

	pthread_create(&thread, NULL, (void*) enviar_requests_pokemones, NULL);

	pthread_create(&thread, NULL, (void*) iniciar_conexion_con_gameboy, NULL);

	iniciar_planificador();

	sleep(1000); //TT
	finalizar_team();
}

void iniciar_team(char*argumentos_iniciales[]) {
	inicializar_listas();

	// Config
	iniciar_config_team(argumentos_iniciales[1]);

	// Logger
	iniciar_logger_team();

	// Planficador
	obtener_variables_planificacion();
	elegir_algoritmo();
	pthread_mutex_init(&cpu_disponible, NULL);
	pthread_mutex_init(&entrenadores_ready, NULL);
}

void finalizar_team() {
	terminar_logger(logger);
	config_destroy(config);
}

