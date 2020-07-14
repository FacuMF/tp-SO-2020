#include "team.h"

void iniciar_deteccion_deadlock(){
	while(deadlocks_pendientes()){
		log_warning(logger,"Entra en DETECCION DEADLOCK");
		int deadlock_en_curso = 1;
		t_list * entrenadores_en_deadlock = obtener_entrenadores_en_estado(BLOCKED_DEADLOCK,
					head_entrenadores);

		t_entrenador * entrenador_activo = list_get(entrenadores_en_deadlock,0);
		t_entrenador * entrenador_pasivo = NULL;

		while(deadlock_en_curso){// Ciclo de un deadlock
			log_warning(logger,"deadlock en curso");
			t_list * pok_faltantes = obtener_pokemones_faltantes(entrenador_activo);
			t_list * pok_sobrantes = obtener_pokemones_sobrantes(entrenador_activo);
			char * pokemon_a_recibir = list_get(pok_faltantes,0);
			char * pokemon_a_dar= list_get(pok_sobrantes,0);
			t_list * posibles_pasivos = entrenadores_con_pokemon_sobrante(pokemon_a_recibir);

			log_trace(logger,"Posibles pasivos obtenidos");
			t_entrenador * entrenador_ideal = obtener_entrenador_ideal(posibles_pasivos,entrenador_activo);
			if(entrenador_ideal != NULL){
				log_trace(logger,"es intercambio ideal");
				pokemon_a_dar = obtener_pokemon_a_dar(pok_sobrantes,entrenador_ideal);
				entrenador_pasivo = entrenador_ideal;
				deadlocks_totales++;
				deadlock_en_curso = 0;
			}else
				entrenador_pasivo = list_get(posibles_pasivos,0);

			log_trace(logger,"Pasivo elegido");
			t_deadlock * deadlock = malloc(sizeof(t_deadlock));
			deadlock->capturados_ep = entrenador_pasivo->pokemones_capturados;
			deadlock->posx = entrenador_pasivo->posicion[0];
			deadlock->posy = entrenador_pasivo->posicion[1];
			deadlock->pokemon_dar = pokemon_a_dar;
			deadlock->pokemon_recibir= pokemon_a_recibir;

			log_trace(logger,"Estructura deadlock llena");

			planificar_entrenador_deadlock(entrenador_activo,deadlock);
			log_trace(logger,"Entrenador planificado");
			sem_wait(&resolver_deadlock);
			sem_post(&(entrenador_activo->sem_est));
			sem_post(&(entrenador_pasivo->sem_est));
			sleep(2);// TODO: TBR
			entrenador_activo = entrenador_pasivo;

		}// -- Fin ciclo --
	}
}

void planificar_entrenador_deadlock(t_entrenador * entrenador,t_deadlock * deadlock){
	entrenador->deadlock = deadlock;
	entrenador->ciclos_cpu_restantes = distancia(entrenador,
				deadlock->posx, deadlock->posy) + 5;
	actualizar_timestamp(entrenador);

	entrenador->estado = READY;

	log_trace(logger, "Nuevo ent en ready: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);

	sem_post(&entrenadores_ready);

	if (algoritmo_elegido == A_SJFCD)
		sem_post(&cpu_disponible);
}

char * obtener_pokemon_a_dar(t_list * pok_sobrantes,t_entrenador * entrenador_ideal){
	bool es_sobrante(void * element){
		char * pokemon = element;
		return pokemon_en_lista(pok_sobrantes,pokemon);
	}
	return list_find(obtener_pokemones_faltantes(entrenador_ideal),es_sobrante);
}

t_entrenador * obtener_entrenador_ideal(t_list * posibles_pasivos, t_entrenador * entrenador){
	t_list * sobrantes_disponibles = obtener_pokemones_sobrantes(entrenador);
	bool es_ideal(void*element){
		t_entrenador * entrenador_posible = element;
		return obtener_pokemon_a_dar(sobrantes_disponibles,entrenador_posible)!=NULL;
	}
	// nota: para optimizar se podria buscar todos los ideales y buscar el mas cercano en vez del primero.
	// nota2: tambien aquel que sea mayor (sus sobrantes que necesito+ mis sobrantes q necesita)
	// TODO: Sortear por distancia p optimizacion
	return list_find(posibles_pasivos, es_ideal);
}


t_list * entrenadores_con_pokemon_sobrante(char * pokemon){
	bool le_sobra_pokemon(void*element){
		t_entrenador * entrenador = element;
		t_list * sobrantes = obtener_pokemones_sobrantes(entrenador);
		return pokemon_en_lista(sobrantes,pokemon);
	}

	return list_filter(obtener_entrenadores_en_estado(BLOCKED_DEADLOCK,
			head_entrenadores), le_sobra_pokemon);
}

int deadlocks_pendientes(){
	t_list * entrenadores_en_deadlock = obtener_entrenadores_en_estado(BLOCKED_DEADLOCK,
						head_entrenadores);

	void * entrenador = list_get(entrenadores_en_deadlock,0);
	return entrenador != NULL;
}

t_list * obtener_pokemones_faltantes(t_entrenador * entrenador){
	bool falta_en_capturados(void * element){
		char*pokemon = element;
		return cantidad_repeticiones_en_lista(entrenador->pokemones_capturados,pokemon)<cantidad_repeticiones_en_lista(entrenador->pokemones_por_capturar,pokemon);
	}
	return list_filter(entrenador->pokemones_por_capturar, falta_en_capturados);
}

t_list * obtener_pokemones_sobrantes(t_entrenador * entrenador){
	bool sobrante_en_capturados(void * element){
			char*pokemon = element;
			return cantidad_repeticiones_en_lista(entrenador->pokemones_capturados,pokemon)>cantidad_repeticiones_en_lista(entrenador->pokemones_por_capturar,pokemon);
	}
	return list_filter(entrenador->pokemones_capturados, sobrante_en_capturados);
}