#include "team.h"

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while (1) {
		sem_wait(&(entrenador->sem_est));
		log_entrenador_ser(entrenador, "activo");
		if (entrenador->catch_pendiente != NULL) {
			log_entrenador_ser(entrenador, "despierto");
			cazar_pokemon(entrenador);
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_ESPERANDO;
			sem_post(&cpu_disponible);
			log_entrenador_ser(entrenador, "dormido");
		} else if (entrenador->deadlock != NULL) {
			log_entrenador_ser(entrenador, "despierto");
			realizar_intercambio(entrenador);

			entrenador->estado = BLOCKED_DEADLOCK;
			actualizar_timestamp(entrenador);
			sem_post(&resolver_deadlock);
			sem_post(&cpu_disponible);
			log_entrenador_ser(entrenador, "dormido");
		} else if (objetivo_propio_cumplido(entrenador)) {
			actualizar_timestamp(entrenador);
			break; // Pasa a exit
		} else if (tiene_espacio_disponible(entrenador)) {
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_NORMAL;
			log_entrenador_ser(entrenador, "bloqueado normal");
		} else if (!tiene_espacio_disponible(entrenador)) {
			actualizar_timestamp(entrenador);
			if(entrenador->estado == BLOCKED_DEADLOCK)
				sem_post(&sincro_deadlock);
			entrenador->estado = BLOCKED_DEADLOCK;
			log_entrenador_ser(entrenador, "blockeado deadlock");
		}

	}
	log_entrenador_ser(entrenador, "pasa a exit");
	int postear = entrenador->estado== BLOCKED_DEADLOCK;
	entrenador->estado = EXIT;
	if(postear)
		sem_post(&sincro_deadlock);
}

void log_entrenador_ser(t_entrenador * entrenador, char * estado) {

	log_debug(logger, "E%c %s. Posicion %i %i", entrenador->id, estado,
			entrenador->posicion[0], entrenador->posicion[1]);
}

void realizar_intercambio(t_entrenador * entrenador) {
	int ciclos_esta_corrida = 0;
	sem_wait(&cpu_disponible_sjf);
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
				log_entrenador_ser(entrenador, "DESALOJADO");
				entrenador->estimacion_rafaga = constante_estimacion
						* ciclos_esta_corrida
						+ (1 - constante_estimacion)
								* entrenador->estimacion_rafaga;
				ciclos_esta_corrida = 0;
				desalojar = 0;
				bloquear_entrenador(entrenador);
			}
			break;
		default:
			break;
		}

		// Ejecuta
		ciclos_esta_corrida++;
		entrenador->ciclos_cpu_totales++;
		entrenador->estimacion_rafaga--;

		sleep(retardo_ciclo_cpu);

		if (entrenador->ciclos_cpu_restantes > 5) {
			mover_entrenador(entrenador);
		} else if (entrenador->ciclos_cpu_restantes > 1) {
			log_trace(logger, "Intercambio en proceso");
		} else {
			list_add(entrenador->pokemones_capturados,
					entrenador->deadlock->pokemon_recibir);
			list_add(entrenador->deadlock->capturados_ep,
					entrenador->deadlock->pokemon_dar);
			eliminar_si_esta(entrenador->pokemones_capturados,
					entrenador->deadlock->pokemon_dar);
			eliminar_si_esta(entrenador->deadlock->capturados_ep,
					entrenador->deadlock->pokemon_recibir);

			/*log_info(logger,
			 "Intercambio efectuado entre E%c y E%c en posicon %d %d para %s por %s",
					entrenador->id, entrenador->deadlock->id,
			 entrenador->posicion[0], entrenador->posicion[1],
			 entrenador->deadlock->pokemon_dar,entrenador->deadlock->pokemon_recibir);
			 */
			log_info(logger, "Intercambio efectuado entre E%c y E%c",
					entrenador->id, entrenador->deadlock->id);

			free(entrenador->deadlock);
			entrenador->deadlock = NULL;
		}

		entrenador->ciclos_cpu_restantes--;

		log_trace(logger, "Ciclos restantes: %d",
				entrenador->ciclos_cpu_restantes);

	}
	entrenador->estimacion_rafaga = constante_estimacion * ciclos_esta_corrida
			+ (1 - constante_estimacion) * entrenador->estimacion_rafaga;
	sem_post(&cpu_disponible_sjf);
}

