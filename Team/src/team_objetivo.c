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

// Funciones de fijarse si se requiere pokemon para objetivo global
bool requiero_pokemon(t_appeared_pokemon * mensaje_appeared){

	bool pokemon_requerido = esta_pokemon_objetivo(mensaje_appeared->pokemon) && !capture_pokemon_objetivo(mensaje_appeared->pokemon);
	return pokemon_requerido;
}
 bool esta_pokemon_objetivo(char *pokemon_candidato){
 	t_list *lista_pokemones_objetivo_global = obtener_pokemones_de_lista_seleccionada(objetivo_global);
 	log_trace(logger,"Lista de pokemones objetivo");
 	bool esta = esta_en_lista(pokemon_candidato,lista_pokemones_objetivo_global);
 	log_trace(logger,"Pokemon esta en objetivo %d \n",esta);
 	return esta;
 	}

 bool capture_pokemon_objetivo(char * pokemon_candidato){

 	t_list * lista_pokemones_globales_capturados = obtener_pokemones_de_lista_seleccionada(pokemones_globales_capturados);

 	if(esta_en_lista(pokemon_candidato,lista_pokemones_globales_capturados))
 	{
 		log_trace(logger,"Pokemon Capturado previamente");
 		bool atrape_repeticiones_necesarias = pokemon_fue_atrapado_cantidad_necesaria(pokemon_candidato);
 		log_trace(logger,"Pokemon fue atrapado veces necesarias %d \n",atrape_repeticiones_necesarias);
 		return atrape_repeticiones_necesarias;
 	}else
 	{
 	log_trace(logger,"Pokemon no esta en la lista, no fue capturado");
 	return false;
 	}
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

 int cantidad_pokemon_en_lista_objetivos(t_list * lista_seleccionada, char * pokemon){
	 bool es_pokemon_requerido(void *elemento){
	 		 t_objetivo * objetivo_a_comparar=elemento;
	 		 return !strcasecmp(objetivo_a_comparar->pokemon,pokemon);
	 	 }
	 	t_objetivo * objetivo_de_pokemon = list_find(lista_seleccionada,es_pokemon_requerido);
	 	 int cantidad= objetivo_de_pokemon->cantidad;
	 	return cantidad;
 }



 bool pokemon_fue_atrapado_cantidad_necesaria(char *pokemon){
 	int cantidad_en_objetivo = cantidad_pokemon_en_lista_objetivos(objetivo_global,pokemon);
 	int cantidad_veces_atrapado = cantidad_pokemon_en_lista_objetivos(pokemones_globales_capturados,pokemon);
 	return cantidad_en_objetivo == cantidad_veces_atrapado;
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
	//TODO: cambiar la condicion a que por cada pokemon_por_capturar este en pokemones_capturados
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
