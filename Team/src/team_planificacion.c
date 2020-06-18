#include "team.h"

// MANEJO DE HILOS
void lanzar_hilos(t_list *head_entrenadores){
	list_iterate(head_entrenadores, lanzar_hilo_entrenador);
	log_trace(logger, "Hilos lanzados");
}

void lanzar_hilo_entrenador(void*element) {
	t_entrenador * entrenador = element;

	pthread_t hilo_entrenador;
	int result = pthread_create(&hilo_entrenador, NULL, (void*) ser_entrenador,(void*)entrenador);
	if (result != 0)
		log_error(logger, "Error lanzando el hilo"); //TODO: revisar manejo de errores

	//POR AHORA NO HACE FALTA result = pthread_join(hilo_entrenador, NULL);

	if (result != 0)
		log_error(logger, "Error joineando el hilo"); //TODO: revisar manejo de errores

	log_trace(logger, "Entrenador lanzado");

	return;
}

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;
	log_info(logger,"Hola, Soy un entrenador");

	while(!(objetivo_cumplido(entrenador)))
	{
		log_trace(logger, "Data Entrenador: Posicion %i %i", entrenador->posicion[0],
					entrenador->posicion[1]);

		pthread_mutex_lock(&(entrenador->sem_est));
		//ir a buscar pokemon, etc
	/*log_debug(logger, "HOlAAAA SOY UN HIJOOOOOOO");

	while(1)
	//while(!(objetivo_cumplido(entrenador)))
	{
		log_debug(logger, "Data Entrenador: Posicion %i %i", entrenador->posicion[0],
					entrenador->posicion[1]);


		pthread_mutex_lock(&(entrenador->sem_est));*/

		//case(entrenador->tarea)
			// ir a buscar pokemon, etc
			// solucionar deadlock activo
			// solucionar deadlock pasivo
	}

}

int distancia(t_entrenador * entrenador, int posx, int posy){
	int distancia_e= abs(distancia_en_eje(entrenador,posx,1)) + abs(distancia_en_eje(entrenador,posx,2));
	return distancia_e;
}
/*double suma_de_distancias_al_cuadrado(t_entrenador*entrenador, double posx, double posy){
	double suma = pow(distancia_en_eje(entrenador,posx,1),2) + pow(distancia_en_eje(entrenador,posy,2),2);
	return suma;
}*/
int distancia_en_eje(t_entrenador *entrenador, int pose, int pos){
	int resta= entrenador->posicion[pos] - pose;
	return resta;
}

// Funciones para planificacion del entrenador
void desbloquear_entrenador(t_entrenador * entrenador){
	pthread_mutex_unlock(&(entrenador->sem_est));
}
void mover_entrenador_a_posicion(t_entrenador*entrenador,int posx, int posy ){
	int distancia_en_x = abs(distancia_en_eje(entrenador,posx,1));
	int distancia_en_y= abs(distancia_en_eje(entrenador,posy,2));
	int retardo_ciclo = config_get_int_value(config,"RETARDO_CICLO_CPU"); // TODO pasar a variable global ya que probablemente se use en varias funciones
	while(distancia_en_x !=0){
		sleep(retardo_ciclo);
		distancia_en_x --;
	}
	while(distancia_en_y !=0){
		sleep(retardo_ciclo);
		distancia_en_y --;
	}
	cambiar_posicion_entrenador(entrenador,posx,posy);
}
void cambiar_posicion_entrenador(t_entrenador*entrenador,int posx, int posy){
	entrenador->posicion[1] = posx;
	entrenador->posicion[2]=posy;
}
// Funcion de Planificacion de entrenadores
void comenzar_planificacion_entrenadores(t_appeared_pokemon * appeared_recibido,t_list * head_entrenadores){
	t_entrenador *entrenador_a_planificar= hallar_entrenador_mas_cercano_segun_appeared(appeared_recibido,head_entrenadores);
	desbloquear_entrenador(entrenador_a_planificar);
	//mover_entrenador_a_posicion(entrenador_a_planificar,appeared_recibido->posx,appeared_recibido->posy); // Comento para poder testear sin modificar entrenadores
	// atrapar_pokemon(entrenador_a_planificar,appeared_recibido); TODO lanzar mensaje catch_pokemon
}
t_entrenador * hallar_entrenador_mas_cercano_segun_appeared(t_appeared_pokemon * appeared_recibido,t_list * head_entrenadores){
	t_entrenador * entrenador_a_planificar_cercano = hallar_entrenador_mas_cercano(head_entrenadores,appeared_recibido->posx,appeared_recibido->posy);
	return entrenador_a_planificar_cercano;
}

t_entrenador * hallar_entrenador_mas_cercano(t_list * head_entrenadores,int posx, int posy){

	bool tiene_espacio_disponible(void * elemento){
		t_entrenador * entrenador = elemento;
		int size_capturados = list_size(entrenador->pokemones_capturados);
		int size_por_capturar= list_size(entrenador->pokemones_por_capturar);
		return size_capturados < size_por_capturar;
	}
	bool menor_distancia(void*elemento_1,void*elemento_2){
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return distancia(entrenador_1,posx,posy) > distancia(entrenador_2,posx,posy);
	}
	t_list *entrenadores_disponibles = list_filter(head_entrenadores,tiene_espacio_disponible);
	t_list * entrenadores_mas_cercanos = list_sorted(entrenadores_disponibles, menor_distancia);
	t_entrenador * entrenador_mas_cercano = list_get(entrenadores_mas_cercanos,0);

	log_trace(logger, "Posicion entrenador cercano: Posicion %i %i", entrenador_mas_cercano->posicion[0], entrenador_mas_cercano->posicion[1]);
	return entrenador_mas_cercano;
}

bool requiero_pokemon(t_appeared_pokemon * mensaje_appeared){

	bool pokemon_requerido = esta_pokemon_objetivo(mensaje_appeared->pokemon) && !capture_pokemon_objetivo(mensaje_appeared->pokemon);
	return pokemon_requerido;
}

bool esta_pokemon_objetivo(char *pokemon_candidato){
	t_list *lista_pokemones_objetivo_global = obtener_pokemones_de_lista_seleccionada(objetivo_global);
	bool esta = esta_en_lista(pokemon_candidato,lista_pokemones_objetivo_global);
	return esta;
	}

bool capture_pokemon_objetivo(char * pokemon_candidato){

	t_list * lista_pokemones_globales_capturados = obtener_pokemones_de_lista_seleccionada(pokemones_globales_capturados);

	if(esta_en_lista(pokemon_candidato,lista_pokemones_globales_capturados))
	{
		bool atrape_repeticiones_necesarias = pokemon_fue_atrapado_cantidad_necesaria(pokemon_candidato);
		return atrape_repeticiones_necesarias;
	}else
	{
	return false;
	}
}
bool pokemon_fue_atrapado_cantidad_necesaria(char *pokemon){
	int cantidad_en_objetivo = cantidad_pokemon_en_lista_objetivos(objetivo_global,pokemon);
	int cantidad_veces_atrapado = cantidad_pokemon_en_lista_objetivos(pokemones_globales_capturados,pokemon);
	return cantidad_en_objetivo == cantidad_veces_atrapado;
}

