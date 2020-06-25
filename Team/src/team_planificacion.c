#include "team.h"


void iniciar_planificador(){
	list_iterate(head_entrenadores, lanzar_hilo_entrenador);

	//TODO: cambiar a mientras no se haya cumplido el global y no haya terminado everything
	while(1){
		//TODO: lockearse, esperar a que le avisen de nueva tarea
		// Si es appeared / localized revisar si lo necesito
		//...
	}

}

void lanzar_hilo_entrenador(void*element) {
	t_entrenador * entrenador = element;
	pthread_t hilo_entrenador;

	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,
			(void*) entrenador);

	(result != 0)?
			log_error(logger, "Error lanzando el hilo"):
			log_trace(logger, "Entrenador lanzado: Pos %i %i", entrenador->posicion[0], entrenador->posicion[1]);
}

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while (!(objetivo_cumplido(entrenador))) {
		pthread_mutex_lock(&(entrenador->sem_est));
		log_trace(logger, "Entrenador despierto: Posicion %i %i", entrenador->posicion[0], entrenador->posicion[1]);
	}

}

int distancia(t_entrenador * entrenador, int posx, int posy) {
	int distancia_e = abs(distancia_en_eje(entrenador, posx, 0))
			+ abs(distancia_en_eje(entrenador, posx, 1));
	return distancia_e;
}

int distancia_en_eje(t_entrenador *entrenador, int pose, int pos) {
	int resta = entrenador->posicion[pos] - pose;
	return resta;
}


void mover_entrenador_a_posicion(t_entrenador*entrenador, int posx, int posy) {
	int distancia_en_x = abs(distancia_en_eje(entrenador, posx, 0));
	int distancia_en_y = abs(distancia_en_eje(entrenador, posy, 1));

	int retardo_ciclo = config_get_int_value(config, "RETARDO_CICLO_CPU"); // TODO pasar a variable global ya que probablemente se use en varias funciones

	while (distancia_en_x != 0) {
		sleep(retardo_ciclo);
		distancia_en_x--;
	}

	while (distancia_en_y != 0) {
		sleep(retardo_ciclo);
		distancia_en_y--;
	}

	cambiar_posicion_entrenador(entrenador, posx, posy);
	log_trace(logger,
			"Entrenador se movio de posicion: Nueva posicion en X: %d , En y: %d",
			entrenador->posicion[0], entrenador->posicion[1]);
}

void cambiar_posicion_entrenador(t_entrenador*entrenador, int posx, int posy) {
	entrenador->posicion[0] = posx;
	entrenador->posicion[1] = posy;
}

// Funcion de Planificacion de entrenadores
/*
 void comenzar_planificacion_entrenadores(t_appeared_pokemon * appeared_recibido){
 t_entrenador *entrenador_a_planificar= hallar_entrenador_mas_cercano_segun_appeared(appeared_recibido);
 desbloquear_entrenador(entrenador_a_planificar);
 mover_entrenador_a_posicion(entrenador_a_planificar,appeared_recibido->posx,appeared_recibido->posy); // Comento para poder testear sin modificar entrenadores

 log_trace(logger,"Aca ejecutaria envio de catch pokemon");

 atrapar_pokemon(entrenador_a_planificar,appeared_recibido); //TODO lanzar mensaje catch_pokemon

 }
 */

t_entrenador * hallar_entrenador_mas_cercano_segun_appeared(
		t_appeared_pokemon * appeared_recibido) {
	t_entrenador * entrenador_a_planificar_cercano =
			hallar_entrenador_mas_cercano(appeared_recibido->posx,
					appeared_recibido->posy);
	return entrenador_a_planificar_cercano;
}

t_entrenador * hallar_entrenador_mas_cercano(int posx, int posy) {

	bool tiene_espacio_disponible(void * elemento) {
		t_entrenador * entrenador = elemento;
		int size_capturados = list_size(entrenador->pokemones_capturados);
		int size_por_capturar = list_size(entrenador->pokemones_por_capturar);
		return size_capturados < size_por_capturar;
	}
	bool menor_distancia(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return distancia(entrenador_1, posx, posy)
				> distancia(entrenador_2, posx, posy);
	}
	t_list *entrenadores_disponibles = list_filter(head_entrenadores,
			tiene_espacio_disponible);
	t_list * entrenadores_mas_cercanos = list_sorted(entrenadores_disponibles,
			menor_distancia);
	t_entrenador * entrenador_mas_cercano = list_get(entrenadores_mas_cercanos,
			0);

	log_trace(logger, "Posicion entrenador cercano: Posicion %i %i",
			entrenador_mas_cercano->posicion[0],
			entrenador_mas_cercano->posicion[1]);
	return entrenador_mas_cercano;
}

