#include "team.h"

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while (1) {
		sem_wait(&(entrenador->sem_est));

		if (entrenador->catch_pendiente != NULL) {
			log_debug(logger, "Entrenador despierto: Posicion %i %i",
					entrenador->posicion[0], entrenador->posicion[1]);
			cazar_pokemon(entrenador);
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_ESPERANDO;
			sem_post(&cpu_disponible);
			log_debug(logger, "Entrenador dormido: Posicion %i %i",
					entrenador->posicion[0], entrenador->posicion[1]);
		} else if (objetivo_propio_cumplido(entrenador)) {
			actualizar_timestamp(entrenador);
			sem_post(&cpu_disponible);
			break; // Pasa a exit
		} else if (tiene_espacio_disponible(entrenador)) {
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_NORMAL;
			sem_post(&cpu_disponible);
			log_debug(logger, "Entrenador blockeado normal: Posicion %i %i",
					entrenador->posicion[0], entrenador->posicion[1]);
		} else if (!tiene_espacio_disponible(entrenador)) {
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_DEADLOCK;
			sem_post(&cpu_disponible);
			log_debug(logger, "Entrenador esperando deadlock: Posicion %i %i",
					entrenador->posicion[0], entrenador->posicion[1]);
		}
		//TODO: Agregar else if para cuando tenga que hacer el deadlock

	}
	log_debug(logger, "Entrenador en exit: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);
	entrenador->estado = EXIT;
}

void cazar_pokemon(t_entrenador * entrenador) {
	int ciclos_esta_corrida = 0;

	while (entrenador->ciclos_cpu_restantes > 0) {
		// Chequeo si es necesario bloquearse
		switch (algoritmo_elegido) {
		case A_RR:
			if (ciclos_esta_corrida >= quantum) {
				ciclos_esta_corrida = 0;
				bloquear_entrenador(entrenador);
			}
			break;
		case A_SJFCD:
			if (desalojar) {
				entrenador->estimacion_rafaga = constante_estimacion
						* ciclos_esta_corrida
						+ (1 - constante_estimacion)
								* entrenador->estimacion_rafaga;
				desalojar = 0;
				bloquear_entrenador(entrenador);
			}
			break;
		default:
			break;
		}
		sleep(retardo_ciclo_cpu);
		if (entrenador->ciclos_cpu_restantes > 1) {
			mover_entrenador(entrenador);
			log_info(logger,
					"Entrenador movido a posicon %d %d",
					entrenador->posicion[0], entrenador->posicion[1]);
		} else {
			log_info(logger,
					"Catch efectuado en posicon %d %d para %s",
					entrenador->posicion[0], entrenador->posicion[1],
					entrenador->catch_pendiente->pokemon);
			pthread_create(&thread, NULL, (void*) enviar_mensaje_catch,
					entrenador);
		}

		ciclos_esta_corrida++;
		entrenador->ciclos_cpu_restantes--;
		entrenador->estimacion_rafaga--;
		log_debug(logger,"Ciclos restantes: %d",entrenador->ciclos_cpu_restantes);

	}
	entrenador->estimacion_rafaga = constante_estimacion * ciclos_esta_corrida
			+ (1 - constante_estimacion) * entrenador->estimacion_rafaga;
}

void mover_entrenador(t_entrenador * entrenador) {
	if (entrenador->catch_pendiente->posx > entrenador->posicion[0]) {
		entrenador->posicion[0]++;
	} else if (entrenador->catch_pendiente->posx < entrenador->posicion[0]) {
		entrenador->posicion[0]--;
	} else if (entrenador->catch_pendiente->posy > entrenador->posicion[1]) {
		entrenador->posicion[1]++;
	} else if (entrenador->catch_pendiente->posy < entrenador->posicion[1]) {
		entrenador->posicion[1]--;
	}
}

// Cambio de estado

void ejecutar_entrenador(t_entrenador * entrenador) {
	entrenador->estado = EXEC;
	sem_post(&(entrenador->sem_est));
}

void bloquear_entrenador(t_entrenador * entrenador) {
	actualizar_timestamp(entrenador);
	entrenador->estado = READY;
	sem_post(&cpu_disponible);
	sem_wait(&(entrenador->sem_est));
	log_debug(logger, "Entrenador despierto: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);
}

void actualizar_timestamp(t_entrenador * entrenador) {
	struct timeval tval;
	gettimeofday(&tval, NULL);
	entrenador->ultima_modificacion = tval;
}

void preparar_entrenador(t_entrenador * entrenador,
		t_appeared_pokemon * mensaje_appeared) {
	// Setear status = ready, calcular y llenar ciclos de cpu
	// Guardarle el mensaje de caught.
	entrenador->catch_pendiente = de_appeared_a_catch(mensaje_appeared);
	entrenador->ciclos_cpu_restantes = distancia(entrenador,
			mensaje_appeared->posx, mensaje_appeared->posy) + 1; //+1 por envio de mensaje

	actualizar_timestamp(entrenador);

	entrenador->estado = READY;

	log_trace(logger, "Nuevo ent en ready: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);

}

void planificar_entrenador(t_entrenador * entrenador,
		t_appeared_pokemon * mensaje_appeared) {
	preparar_entrenador(entrenador, mensaje_appeared);

	sem_post(&entrenadores_ready);

	if (algoritmo_elegido == A_SJFCD)
		sem_post(&cpu_disponible);
}

// Auxiliares
int objetivo_propio_cumplido(t_entrenador *entrenador) {
	t_list *pokemones_por_capturar = entrenador->pokemones_por_capturar;
	t_list * pokemones_capturados = entrenador->pokemones_capturados;

	bool fue_capturado(void *element) {
		char * pokemon = element;
		int repeticiones_en_capturados = cantidad_repeticiones_en_lista(
				pokemones_capturados, pokemon);
		int repeticiones_en_por_capturar = cantidad_repeticiones_en_lista(
				pokemones_por_capturar, pokemon);

		return repeticiones_en_capturados == repeticiones_en_por_capturar;
	}
	return list_all_satisfy(pokemones_por_capturar, fue_capturado);
}

