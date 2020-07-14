#include "team.h"

void iniciar_deteccion_deadlock(){
	while(deadlocks_pendientes()){
		int deadlock_en_curso = 1;
		t_list * entrenadores_en_deadlock = obtener_entrenadores_en_estado(BLOCKED_DEADLOCK,
					head_entrenadores);

		// TODO: sortear dependiendo del algoritmo?
		t_entrenador * entrenador_activo = list_get(entrenadores_en_deadlock,0);
		t_entrenador * entrenador_pasivo = NULL;

		while(deadlock_en_curso){// Ciclo de un deadlock
			sem_wait(&resolver_deadlock);

			t_list * pok_faltantes = obtener_pokemones_faltantes(entrenador_activo);
			t_list * pok_sobrantes = obtener_pokemones_sobrantes(entrenador_activo);
			char * pokemon_a_intercambiar = list_get(pok_faltantes,0);
			t_list * posibles_pasivos = entrenadores_con_pokemon_sobrante(pokemon_a_intercambiar);

			t_entrenador * entrenador_ideal = obtener_entrenador_ideal(posibles_pasivos,pokemon_a_intercambiar);
			if(entrenador_ideal != NULL){
				entrenador_pasivo = entrenador_ideal;
				deadlocks_totales++;
				deadlock_en_curso = 0;
			}else
				entrenador_pasivo = list_get(posibles_pasivos,0);

			// TODO: planificar entrenador con estructura
				// ent2 | pokemon_a_obtener | pokemon_a_dar
			// TODO: Revisar si hay algun otro que puedan cambiar?
		}// -- Fin ciclo --
		sem_post(&resolver_deadlock);
	}

}

t_entrenador * obtener_entrenador_ideal(t_list * posibles_pasivos, char*pokemon){
	bool le_falta_pokemon(void*element){
		t_entrenador * entrenador = element;
		t_list * faltantes = obtener_pokemones_faltantes(entrenador);
		return pokemon_en_lista(faltantes,pokemon);
	}
	// nota: para optimizar se podria buscar todos los ideales y buscar el mas cercano en vez del primero.
	// nota2: tambien aquel que sea mayor (sus sobrantes que necesito+ mis sobrantes q necesita)
	return list_find(posibles_pasivos, le_falta_pokemon);
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
		return !pokemon_en_lista(entrenador->pokemones_capturados,pokemon);
	}
	return list_filter(entrenador->pokemones_por_capturar, falta_en_capturados);
}

t_list * obtener_pokemones_sobrantes(t_entrenador * entrenador){
	bool falta_en_capturados(void * element){
			char*pokemon = element;
			return !pokemon_en_lista(entrenador->pokemones_por_capturar,pokemon);
		}
		return list_filter(entrenador->pokemones_capturados, falta_en_capturados);
}
