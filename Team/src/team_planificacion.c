#include "team.h"

void iniciar_planificador() {
	list_iterate(head_entrenadores, lanzar_hilo_entrenador);

	t_entrenador * entrenador_en_exec = NULL;

	sem_wait(&cpu_disponible);
	while (1) {
		while (!entrenadores_en_ready()) {
			sem_wait(&entrenadores_ready);
		}
		log_trace(logger, "Planificador corto plazo en curso");
		t_entrenador * entrenador = obtener_entrenador_a_planificar();

		if(entrenador_en_exec!=NULL)
			log_debug(logger, "EnNu: %c, r%lf. EnEx: %c, r%lf", entrenador->id,entrenador->estimacion_rafaga, entrenador_en_exec->id,entrenador_en_exec->estimacion_rafaga);

		if (algoritmo_elegido == A_SJFCD && entrenador_en_exec != NULL) {
			if (entrenador_en_exec->ciclos_cpu_restantes == 0) {
				log_info(logger,
						"Cambio de entrenador sin desalojo por finalizacion de ciclo de CPU");
				entrenador_en_exec = entrenador;
				ejecutar_entrenador(entrenador);
			} else if (entrenador->estimacion_rafaga
					< entrenador_en_exec->estimacion_rafaga) {
				log_info(logger,
						"Cambio de entrenador con desalojo por menor rafaga.");
				desalojar = 1;
				entrenador_en_exec = entrenador;

				ejecutar_entrenador(entrenador);
			} else {
				if (entrenador->estado != EXEC
						&& entrenador->estimacion_rafaga
								== entrenador_en_exec->estimacion_rafaga){
					log_debug(logger, "Entrenador en exec: %c, r%lf.",entrenador_en_exec->id,entrenador_en_exec->estimacion_rafaga);
					ejecutar_entrenador(entrenador_en_exec);
				}
			}
		} else {
			if (entrenador_en_exec != NULL) {
				switch (algoritmo_elegido) {
				case A_FIFO:
					log_info(logger,
							"Cambio de entrenador por finalizacion de sus ciclos de cpu");
					break;
				case A_RR:
					if (entrenador_en_exec->ciclos_cpu_restantes == 0)
						log_info(logger,
								"Cambio de entrenador por finalizacion de sus ciclos de cpu");
					else
						log_info(logger,
								"Cambio de entrenador por finalizacion de quantum");
					break;
				case A_SJFSD:
					log_info(logger,
							"Cambio de entrenador por finalizacion de sus ciclos de CPU");
					break;
				}
			}
			entrenador_en_exec = entrenador;
			ejecutar_entrenador(entrenador);
		}
		sem_wait(&cpu_disponible);
	}
}

t_entrenador * obtener_entrenador_a_planificar() {
	t_entrenador * entrenador;

	t_list * entrenadores_en_ready = obtener_entrenadores_en_estado(READY,
			head_entrenadores);

	switch (algoritmo_elegido) {
	case A_FIFO:
	case A_RR:
		entrenador = obtener_entrenador_fifo(entrenadores_en_ready);
		break;
	case A_SJFSD:
	case A_SJFCD:
		entrenador = obtener_entrenador_sjf(entrenadores_en_ready);
		break;
	}
	return entrenador;
}

t_entrenador * obtener_entrenador_fifo(t_list * entrenadores) {
	//Sort por time menor y obtener el primero

	bool menor_tiempo(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return timeval_subtract(&entrenador_1->ultima_modificacion,
				&entrenador_2->ultima_modificacion);
	}

	t_list * entrenador_antes_modif = list_sorted(entrenadores, menor_tiempo);

	t_entrenador * entrenador_menor_tiempo = list_get(entrenador_antes_modif,
			0);

	return entrenador_menor_tiempo;
}

t_entrenador * obtener_entrenador_sjf(t_list * entrenadores) {
	//Sort por estimacion menor y obtener el primero

	bool menor_estimacion(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return entrenador_1->estimacion_rafaga < entrenador_1->estimacion_rafaga;
	}

	t_list * entrenadores_menor_est = list_sorted(entrenadores,
			menor_estimacion);

	t_entrenador * entrenador_menor_est = list_get(entrenadores_menor_est, 0);

	return entrenador_menor_est;
}

// AUXILIARES
int objetivo_global_completo() {
	pthread_mutex_lock(&mutex_pokemones_necesitados);
	int result = list_get(pokemones_necesitados, 0) == NULL;
	pthread_mutex_unlock(&mutex_pokemones_necesitados);

	return result;
}

int entrenadores_en_ready() {
	t_list * entrenadores_en_ready = obtener_entrenadores_en_estado(READY,
			head_entrenadores);
	return !list_is_empty(entrenadores_en_ready);
}

int timeval_subtract(x, y)
	struct timeval *x, *y; {
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