void cazar_pokemon(t_entrenador * entrenador) {
	int ciclos_esta_corrida = 0;
	log_entrenador_ser(entrenador, "espera para cazar");
	sem_wait(&cpu_disponible_sjf);
	log_entrenador_ser(entrenador, "cazando");
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
				log_entrenador_ser(entrenador, "DESALOJADO");
				entrenador->estimacion_rafaga = constante_estimacion
						* ciclos_esta_corrida
						+ (1 - constante_estimacion)
								* entrenador->estimacion_rafaga;
				desalojar = 0;
				ciclos_esta_corrida = 0;
				bloquear_entrenador(entrenador);
			}

			break;
		default:
			break;
		}

		// Ejecuta
		ciclos_esta_corrida++;
		entrenador->ciclos_cpu_totales++;
		entrenador->estimacion_rafaga--;

		sleep(retardo_ciclo_cpu);
		if (entrenador->ciclos_cpu_restantes > 1) {
			mover_entrenador(entrenador);
		} else {
			log_info(logger, "Catch efectuado en posicon %d %d para %s",
					entrenador->posicion[0], entrenador->posicion[1],
					entrenador->catch_pendiente->pokemon);

			pthread_mutex_lock(&mutex_hilos);
			pthread_create(&thread, NULL, (void*) enviar_mensaje_catch,
					entrenador);
			pthread_detach(thread);
			pthread_mutex_unlock(&mutex_hilos);

		}

		entrenador->ciclos_cpu_restantes--;
		log_trace(logger, "Ciclos restantes: %d",
				entrenador->ciclos_cpu_restantes);

	}

	entrenador->estimacion_rafaga = constante_estimacion * ciclos_esta_corrida
			+ (1 - constante_estimacion) * entrenador->estimacion_rafaga;
	sem_post(&cpu_disponible_sjf);
}

void mover_entrenador(t_entrenador * entrenador) {
	int objX, objY;
	if (entrenador->catch_pendiente != NULL) {
		objX = entrenador->catch_pendiente->posx;
		objY = entrenador->catch_pendiente->posy;
	} else {
		objX = entrenador->deadlock->posx;
		objY = entrenador->deadlock->posy;
	}

	if (objX > entrenador->posicion[0]) {
		entrenador->posicion[0]++;
	} else if (objX < entrenador->posicion[0]) {
		entrenador->posicion[0]--;
	} else if (objY > entrenador->posicion[1]) {
		entrenador->posicion[1]++;
	} else if (objY < entrenador->posicion[1]) {
		entrenador->posicion[1]--;
	}
	log_info(logger, "Entrenador %c movido a posicon %d %d", entrenador->id,
			entrenador->posicion[0], entrenador->posicion[1]);
}

// Cambio de estado

void ejecutar_entrenador(t_entrenador * entrenador) {
	cambios_contexto_totales++;
	entrenador->estado = EXEC;
	sem_post(&(entrenador->sem_est));
}

void bloquear_entrenador(t_entrenador * entrenador) {
	actualizar_timestamp(entrenador);
	entrenador->estado = READY;
	sem_post(&cpu_disponible);
	sem_post(&entrenadores_ready);
	log_entrenador_ser(entrenador, "dormido");
	sem_post(&cpu_disponible_sjf);

	sem_wait(&(entrenador->sem_est));
	sem_wait(&cpu_disponible_sjf);
	log_entrenador_ser(entrenador, "despierto");
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

	liberar_mensaje_appeared_pokemon(mensaje_appeared);

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

