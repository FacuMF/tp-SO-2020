#include "team.h"


t_list * obtener_pokemones_necesitados_sin_repetidos(){
	t_list * pokemones_sin_repetidos = list_create();

	void agrego_si_no_existe(void * element){
		char*pokemon = element;

		if(!pokemon_en_lista(pokemones_sin_repetidos,pokemon)){
			list_add(pokemones_sin_repetidos,pokemon);
		}
	}

	pthread_mutex_lock(&mutex_pokemones_necesitados);
	list_iterate(pokemones_necesitados,agrego_si_no_existe);
	pthread_mutex_unlock(&mutex_pokemones_necesitados);

	return pokemones_sin_repetidos;
}

// AUXILIARES
void eliminar_si_esta(t_list * lista, char * pokemon){
	bool es_un_repetido_aux(void*elemento){
		char *pokemon_a_comparar = elemento;
		return !strcasecmp(pokemon_a_comparar,pokemon);
	}
	list_remove_by_condition(lista,es_un_repetido_aux);
}

int requiero_pokemon(char * pokemon){
	pthread_mutex_lock(&mutex_pokemones_necesitados);
	int necesitados = cantidad_repeticiones_en_lista(pokemones_necesitados, pokemon);
	necesitados -= cantidad_entrenadores_buscando_pokemon(pokemon);
	pthread_mutex_unlock(&mutex_pokemones_necesitados);

	int result = necesitados>0;

	log_debug(logger,"Requiero mensaje = %i",result);

	return result;
}

int pokemon_en_lista(t_list * lista_pokemones,char * pokemon){
	bool pokemon_igual_aux(void*elemento){
		char * pokemon_a_comparar =elemento;
		return !strcasecmp(pokemon_a_comparar,pokemon);
	}
	int result = list_any_satisfy(lista_pokemones,pokemon_igual_aux);

	log_debug(logger,"Pokemon en lista = %i",result);

	return result;
}

int cantidad_repeticiones_en_lista(t_list * lista_pokemones, char *pokemon ){
	bool es_un_repetido_aux(void*elemento){
		char *pokemon_a_comparar = elemento;
		return !strcasecmp(pokemon_a_comparar,pokemon);
	}
	return list_count_satisfying(lista_pokemones,es_un_repetido_aux);

}
