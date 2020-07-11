#include "team.h"

t_list* obtener_pokemones_por_capturar() {
	t_list * pokemones_por_capturar = list_create();

	void llenar_lista_con_sus_pokemones(void * element) {
		t_entrenador *entrenador = element;
		t_list * lista_a_agregar = list_duplicate(entrenador->pokemones_por_capturar);
		list_add_all(pokemones_por_capturar,lista_a_agregar);
	}

	list_iterate(head_entrenadores, llenar_lista_con_sus_pokemones);

	//log_debug(logger, "Kokemones a capturar  obtenidos");
	//list_iterate(pokemones_por_capturar, mostrar_kokemon);

	return pokemones_por_capturar;
}

t_list * obtener_pokemones_capturados(){
	t_list * pokemones_capturados = list_create();

	void llenar_lista_con_sus_pokemones(void * element) {
		t_entrenador *entrenador = element;
		t_list * lista_a_agregar = list_duplicate(entrenador->pokemones_capturados);
		list_add_all(pokemones_capturados,lista_a_agregar);
	}

	list_iterate(head_entrenadores, llenar_lista_con_sus_pokemones);

	//log_debug(logger, "Kokemones capturados obtenidos");
	//list_iterate(pokemones_capturados, mostrar_kokemon);

	return pokemones_capturados;
}

t_list * obtener_pokemones_necesitados(){
	t_list * pokemones_capturados = obtener_pokemones_capturados();
	t_list * pokemones_por_capturar = obtener_pokemones_por_capturar();
	t_list * pokemones_necesitados = list_duplicate(pokemones_por_capturar);

	char * pokemon_analizado;

	bool es_igual_a_analizado(void * element){
		char * pokemon = element;
		return !strcasecmp(pokemon,pokemon_analizado);
	}

	void borrar_si_fue_capturado(void * element){
		char * pokemon=element;
		if(pokemon_en_lista(pokemones_capturados,pokemon)){
			pokemon_analizado = pokemon;
			list_remove_by_condition(pokemones_necesitados,es_igual_a_analizado);
			list_remove_by_condition(pokemones_capturados,es_igual_a_analizado);
		}
	}

	list_iterate(pokemones_por_capturar,borrar_si_fue_capturado);

	//log_debug(logger, "Kokemones necesitados obtenidos");
	//list_iterate(pokemones_necesitados, mostrar_kokemon);

	return pokemones_necesitados;
}

t_list * obtener_pokemones_necesitados_sin_repetidos(){
	t_list * pokemones_necesitados = obtener_pokemones_necesitados();
	t_list * pokemones_sin_repetidos = list_create();

	void agrego_si_no_existe(void * element){
		char*pokemon = element;

		if(!pokemon_en_lista(pokemones_sin_repetidos,pokemon)){
			list_add(pokemones_sin_repetidos,pokemon);
		}
	}

	list_iterate(pokemones_necesitados,agrego_si_no_existe);

	return pokemones_sin_repetidos;
}

// AUXILIARES

int requiero_pokemon(char * pokemon){
	t_list * pokemones_necesitados = obtener_pokemones_necesitados();

	return pokemon_en_lista(pokemones_necesitados,pokemon);
}

int pokemon_en_lista(t_list * lista_pokemones,char * pokemon){
	bool pokemon_igual_aux(void*elemento){
		char * pokemon_a_comparar =elemento;
		return !strcasecmp(pokemon_a_comparar,pokemon);
	}
	return list_any_satisfy(lista_pokemones,pokemon_igual_aux);
}

int cantidad_repeticiones_en_lista(t_list * lista_pokemones, char *pokemon ){
	bool es_un_repetido_aux(void*elemento){
		char *pokemon_a_comparar = elemento;
		return !strcasecmp(pokemon_a_comparar,pokemon);
	}
	return list_count_satisfying(lista_pokemones,es_un_repetido_aux);

}
