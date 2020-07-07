#include "team.h"

// PLANIFICACION GENERAL

void iniciar_planificador(){
	list_iterate(head_entrenadores, lanzar_hilo_entrenador);
	pthread_mutex_init(&planificar, NULL);

	while(1){//TODO: Mientras no haya terminado tod.o

		while(!entrenadores_en_ready()){
			pthread_mutex_lock(&planificar);
		}

		// TODO: En base al algoritmo, obtener el entrenador siguiente
		t_entrenador * entrenador = obtener_entrenador_a_planificar();
		ejecutar_entrenador(entrenador);

	}

}

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while (!(objetivo_cumplido(entrenador))) {
		pthread_mutex_lock(&(entrenador->sem_est));
		log_trace(logger, "Entrenador despierto: Posicion %i %i", entrenador->posicion[0], entrenador->posicion[1]);

		// TODO: en base a mensaje catch interno moverse a la posicion indicada
		// TODO: Mandar catch
	}

	// TODO: Cambiar su propio estado a exit?

}

bool entrenadores_en_ready(){
	t_list * entrenadores_en_ready = encontrar_entrenadores_en_estado(READY, head_entrenadores);
	return !list_is_empty(entrenadores_en_ready);
}

// PLANIFICACION DE ENTRENADORES
t_entrenador * obtener_entrenador_a_planificar(){
	t_entrenador * entrenador;

	t_list * entrenadores_en_ready = encontrar_entrenadores_en_estado(READY, head_entrenadores);

	// TODO: Dependiendo del algoritmo, llamar al metodo correcto (con un switch)

	//

	return entrenador;
}

t_entrenador * obtener_entrenador_sjf(t_list * entrenadores){
	// TODO: Sort por ciclos de cpu menores
	// TODO: obtener el primero

}

void preparar_entrenador(t_entrenador * entrenador, t_appeared_pokemon * mensaje_appeared){
	// Setear status = ready, calcular y llenar ciclos de cpu
	// Guardarle el mensaje de caught.
	entrenador->catch_pendiente = de_appeared_a_catch(mensaje_appeared);
	entrenador->ciclos_cpu_restantes = distancia(entrenador,mensaje_appeared->posx,mensaje_appeared->posy);
	entrenador->estado = READY;
}

void ejecutar_entrenador(t_entrenador * entrenador){
	entrenador->estado = READY;
	pthread_mutex_unlock(&(entrenador->sem_est));
}

// MANEJO DE MENSAJES

void manejar_appeared(t_appeared_pokemon * mensaje_appeared){
	if (!requiero_pokemon(mensaje_appeared->pokemon))
		return;

	t_entrenador * entrenador_elegido = obtener_entrenador_buscado(mensaje_appeared->posx,mensaje_appeared->posy);

	preparar_entrenador(entrenador_elegido);

	pthread_mutex_unlock(&planificar);
}

void manejar_caught(t_caught_pokemon* mensaje_caught){
	// TODO: Revisar si es correlativo a algun catch por ID
	// TODO: Buscar entrenador con ese catch adentro  // Funcion buscar_entrenador_segun_catch(catch_buscado)
	// TODO: Si es YES, agregar a capturados, si NO skippear este paso
	// TODO: Setear status entrenador = ready/blocked/exit
	// TODO: Sacar appeared de la lista
	// TODO: Si hay algun otro en la lista de recibidos lo planifico de nuevo y pasa a ready
	// TODO: Si es NO voy a buscar si hay auxiliares en mi lista de localized sobrantes.
	// TODO: Si hay, los muevo a la otra lista y los planifico como appeared de nuevo
}

void manejar_localized(t_localized_pokemon* mensaje_localized){
	// TODO: Verifico si se corresponde con un id de rta // Funcion es_id_necesario
	// TODO: Verifico si ya tengo uno en mi lista para esta especie (app o localized)
	// TODO: Si YES, lo descarto
	// TODO: Si es NO, Verifico que tantos necesito
	// TODO: Los que necesito los planifico como appeared
	// TODO: los que me sobran los guardo en una lista de auxiliares por si los otros fallan.
}


// FILTRADO DE ENTRENADORES

t_entrenador * obtener_entrenador_buscado(int posx, int posy){
	// Obtener el entrendor con el status correcto, lugar disponible y mas cercano.

	t_list * entrenadores_disponibles = obtener_entrenadores_disponibles(head_entrenadores);
	entrenadores_disponibles = obtener_entrenadores_con_espacio(entrenadores_disponibles);
	return hallar_entrenador_mas_cercano(posx,posy,entrenadores_disponibles);
}

t_list * obtener_entrenadores_disponibles(t_list * entrenadores){
	t_list * entrenadores_disponibles = encontrar_entrenadores_en_estado(NEW,entrenadores);
	t_list * entrenadores_blocked_normal = encontrar_entrenadores_en_estado(BLOCKED_NORMAL,entrenadores);

	list_add_all(entrenadores_disponibles, entrenadores_blocked_normal);

	return entrenadores_disponibles;
}

