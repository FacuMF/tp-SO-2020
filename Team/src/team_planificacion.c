#include "team.h"

// PLANIFICACION GENERAL

void iniciar_planificador() {
	list_iterate(head_entrenadores, lanzar_hilo_entrenador);
	elegir_algoritmo();

	pthread_mutex_init(&cpu_disponible, NULL);
	pthread_mutex_init(&entrenadores_ready, NULL);

	t_entrenador * entrenador_en_exec = NULL;

	while (1) { //TODO: Mientras no haya terminado tod.o
		pthread_mutex_lock(&cpu_disponible);

		while (!entrenadores_en_ready()) {
			pthread_mutex_lock(&entrenadores_ready);
		}

		t_entrenador * entrenador = obtener_entrenador_a_planificar();

		// TODO: Revisar y testear
		if(algoritmo_elegido == A_SJFCD && entrenador_en_exec != NULL){
			if(entrenador->estimacion_rafaga != entrenador->estimacion_rafaga){
				desalojar = 1;
				entrenador_en_exec = entrenador;
				ejecutar_entrenador(entrenador);
			}
		}else{
			entrenador_en_exec = entrenador;
			ejecutar_entrenador(entrenador);
		}
	}

}

void elegir_algoritmo() {
	char* algoritmo = config_get_string_value(config,
			"ALGORITMO_PLANIFICACION");

	if (strcmp(algoritmo, "FIFO") == 0) {
		algoritmo_elegido = A_FIFO;
	} else if (strcmp(algoritmo, "RR") == 0) {
		algoritmo_elegido = A_RR;
	} else if (strcmp(algoritmo, "SJF-SD") == 0) {
		algoritmo_elegido = A_SJFCD;
	} else if (strcmp(algoritmo, "SJF-CD") == 0) {
		algoritmo_elegido = A_SJFSD;
	} else {
		log_error(logger, "Algoritmo invalido");
	}
}

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while (1) {
		pthread_mutex_lock(&(entrenador->sem_est));
		log_debug(logger, "Entrenador despierto: Posicion %i %i",
				entrenador->posicion[0], entrenador->posicion[1]);

		if(entrenador->catch_pendiente!=NULL){
			cazar_pokemon(entrenador);
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_ESPERANDO;
			pthread_mutex_unlock(&cpu_disponible);
		}else if(objetivo_propio_cumplido(entrenador)){
			actualizar_timestamp(entrenador);
			pthread_mutex_unlock(&cpu_disponible);
			break; // Pasa a exit
		}else if(tiene_espacio_disponible(entrenador)){
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_NORMAL;
			pthread_mutex_unlock(&cpu_disponible);
		}else if(!tiene_espacio_disponible(entrenador)){
			actualizar_timestamp(entrenador);
			entrenador->estado = BLOCKED_DEADLOCK;
			pthread_mutex_unlock(&cpu_disponible);
		}
		//TODO: Agregar else if para cuando tenga que hacer el deadlock
	}
	entrenador->estado = EXIT;
}

void cazar_pokemon(t_entrenador * entrenador) {
	int ciclos_esta_corrida = 0;

	while (entrenador->ciclos_cpu_restantes > 0) {
		// Chequeo si es necesario bloquearse
		switch (algoritmo_elegido) {
		case A_RR:
			if (ciclos_esta_corrida >= quantum) {
				ciclos_esta_corrida = 0;
				bloquear_entrenador(entrenador);
			}
			break;
		case A_SJFCD:
			if (desalojar) {
				entrenador->estimacion_rafaga = constante_estimacion * ciclos_esta_corrida + (1-constante_estimacion)*entrenador->estimacion_rafaga;
				desalojar = 0;
				bloquear_entrenador(entrenador);
			}
			break;
		default:
			break;
		}
		sleep(retardo_ciclo_cpu);
		if(entrenador->ciclos_cpu_restantes > 1)
			mover_entrenador(entrenador); // TODO: Usar mutex para uso de CPU.
		else
			enviar_mensaje_catch(entrenador->catch_pendiente);

		ciclos_esta_corrida++;
		entrenador->ciclos_cpu_restantes--;
		entrenador->estimacion_rafaga--;

		log_debug(logger, "Entrenador en posicon %d %d, ciclos restantes: %d",
				entrenador->posicion[0], entrenador->posicion[1],
				entrenador->ciclos_cpu_restantes);
	}
	entrenador->estimacion_rafaga = constante_estimacion * ciclos_esta_corrida + (1-constante_estimacion)*entrenador->estimacion_rafaga;
}

