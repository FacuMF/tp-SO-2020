#include "team.h"

// CARGAR ENTRENADORES

t_list* cargar_entrenadores() {
	t_list* head_entrenadores = list_create();

	char ** posiciones = config_get_array_value(config,
			"POSICIONES_ENTRENADORES");
	char ** pokemones_capturados = config_get_array_value(config,
			"POKEMON_ENTRENADORES");
	char ** objetivos = config_get_array_value(config,
			"OBJETIVOS_ENTRENADORES");

	int i = 0;
	while (posiciones[i] != NULL) {	// TODO: Cambiar a for
		t_entrenador * entrenador = malloc(sizeof(t_entrenador));
		pthread_mutex_init(&entrenador->sem_est, NULL);
		pthread_mutex_lock(&(entrenador->sem_est));	//TODO: Ver como inicializar hilo en 0 sin hacer lock inmediatamente

		entrenador->posicion = de_string_a_posicion(posiciones[i]);
		entrenador->pokemones_capturados = string_a_pokemon_list(
				pokemones_capturados[i]);
		entrenador->pokemones_por_capturar = string_a_pokemon_list(
				objetivos[i]);
		entrenador->estado = NEW;
		list_add(head_entrenadores, entrenador);
		i++;
	}

	log_trace(logger, "Entrenadores cargados");
	//mostrar_entrenadores(head_entrenadores);

	return (head_entrenadores);
}

void mostrar_entrenadores(t_list * head_entrenadores) {
	list_iterate(head_entrenadores, mostrar_data_entrenador);
}

void mostrar_data_entrenador(void * element) {
	t_entrenador * entrenador = element;
	log_trace(logger, "Data Entrenador: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);
	list_iterate(entrenador->pokemones_capturados, mostrar_kokemon);
	list_iterate(entrenador->pokemones_por_capturar, mostrar_kokemon);
}

// PARSERS DE INPUT DATA

int* de_string_a_posicion(char* cadena_con_posiciones) {
	char** posicion_prueba = string_split(cadena_con_posiciones, "|");

	int* posicion = malloc(sizeof(int) * 3);
	posicion[0] = atoi(posicion_prueba[0]);
	posicion[1] = atoi(posicion_prueba[1]);

	return posicion;
}

t_list* string_a_pokemon_list(char* cadena_con_pokemones) {
	char** pokemones = string_split(cadena_con_pokemones, "|");

	t_list* head_pokemones = list_create();

	int i = 0;

	while (pokemones[i] != NULL) {
		list_add(head_pokemones, pokemones[i]);
		i++;
	}

	return head_pokemones;

}

void aniadir_pokemon(t_list *pokemones_repetidos, void * pokemones) {
	list_add(pokemones_repetidos, pokemones);
}

t_list* obtener_pokemones(t_list *head_entrenadores) {
	t_list * pokemones_repetidos = list_create();
	void aniadir_pokemon_aux(void *pokemones) {
		aniadir_pokemon(pokemones_repetidos, pokemones);
	}
	void buscar_pokemon(void * head) {
		t_entrenador *entrenador = head;
		list_iterate(entrenador->pokemones_por_capturar, aniadir_pokemon_aux);
	}

	list_iterate(head_entrenadores, buscar_pokemon);

	log_trace(logger, "Kokemones obtenidos");
	//list_iterate(pokemones_repetidos, mostrar_kokemon);

	return pokemones_repetidos;
}

void mostrar_kokemon(void*elemento) {
	log_trace(logger, elemento);
}