t_list * encontrar_entrenadores_en_estado(t_estado estado_buscado,t_list * entrenadores){

	bool esta_en_estado_correspondiente_aux(void * elemento){
		t_entrenador * entrenador_estado = elemento;
		return entrenador_estado->estado == estado_buscado;
	}

	t_list * entrenadores_en_estado = list_filter(entrenadores,esta_en_estado_correspondiente_aux);

	return entrenadores_en_estado;
}

t_list * obtener_entrenadores_con_espacio(t_list * entrenadores){
	bool tiene_espacio_disponible(void * elemento) {
		t_entrenador * entrenador = elemento;
		int size_capturados = list_size(entrenador->pokemones_capturados);
		int size_por_capturar = list_size(entrenador->pokemones_por_capturar);
		return size_capturados < size_por_capturar;
	}
	return list_filter(entrenadores,tiene_espacio_disponible);

}

t_entrenador * hallar_entrenador_mas_cercano(int posx, int posy, t_list *entrenadores) {

	bool menor_distancia(void*elemento_1, void*elemento_2) {
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return distancia(entrenador_1, posx, posy)
				> distancia(entrenador_2, posx, posy);
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


// AUXILIARES A REVISAR

// TODO ver en que archivo poner cada funcion
bool es_id_necesario(int id_a_chequear){

	bool esta_id_en_lista(void * elemento){
		int * id_de_lista = elemento;
		return *(id_de_lista) == id_a_chequear;
	}

	bool resultado_busqueda_id = list_any_satisfy(ids_mensajes_utiles,esta_id_en_lista);

	return resultado_busqueda_id;
}

t_catch_pokemon * encontrar_en_lista_de_catch_pokemon (char * pokemon_a_encontrar){

	bool buscar_pokemon(void * elemento){
		t_catch_pokemon * catch_de_lista = elemento;
		return !strcasecmp(catch_de_lista->pokemon,pokemon_a_encontrar);
	}

	t_catch_pokemon * catch_objetivo = list_find(lista_de_catch,buscar_pokemon);

	if (catch_objetivo == NULL)
	{
		log_trace(logger, "El pokemon no se encontraba en la lista de catch");

		return catch_objetivo;
	}else
	{
		return catch_objetivo;
	}

}

t_catch_pokemon * de_appeared_a_catch(t_appeared_pokemon * appeared){

	t_catch_pokemon * mensaje_catch = crear_catch_pokemon(
				appeared->pokemon,
				appeared->posx,
				appeared->posy, -30);

	return mensaje_catch;

}


t_list * lista_de_catch_a_partir_localized(t_localized_pokemon * localized_a_chequear){

	t_list * lista_catchs_localized = list_create();

	char * pokemon_de_catch = localized_a_chequear->pokemon;


	void generar_catch_por_posicion(void * elemento){
		t_posicion * posicion_de_lista = elemento;

		t_catch_pokemon * catch_a_agregar = crear_catch_pokemon(
		pokemon_de_catch,
		posicion_de_lista->x,
		posicion_de_lista->y,
		-30);

		list_add(lista_catchs_localized,catch_a_agregar);

	}

	list_iterate(localized_a_chequear->posiciones,generar_catch_por_posicion);

	return lista_catchs_localized;
}

t_entrenador * buscar_entrenador_segun_catch(t_catch_pokemon * catch_buscado){

	bool buscar_entrenador_con_catch(void * elemento){

		t_entrenador * cada_entrenador = elemento;

		int id_mensaje_entrenador = cada_entrenador->catch_pendiente->id_mensaje;

		return id_mensaje_entrenador == catch_buscado->id_mensaje;
	}

	t_entrenador * entrenador_buscado =list_find(head_entrenadores,buscar_entrenador_con_catch);

	if(entrenador_buscado==NULL){
		log_trace(logger,"El catch no esta pendiente para ningun entrenador");
		return entrenador_buscado;
	}else
	{
		return entrenador_buscado;
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

// Funcion de Planificacion de entrenadores
/*
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


 void comenzar_planificacion_entrenadores(t_appeared_pokemon * appeared_recibido){
 t_entrenador *entrenador_a_planificar= hallar_entrenador_mas_cercano_segun_appeared(appeared_recibido);
 desbloquear_entrenador(entrenador_a_planificar);
 mover_entrenador_a_posicion(entrenador_a_planificar,appeared_recibido->posx,appeared_recibido->posy); // Comento para poder testear sin modificar entrenadores

 log_trace(logger,"Aca ejecutaria envio de catch pokemon");

 atrapar_pokemon(entrenador_a_planificar,appeared_recibido); //TODO lanzar mensaje catch_pokemon

 }
 */


void lanzar_hilo_entrenador(void*element) {
	t_entrenador * entrenador = element;
	pthread_t hilo_entrenador;

	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,
			(void*) entrenador);

	(result != 0)?
			log_error(logger, "Error lanzando el hilo"):
			log_trace(logger, "Entrenador lanzado: Pos %i %i", entrenador->posicion[0], entrenador->posicion[1]);
}

