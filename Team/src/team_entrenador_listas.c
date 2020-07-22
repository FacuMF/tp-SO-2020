#include "team.h"

t_entrenador * obtener_entrenador_buscado(int posx, int posy) {
	// Obtener el entrendor con el status correcto, lugar disponible y mas cercano.

	t_list * entrenadores_disponibles = obtener_entrenadores_disponibles(
			head_entrenadores);
	if(list_get(entrenadores_disponibles,0)==NULL) return NULL;
	entrenadores_disponibles = obtener_entrenadores_con_espacio(
			entrenadores_disponibles);
	if(list_get(entrenadores_disponibles,0)==NULL) return NULL;


	return obtener_entrenador_mas_cercano(posx, posy, entrenadores_disponibles);
}

t_list * obtener_entrenadores_disponibles(t_list * entrenadores) {
	t_list * entrenadores_disponibles = obtener_entrenadores_en_estado(NEW,
			entrenadores);

	t_list * entrenadores_blocked_normal = obtener_entrenadores_en_estado(
			BLOCKED_NORMAL, entrenadores);

	list_add_all(entrenadores_disponibles, entrenadores_blocked_normal);

	list_destroy(entrenadores_blocked_normal);

	return entrenadores_disponibles;
}

t_list * obtener_entrenadores_en_estado(t_estado estado_buscado,
		t_list * entrenadores) {

	bool esta_en_estado_correspondiente_aux(void * elemento) {
		t_entrenador * entrenador_estado = elemento;
		return entrenador_estado->estado == estado_buscado;
	}

	t_list * entrenadores_en_estado = list_filter(entrenadores,
			esta_en_estado_correspondiente_aux);

	return entrenadores_en_estado;
}

t_list * obtener_entrenadores_con_espacio(t_list * entrenadores) {
	bool tiene_espacio_disponible_aux(void * elemento) {
		t_entrenador * entrenador = elemento;
		return tiene_espacio_disponible(entrenador);
	}

	t_list * result = list_filter(entrenadores, tiene_espacio_disponible_aux);
	list_destroy(entrenadores);

	return result;

}

t_entrenador * obtener_entrenador_segun_id_mensaje(int id_mensaje) {

	bool buscar_entrenador_con_catch(void * elemento) {

		t_entrenador * cada_entrenador = elemento;

		int id_mensaje_entrenador = cada_entrenador->catch_pendiente->id_mensaje;

		return id_mensaje_entrenador == id_mensaje;
	}

	t_entrenador * entrenador_buscado = list_find(head_entrenadores,
			buscar_entrenador_con_catch);

	return entrenador_buscado;
}

int cantidad_entrenadores_buscando_pokemon(char * pokemon) {

	bool entrenador_busca_pokemon(void * elemento) {

		t_entrenador * entrenador = elemento;
		if(entrenador->catch_pendiente == NULL)
			return 0;
		else
			return !strcasecmp(entrenador->catch_pendiente->pokemon,pokemon);
	}

	return list_count_satisfying(head_entrenadores,entrenador_busca_pokemon);
}

t_entrenador * obtener_entrenador_mas_cercano(int posx, int posy,
		t_list *entrenadores) {

	bool menor_distancia(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return distancia(entrenador_1, posx, posy)
				< distancia(entrenador_2, posx, posy);
	}

	t_list * entrenadores_mas_cercanos = list_sorted(entrenadores,
			menor_distancia);

	t_entrenador * entrenador_mas_cercano = list_get(entrenadores_mas_cercanos,
			0);

	list_destroy(entrenadores_mas_cercanos);
	list_destroy(entrenadores);

	return entrenador_mas_cercano;
}

// AUXILIARES

int tiene_espacio_disponible(t_entrenador * entrenador){
	int size_capturados = list_size(entrenador->pokemones_capturados);
	int size_por_capturar = list_size(entrenador->pokemones_por_capturar);
	return size_capturados < size_por_capturar;
}

int distancia(t_entrenador * entrenador, int posx, int posy) {
	int distancia_x = entrenador->posicion[0] - posx;
	int distancia_y = entrenador->posicion[1] - posy;

	int distancia_e = abs(distancia_x) + abs(distancia_y);
	//log_debug(logger, "distancia entre %d %d y %d %d : %d",entrenador->posicion[0],entrenador->posicion[1],posx,posy,distancia_e);

	return distancia_e;
}
