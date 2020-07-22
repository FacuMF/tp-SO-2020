#include "team.h"

void manejar_appeared(t_appeared_pokemon * mensaje_appeared) {
	pthread_mutex_lock(&manejar_mensaje);
	if (!requiero_pokemon(mensaje_appeared->pokemon)) {
		liberar_mensaje_appeared_pokemon(mensaje_appeared);
		return;
	}

	list_add(pokemones_recibidos, mensaje_appeared->pokemon);

	log_trace(logger, "Manejo mensaje appeared");

	t_entrenador * entrenador_elegido = obtener_entrenador_buscado(
			mensaje_appeared->posx, mensaje_appeared->posy);

	if (entrenador_elegido != NULL) {
		planificar_entrenador(entrenador_elegido, mensaje_appeared);
	} else
		list_add(appeared_a_asignar, mensaje_appeared);
	pthread_mutex_unlock(&manejar_mensaje);
}

void manejar_appeared_aux(void * element) {
	t_appeared_pokemon * mensaje = element;
	manejar_appeared(mensaje);
}

void manejar_localized(t_localized_pokemon* mensaje_localized) {
	pthread_mutex_lock(&manejar_mensaje);

	//TODO: Testear con gamecard el necesito mensaje
	if ((!necesito_mensaje(mensaje_localized->id_mensaje))
			|| (!requiero_pokemon(mensaje_localized->pokemon))
			|| (pokemon_en_lista(pokemones_recibidos,
					mensaje_localized->pokemon))) {
		liberar_mensaje_localized_pokemon(mensaje_localized);
		return; // Mensaje descartado
	}
	log_trace(logger, "Manejo mensaje localized");

	list_add(pokemones_recibidos, mensaje_localized->pokemon);

	pthread_mutex_lock(&mutex_pokemones_necesitados);
	int necesitados = cantidad_repeticiones_en_lista(pokemones_necesitados,
			mensaje_localized->pokemon);
	pthread_mutex_unlock(&mutex_pokemones_necesitados);

	necesitados -= cantidad_entrenadores_buscando_pokemon(
			mensaje_localized->pokemon);

	t_list * mensajes_appeared_equivalentes = de_localized_a_lista_appeared(
			mensaje_localized);
	t_list * mensajes_appeared_necesitados = list_take_and_remove(
			mensajes_appeared_equivalentes, necesitados);

	list_iterate(mensajes_appeared_necesitados, manejar_appeared_aux);

	list_add_all(appeared_auxiliares, mensajes_appeared_equivalentes);

	list_destroy(mensajes_appeared_equivalentes);
	list_destroy(mensajes_appeared_necesitados);
	liberar_mensaje_localized_pokemon(mensaje_localized);

	pthread_mutex_unlock(&manejar_mensaje);
}

void manejar_caught(t_caught_pokemon* mensaje_caught, t_entrenador * entrenador) {
	if (entrenador == NULL)
		entrenador = obtener_entrenador_segun_id_mensaje(
				mensaje_caught->id_mensaje);
	if (entrenador == NULL) {
		liberar_mensaje_caught_pokemon(mensaje_caught);
		return; // Mensaje descartado
	}

	pthread_mutex_lock(&manejar_mensaje);
	log_trace(logger, "Manejo mensaje caught");

	char * pokemon = entrenador->catch_pendiente->pokemon;
	if (mensaje_caught->ok_or_fail) { // SI LO ATRAPO

		// Zona critica ampliada para evitar inconsistencias por pokemon siendo buscado y en lista de necesitados.
		list_add(entrenador->pokemones_capturados, pokemon);

		pthread_mutex_lock(&mutex_pokemones_necesitados);
		eliminar_si_esta(pokemones_necesitados, pokemon);
		free(entrenador->catch_pendiente);
		entrenador->catch_pendiente = NULL;
		pthread_mutex_unlock(&mutex_pokemones_necesitados);

		//hasta aca esta en bloqueado, nadie lo va a tocar.

		sem_post(&(entrenador->sem_est)); // Se autosetea status entrenador = blocked_normal/blocked_deadlock/exit

		// Si guarde auxiliares y no los necesito, borrarlos
		if (pokemon_en_auxiliares(pokemon)
				&& !pokemon_asignado_a_entrenador(pokemon))
			eliminar_de_lista_appeared(appeared_auxiliares, pokemon);

		if (entrenador->estado != BLOCKED_NORMAL) {
			sem_post(&verificar_objetivo_global);
			pthread_mutex_unlock(&manejar_mensaje);
			liberar_mensaje_caught_pokemon(mensaje_caught);
			verificar_pendientes();
			return;
		}

		// Si hay pendientes, replanificarlo
		t_list * lista_com_mensaje = list_take_and_remove(appeared_a_asignar,
				1);
		t_appeared_pokemon * mensaje = list_get(lista_com_mensaje, 0);

		if (mensaje != NULL)
			planificar_entrenador(entrenador, mensaje);
		else
			sem_post(&verificar_objetivo_global);

		list_destroy(lista_com_mensaje);

	} else { // SI NO LO ATRAPÓ
		t_appeared_pokemon * mensaje_app = obtener_auxiliar_de_lista(pokemon);

		if (mensaje_app != NULL) {
			planificar_entrenador(entrenador, mensaje_app);
		} else {
			// Si hay pendientes, replanificarlo
			t_list * lista_com_mensaje = list_take_and_remove(
					appeared_a_asignar, 1);
			t_appeared_pokemon * mensaje = list_get(lista_com_mensaje, 0);

			if (mensaje != NULL)
				planificar_entrenador(entrenador, mensaje);
		}
	}
	liberar_mensaje_caught_pokemon(mensaje_caught);
	pthread_mutex_unlock(&manejar_mensaje);
}