void bloquear_entrenador(t_entrenador * entrenador) {
	pthread_mutex_unlock(&cpu_disponible);
	actualizar_timestamp(entrenador);
	entrenador->estado = READY;
	pthread_mutex_lock(&(entrenador->sem_est));
	log_debug(logger, "Entrenador despierto: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);
}

void mover_entrenador(t_entrenador * entrenador) {
	if (entrenador->catch_pendiente->posx > entrenador->posicion[0]) {
		entrenador->posicion[0]++;
	} else if (entrenador->catch_pendiente->posx < entrenador->posicion[0]) {
		entrenador->posicion[0]--;
	} else if (entrenador->catch_pendiente->posy > entrenador->posicion[1]) {
		entrenador->posicion[1]++;
	} else if (entrenador->catch_pendiente->posy < entrenador->posicion[1]) {
		entrenador->posicion[1]--;
	}
}

int entrenadores_en_ready() {
	t_list * entrenadores_en_ready = encontrar_entrenadores_en_estado(READY,
			head_entrenadores);
	return !list_is_empty(entrenadores_en_ready);
}

int objetivo_propio_cumplido(t_entrenador *entrenador){
	t_list *pokemones_por_capturar= entrenador->pokemones_por_capturar;
	t_list * pokemones_capturados = entrenador->pokemones_capturados;

	bool fue_capturado(void *pokemon){
		char * pokemon_a_chequear = pokemon;
		int repeticiones_en_capturados = cantidad_repeticiones_en_lista(pokemones_capturados,pokemon_a_chequear);
		int repeticiones_en_por_capturar= cantidad_repeticiones_en_lista(pokemones_capturados,pokemon_a_chequear);

		return repeticiones_en_capturados == repeticiones_en_por_capturar ;
	}
	return list_all_satisfy(pokemones_por_capturar,fue_capturado);
}


// PLANIFICACION DE ENTRENADORES
t_entrenador * obtener_entrenador_a_planificar() {
	t_entrenador * entrenador;

	t_list * entrenadores_en_ready = encontrar_entrenadores_en_estado(READY,
			head_entrenadores);

	switch (algoritmo_elegido) {
	case A_FIFO:
	case A_RR:
		entrenador = obtener_entrenador_fifo(entrenadores_en_ready);
		break;
	case A_SJFSD:
	case A_SJFCD:
		entrenador = obtener_entrenador_sjf(entrenadores_en_ready);
		break;
	}
	return entrenador;
}

t_entrenador * obtener_entrenador_fifo(t_list * entrenadores) {
	//Sort por time menor y obtener el primero

	bool menor_tiempo(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return timeval_subtract(&entrenador_1->ultima_modificacion,
				&entrenador_2->ultima_modificacion);
	}

	t_list * entrenador_antes_modif = list_sorted(entrenadores, menor_tiempo);

	t_entrenador * entrenador_menor_tiempo = list_get(entrenador_antes_modif,
			0);

	return entrenador_menor_tiempo;
}

t_entrenador * obtener_entrenador_sjf(t_list * entrenadores) {
	//Sort por estimacion menor y obtener el primero

	bool menor_estimacion(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return entrenador_1->estimacion_rafaga < entrenador_1->estimacion_rafaga;
	}

	t_list * entrenadores_menor_est = list_sorted(entrenadores, menor_estimacion);

	t_entrenador * entrenador_menor_est = list_get(entrenadores_menor_est,
			0);

	return entrenador_menor_est;
}


int timeval_subtract(x, y)
	struct timeval *x, *y; {
	/* Perform the carry for the later subtraction by updating y. */
	if (x->tv_usec < y->tv_usec) {
		int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
		y->tv_usec -= 1000000 * nsec;
		y->tv_sec += nsec;
	}
	if (x->tv_usec - y->tv_usec > 1000000) {
		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
		y->tv_usec += 1000000 * nsec;
		y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait.
	 tv_usec is certainly positive.
	 NOTA: DESCOMENTAR SI USAMOS HRRN
	 result->tv_sec = x->tv_sec - y->tv_sec;
	 result->tv_usec = x->tv_usec - y->tv_usec;
	 */

	/* Return 1 if result is negative. */
	return x->tv_sec < y->tv_sec;
}

void preparar_entrenador(t_entrenador * entrenador,
		t_appeared_pokemon * mensaje_appeared) {
	// Setear status = ready, calcular y llenar ciclos de cpu
	// Guardarle el mensaje de caught.
	entrenador->catch_pendiente = de_appeared_a_catch(mensaje_appeared);
	entrenador->ciclos_cpu_restantes = distancia(entrenador,
			mensaje_appeared->posx, mensaje_appeared->posy) + 1; //+1 por envio de mensaje

	actualizar_timestamp(entrenador);

	entrenador->estado = READY;

	log_debug(logger, "Nuevo ent en ready: Posicion %i %i",
			entrenador->posicion[0], entrenador->posicion[1]);

}

void actualizar_timestamp(t_entrenador * entrenador) {
	struct timeval tval;
	gettimeofday(&tval, NULL);
	entrenador->ultima_modificacion = tval;
}

void ejecutar_entrenador(t_entrenador * entrenador) {
	entrenador->estado = EXEC;
	pthread_mutex_unlock(&(entrenador->sem_est));
}

// MANEJO DE MENSAJES

void manejar_appeared(t_appeared_pokemon * mensaje_appeared) {
	log_debug(logger,"Manejo mensaje appeared");
	if (!requiero_pokemon(mensaje_appeared->pokemon))
		return;

	log_trace(logger,"Necesito pokemon");

	t_entrenador * entrenador_elegido = obtener_entrenador_buscado(
			mensaje_appeared->posx, mensaje_appeared->posy);

	if (entrenador_elegido != NULL){
		preparar_entrenador(entrenador_elegido, mensaje_appeared);

		pthread_mutex_unlock(&entrenadores_ready);

		if (algoritmo_elegido == A_SJFCD)
			pthread_mutex_unlock(&cpu_disponible);
	}else{
		list_add(appeared_a_asignar,mensaje_appeared);
	}
}

void manejar_caught(t_caught_pokemon* mensaje_caught) {
	t_entrenador * entrenador = buscar_entrenador_segun_id_mensaje(mensaje_caught->id_mensaje);
	if(entrenador==NULL)
		return; // Mensaje descartado

	if(mensaje_caught->ok_or_fail){ // SI LO ATRAPO
		//TODO: agregar a capturados
		entrenador->catch_pendiente = NULL;
		pthread_mutex_unlock(&(entrenador->sem_est));// Se autosetea status entrenador = blocked_normal/blocked_deadlock/exit
		//TODO: Verificar si hay en lista de sobrantes y cuantos necesito en base a catch de entrenadores.
		//TODO: hace falta que se fije si hay otros para ir a buscar?
	}else{ // SI NO LO ATRAPÓ

	}

	// TODO: Si hay algun otro en la lista de pendientes por planificar lo planifico de nuevo y pasa a ready
	// TODO: Si es CAUGHT NO voy a buscar si hay auxiliares en mi lista de localized sobrantes.
	// TODO: Si hay, se lo planifico
}

void manejar_localized(t_localized_pokemon* mensaje_localized) {
	if(!necesito_mensaje(mensaje_localized->id_mensaje))
		return; // Mensaje descartado

	if (!requiero_pokemon(mensaje_localized->pokemon))
			return; // Mensaje descartado

	if(mensaje_repetido(mensaje_localized))
		return; // Mensaje descartado

	// TODO: Si es NO, Verifico que tantos necesito
		// TODO: Los que necesito los planifico como appeared
	// TODO: los que me sobran los guardo en una lista de auxiliares por si los otros fallan.
}

int necesito_mensaje(int id_mensaje){
	bool corresponde_con_id_buscado(void * element){
		int id = (int)element;
		return id == id_mensaje;
	}

	return list_find(ids_mensajes_utiles,corresponde_con_id_buscado) != NULL;
}

int mensaje_repetido(t_localized_pokemon * mensaje_localized){
	return pokemon_asignado_a_entrenador(mensaje_localized->pokemon) && pokemon_en_pendientes(mensaje_localized->pokemon);
}

int pokemon_en_pendientes(char * pokemon){
	bool pokemon_pendiente(void * elemento){
		t_appeared_pokemon * appeared = elemento;
		return strcasecmp(appeared->pokemon,pokemon);
	}

	return list_any_satisfy(appeared_a_asignar,pokemon_pendiente);
}

int pokemon_asignado_a_entrenador(char * pokemon){
	bool pokemon_asignado(void * elemento){
		t_entrenador * entrenador = elemento;
		if(entrenador->catch_pendiente == NULL)
			return 0;
		return strcasecmp(entrenador->catch_pendiente->pokemon,pokemon);
	}

	return list_any_satisfy(head_entrenadores,pokemon_asignado);
}

// FILTRADO DE ENTRENADORES

t_entrenador * obtener_entrenador_buscado(int posx, int posy) {
	// Obtener el entrendor con el status correcto, lugar disponible y mas cercano.

	t_list * entrenadores_disponibles = obtener_entrenadores_disponibles(
			head_entrenadores);
	entrenadores_disponibles = obtener_entrenadores_con_espacio(
			entrenadores_disponibles);
	return hallar_entrenador_mas_cercano(posx, posy, entrenadores_disponibles);
}

t_list * obtener_entrenadores_disponibles(t_list * entrenadores) {
	t_list * entrenadores_disponibles = encontrar_entrenadores_en_estado(NEW,
			entrenadores);
	t_list * entrenadores_blocked_normal = encontrar_entrenadores_en_estado(
			BLOCKED_NORMAL, entrenadores);

	list_add_all(entrenadores_disponibles, entrenadores_blocked_normal);

	return entrenadores_disponibles;
}

t_list * encontrar_entrenadores_en_estado(t_estado estado_buscado,
		t_list * entrenadores) {

	bool esta_en_estado_correspondiente_aux(void * elemento) {
		t_entrenador * entrenador_estado = elemento;
		return entrenador_estado->estado == estado_buscado;
	}

	t_list * entrenadores_en_estado = list_filter(entrenadores,
			esta_en_estado_correspondiente_aux);
	// TODO: Ver que hacer cuando el entrenador mas cercano es NULL.

	return entrenadores_en_estado;
}

t_list * obtener_entrenadores_con_espacio(t_list * entrenadores) {
	bool tiene_espacio_disponible_aux(void * elemento) {
		t_entrenador * entrenador = elemento;
		return tiene_espacio_disponible(entrenador);
	}
	return list_filter(entrenadores, tiene_espacio_disponible_aux);

}

int tiene_espacio_disponible(t_entrenador * entrenador){
	int size_capturados = list_size(entrenador->pokemones_capturados);
	int size_por_capturar = list_size(entrenador->pokemones_por_capturar);
	return size_capturados < size_por_capturar;
}

t_entrenador * hallar_entrenador_mas_cercano(int posx, int posy,
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

	log_trace(logger, "Posicion entrenador cercano: Posicion %i %i",
			entrenador_mas_cercano->posicion[0],
			entrenador_mas_cercano->posicion[1]);
	return entrenador_mas_cercano;
}

// AUXILIARES

t_catch_pokemon * de_appeared_a_catch(t_appeared_pokemon * appeared) {

	t_catch_pokemon * mensaje_catch = crear_catch_pokemon(appeared->pokemon,
			appeared->posx, appeared->posy, -30);

	return mensaje_catch;

}

// TODO: Cambiar a lista de APPEARED a partir de localized, quizas.
t_list * lista_de_catch_a_partir_localized(
		t_localized_pokemon * localized_a_chequear) {

	t_list * lista_catchs_localized = list_create();

	char * pokemon_de_catch = localized_a_chequear->pokemon;

	void generar_catch_por_posicion(void * elemento) {
		t_posicion * posicion_de_lista = elemento;

		t_catch_pokemon * catch_a_agregar = crear_catch_pokemon(
				pokemon_de_catch, posicion_de_lista->x, posicion_de_lista->y,
				-30);

		list_add(lista_catchs_localized, catch_a_agregar);

	}

	list_iterate(localized_a_chequear->posiciones, generar_catch_por_posicion);

	return lista_catchs_localized;
}

t_entrenador * buscar_entrenador_segun_id_mensaje(int id_mensaje) {

	bool buscar_entrenador_con_catch(void * elemento) {

		t_entrenador * cada_entrenador = elemento;

		int id_mensaje_entrenador = cada_entrenador->catch_pendiente->id_mensaje;

		return id_mensaje_entrenador == id_mensaje;
	}

	t_entrenador * entrenador_buscado = list_find(head_entrenadores,
			buscar_entrenador_con_catch);

	if (entrenador_buscado == NULL)
		log_trace(logger, "Entrenador NO encontrado para el catch");
	else
		log_trace(logger, "Entrenador encontrado para el catch");

	return entrenador_buscado;
}

int distancia(t_entrenador * entrenador, int posx, int posy) {
	int distancia_x = entrenador->posicion[0] - posx;
	int distancia_y = entrenador->posicion[1] - posy;

	int distancia_e = abs(distancia_x) + abs(distancia_y);
	//log_debug(logger, "distancia entre %d %d y %d %d : %d",entrenador->posicion[0],entrenador->posicion[1],posx,posy,distancia_e);

	return distancia_e;
}


void lanzar_hilo_entrenador(void*element) {
	t_entrenador * entrenador = element;
	pthread_t hilo_entrenador;

	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,
			(void*) entrenador);

	(result != 0) ?
			log_error(logger, "Error lanzando el hilo") :
			log_trace(logger, "Entrenador lanzado: Pos %i %i",
					entrenador->posicion[0], entrenador->posicion[1]);
}

