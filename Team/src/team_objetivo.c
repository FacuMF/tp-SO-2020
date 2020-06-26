#include "team.h"

// OBJETIVO GLOBAL

t_list* formar_lista_de_objetivos(t_list * lista_de_pokemones) {
	t_list * lista_objetivo = list_create();
	void agrego_si_no_existe_aux(void *elemento) {
		agrego_si_no_existe(lista_objetivo, elemento);
	}

	list_iterate(lista_de_pokemones, agrego_si_no_existe_aux);

	log_trace(logger, "Objetivos Formados");
	list_iterate(lista_objetivo, mostrar_objetivo);

	return lista_objetivo;
}

void agrego_si_no_existe(t_list * lista_objetivo, void *nombrePokemon) {
	bool _yaExiste(void *inputObjetivo) {
		t_objetivo *cadaObjetivo = inputObjetivo;
		return !strcasecmp(cadaObjetivo->pokemon, nombrePokemon);
	}

	t_objetivo *objetivo = list_find(lista_objetivo, _yaExiste);

	if (objetivo != NULL) {
		objetivo->cantidad++;
	} else {
		t_objetivo * nuevo_objetivo = malloc(sizeof(t_objetivo));
		nuevo_objetivo->cantidad = 1;
		nuevo_objetivo->pokemon = nombrePokemon;
		list_add(lista_objetivo, nuevo_objetivo);
	}

}

void mostrar_objetivo(void *elemento) {
	t_objetivo *objetivo = elemento;
	log_trace(logger, "Data de objetivo: %s %i",objetivo->pokemon ,objetivo->cantidad);
}

bool requiero_pokemon(char * pokemon_a_chequear){
	t_objetivo * objetivo_pokemon_chequeado = hallar_objetivo_segun_pokemon(objetivo_global,pokemon_a_chequear);

	if(objetivo_pokemon_chequeado==NULL){
		return false;
	}
	else{
		log_trace(logger,"Pokemon dentro del objetivo");

		t_list *  pokemones_capturados = obtener_pokemones_capturados();

		t_list * lista_pokemones_ya_atrapados = formar_lista_de_objetivos(pokemones_capturados);

		bool requiero = !objetivo_de_pokemon_cumplido (objetivo_pokemon_chequeado,lista_pokemones_ya_atrapados);

		return requiero;
	}
}

t_objetivo * hallar_objetivo_segun_pokemon(t_list * lista_seleccionada,char*pokemon_a_buscar){

	bool es_objetivo_de_pokemon(void *elemento){
		 		 t_objetivo * objetivo_a_comparar=elemento;

		 		 return !strcasecmp(objetivo_a_comparar->pokemon,pokemon_a_buscar);
		 	 }

		 	t_objetivo * objetivo_encontrado = list_find(lista_seleccionada,es_objetivo_de_pokemon);

	return objetivo_encontrado;
}

bool objetivo_de_pokemon_cumplido(t_objetivo * objetivo_pokemon, t_list * pokemones_atrapados){

	t_objetivo * objetivo_en_lista_atrapados = hallar_objetivo_segun_pokemon(pokemones_atrapados,objetivo_pokemon->pokemon);

	return objetivo_en_lista_atrapados->cantidad == objetivo_pokemon->cantidad;
	}




t_list * obtener_pokemones_de_lista_seleccionada(t_list * lista_seleccionada){
	t_list * lista_pokemones_objetivos_capturados = list_create();
	void aniadir_pokemon_a_lista(void *elemento){
				t_objetivo * objetivo = elemento;
				list_add(lista_pokemones_objetivos_capturados,objetivo->pokemon);
			}
			list_iterate(lista_seleccionada,aniadir_pokemon_a_lista);
		return lista_pokemones_objetivos_capturados;

}


// Funciones de objetivo cumplido
bool objetivo_cumplido(t_entrenador *entrenador){
	t_list *pokemone_por_capturar= entrenador->pokemones_por_capturar;
	t_list * pokemone_capturados = entrenador->pokemones_capturados;
	bool fue_capturado(void *pokemon){
		char * pokemon_a_chequear = pokemon;
		bool capturado;
		if(es_repetido(pokemon,pokemone_por_capturar)){
			int repeticiones_en_capturados = cantidad_repeticiones_en_lista(pokemon_a_chequear,pokemone_capturados);
			int repeticiones_en_por_capturar= cantidad_repeticiones_en_lista(pokemon_a_chequear,pokemone_por_capturar);
			capturado = repeticiones_en_capturados == repeticiones_en_por_capturar ;
		}else{
		capturado = esta_en_lista(pokemon_a_chequear,pokemone_capturados);
		}
		return capturado;
	}
	bool cumpli_objetivo= list_all_satisfy(pokemone_por_capturar,fue_capturado);
	return cumpli_objetivo;
}

bool pokemon_igual(char *un_pokemon, char * otro_pokemon){
	return !strcasecmp(un_pokemon,otro_pokemon);
}
bool esta_en_lista(char * pokemon,t_list * lista_pokemones){
	bool pokemon_igual_aux(void*elemento){
			char * pokemon_a_comparar =elemento;
			bool captura = pokemon_igual(pokemon_a_comparar,pokemon);
			return captura;
		}
	bool pokemon_encontrado = list_any_satisfy(lista_pokemones,pokemon_igual_aux);
	return pokemon_encontrado;
}
int cantidad_repeticiones_en_lista(char *pokemon, t_list * lista_pokemones){
	bool es_un_repetido_aux(void*elemento){
		char *pokemon_a_comparar = elemento;
		bool repetido = pokemon_igual(pokemon_a_comparar,pokemon);
		return repetido;
	}
	int cantidad = list_count_satisfying(lista_pokemones,es_un_repetido_aux);
	return cantidad;
}

bool es_repetido(char *pokemon, t_list *lista_pokemones){

	bool es_un_repetido_aux(void*elemento){
			char *pokemon_a_comparar = elemento;
			bool repetido = pokemon_igual(pokemon_a_comparar,pokemon);
			return repetido;
		}
	int cantidad = list_count_satisfying(lista_pokemones,es_un_repetido_aux);
	return cantidad >1;
}