// AUXILIARES - POKEMON
void verificar_pendientes() {
	// Si hay pendientes, replanificarlo
	t_list * lista_com_mensaje = list_take_and_remove(appeared_a_asignar, 1);
	t_appeared_pokemon * mensaje = list_get(lista_com_mensaje, 0);
	list_destroy(lista_com_mensaje);

	if (mensaje == NULL){
		sem_post(&verificar_objetivo_global);
		return;
	}
	manejar_appeared(mensaje);
}

int pokemon_en_auxiliares(char * pokemon) {
	bool pokemon_auxiliar(void * elemento) {
		t_appeared_pokemon * appeared = elemento;
		return !strcasecmp(appeared->pokemon, pokemon);
	}

	return list_any_satisfy(appeared_auxiliares, pokemon_auxiliar);
}

int pokemon_asignado_a_entrenador(char * pokemon) {
	bool pokemon_asignado(void * elemento) {
		t_entrenador * entrenador = elemento;
		if (entrenador->catch_pendiente == NULL)
			return 0;
		return !strcasecmp(entrenador->catch_pendiente->pokemon, pokemon);
	}

	return list_any_satisfy(head_entrenadores, pokemon_asignado);
}

// AUXILIARES - Mensajes
int necesito_mensaje(int id_mensaje) {
	bool corresponde_con_id_buscado(void * element) {
		int id = (int) element;
		return id == id_mensaje;
	}
	pthread_mutex_lock(&mutex_ids_mensajes);
	int result = list_find(ids_mensajes_utiles,
			corresponde_con_id_buscado) != NULL;
	pthread_mutex_unlock(&mutex_ids_mensajes);

	return result;
}

t_appeared_pokemon * obtener_auxiliar_de_lista(char * pokemon) {
	bool corresponde_con_pokemon(void * element) {
		t_appeared_pokemon * appeared = element;
		return !strcasecmp(appeared->pokemon, pokemon);
	}

	t_appeared_pokemon * mensaje = list_find(appeared_auxiliares,
			corresponde_con_pokemon);

	bool es_diferente_a_mensaje(void * element) {
		t_appeared_pokemon * appeared = element;
		return strcasecmp(appeared->pokemon, mensaje->pokemon)
				&& appeared->id_mensaje != mensaje->id_mensaje
				&& appeared->posx != mensaje->posx
				&& appeared->posy != mensaje->posy;
	}
	if (mensaje != NULL) {
		t_list * lista_auxiliar = appeared_auxiliares;
		appeared_auxiliares = list_filter(appeared_auxiliares,
				es_diferente_a_mensaje);
		list_destroy(lista_auxiliar);
	}

	return mensaje;
}

void eliminar_de_lista_appeared(t_list * appeared_auxiliares, char * pokemon) {
	bool corresponde_con_pokemon(void * element) {
		t_appeared_pokemon * appeared = element;
		return !strcasecmp(appeared->pokemon, pokemon);
	}

	for (int i = 0;
			i
					< list_count_satisfying(appeared_auxiliares,
							corresponde_con_pokemon); i++)
		list_remove_by_condition(appeared_auxiliares, corresponde_con_pokemon);
}

t_catch_pokemon * de_appeared_a_catch(t_appeared_pokemon * appeared) {

	t_catch_pokemon * mensaje_catch = crear_catch_pokemon(appeared->pokemon,
			appeared->posx, appeared->posy, -30);

	return mensaje_catch;

}

t_list * de_localized_a_lista_appeared(t_localized_pokemon * localized) {

	t_list * lista_appeared = list_create();

	char * pokemon = localized->pokemon;

	void agregar_appeared_de_posiciones(void * elemento) {
		t_posicion * posicion_de_lista = elemento;

		t_appeared_pokemon * appeared_a_agregar = crear_appeared_pokemon(
				pokemon, posicion_de_lista->x, posicion_de_lista->y, -30);

		list_add(lista_appeared, appeared_a_agregar);
	}

	list_iterate(localized->posiciones, agregar_appeared_de_posiciones);

	return lista_appeared;
}
