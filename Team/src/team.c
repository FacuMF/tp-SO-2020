#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	//pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);
	//pthread_detach(thread);

	pthread_create(&thread, NULL, (void*) enviar_requests_pokemones, NULL);
	pthread_detach(thread);

	pthread_create(&thread, NULL, (void*) iniciar_conexion_con_gameboy, NULL);
	pthread_detach(thread);

	pthread_create(&thread, NULL, (void*) iniciar_planificador, NULL);
	pthread_detach(thread);

	while (!objetivo_global_completo()) {
		sem_wait(&verificar_objetivo_global);
	}

	log_info(logger,"Inicio algoritmo de deteccion de deadlock");
	iniciar_deteccion_deadlock();

	log_metricas();

	finalizar_team();
}

void log_metricas(){
	log_info(logger,"Ciclos CPU totales: %d", obtener_ciclos_cpu_totales());
	list_iterate(head_entrenadores,log_cpu_entrenador);
	log_info(logger,"Cantidad de cambios de contexto: %d",cambios_contexto_totales);
	log_info(logger, "Deadlocks Encontrados y Resueltos: %d",deadlocks_resueltos);

}

void log_cpu_entrenador(void * element){
	t_entrenador * entrenador = element;
	log_info(logger,"Ciclos Totales E%c = %d",entrenador->id,entrenador->ciclos_cpu_totales);
}

int obtener_ciclos_cpu_totales(){
	int total = 0;
	void sumar_ciclos(void * element){
		t_entrenador * entrenador = element;
		total += entrenador->ciclos_cpu_totales;
	}

	list_iterate(head_entrenadores,sumar_ciclos);
	return total;
}

void iniciar_team(char*argumentos_iniciales[]){
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
	sem_init(&sincro_deadlock,1,0);

	pthread_mutex_init(&manejar_mensaje, NULL);

	pthread_mutex_init(&mutex_pokemones_necesitados, NULL);
	pthread_mutex_init(&mutex_ids_mensajes, NULL);

	// Data Compartida
	inicializar_listas();
}

void finalizar_team() {

	list_destroy(ids_mensajes_utiles);
	list_destroy(appeared_a_asignar);
	list_destroy(appeared_auxiliares);
	list_destroy(pokemones_recibidos);
	list_destroy(pokemones_necesitados);

	list_destroy_and_destroy_elements(head_entrenadores,liberar_entrenador);


	pthread_mutex_destroy(&chequeo_sem_suscrip);
	pthread_mutex_destroy(&manejar_mensaje);
	pthread_mutex_destroy(&mutex_pokemones_necesitados);
	pthread_mutex_destroy(&mutex_ids_mensajes);

	sem_close(&entrenadores_ready);
	sem_close(&cpu_disponible);
	sem_close(&verificar_objetivo_global);
	sem_close(&suscripcion);
	sem_close(&resolver_deadlock);
	sem_close(&cpu_disponible_sjf);
	sem_close(&sincro_deadlock);


	terminar_logger(logger);
	config_destroy(config);
}

void liberar_entrenador(void * element){
	t_entrenador * entrenador = element;

	//char id;
	sem_close(&entrenador->sem_est);

	//int ciclos_cpu_totales;
	free(entrenador->posicion);
	list_destroy_and_destroy_elements(entrenador->pokemones_capturados,free);
	list_destroy(entrenador->pokemones_por_capturar);
	free(entrenador);
}
