#include "team.h"

// OBJETIVO GLOBAL

t_list* formar_objetivo(t_list * pokemones_repetidos) {
	t_list * objetivo_global = list_create();
	void agrego_si_no_existe_aux(void *elemento) { // USO INNER FUNCTIONS TODO: pasar a readme
		agrego_si_no_existe(objetivo_global, elemento);
	}

	list_iterate(pokemones_repetidos, agrego_si_no_existe_aux);

	log_trace(logger, " ---- Objetivos Formados ----");
	list_iterate(objetivo_global, mostrar_objetivo);

	return objetivo_global;
}

void agrego_si_no_existe(t_list * objetivo_global, void *nombrePokemon) {
	bool _yaExiste(void *inputObjetivo) {
		t_objetivo *cadaObjetivo = inputObjetivo;
		return !strcmp(cadaObjetivo->pokemon, nombrePokemon);
	}

	t_objetivo *objetivo = list_find(objetivo_global, _yaExiste);

	if (objetivo != NULL) {
		objetivo->cantidad++;
	} else {
		t_objetivo * nuevo_objetivo = malloc(sizeof(t_objetivo));
		nuevo_objetivo->cantidad = 1;
		nuevo_objetivo->pokemon = nombrePokemon;
		list_add(objetivo_global, nuevo_objetivo);
	}

}



void mostrar_objetivo(void *elemento) {
	log_trace(logger, "Data de objetivo!");

	t_objetivo *objetivo = elemento;
	log_trace(logger, "objetivo: %s", objetivo->pokemon);
	log_trace(logger, "objetivo: %i", objetivo->cantidad);

}
int objetivo_cumplido(t_entrenador *entrenador){
	return list_is_empty(entrenador->pokemones_por_capturar);
}
