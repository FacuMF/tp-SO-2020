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

	result = pthread_join(hilo_entrenador, NULL);
	if (result != 0)
		log_error(logger, "Error joineando el hilo"); //TODO: revisar manejo de errores

	return;

}

void ser_entrenador(void *element) {
	t_entrenador * entrenador = element;

	while(!(objetivo_cumplido(entrenador)))
	{
		pthread_mutex_lock(&(entrenador->sem_est));
		log_trace(logger, "Data Entrenador: Posicion %i %i", entrenador->posicion[0],
					entrenador->posicion[1]);
		//ir a buscar pokemon, etc

	}
}

double distancia(t_entrenador * entrenador, double posx, double posy){
	double distancia_e= sqrt(suma_de_distancias_al_cuadrado(entrenador,posx,posy));
	return distancia_e;
}
double suma_de_distancias_al_cuadrado(t_entrenador*entrenador, double posx, double posy){
	double suma = pow(distancia_en_eje(entrenador,posx,1),2) + pow(distancia_en_eje(entrenador,posy,2),2);
	return suma;
}
double distancia_en_eje(t_entrenador *entrenador, double pose, int pos){
	double resta= entrenador->posicion[pos] - pose;
	return resta;
}

// Funciones para planificacion del entrenador

t_entrenador * hallar_entrenador_mas_cercano(t_list * head_entrenadores,double posx, double posy){

	bool menor_distancia(void*elemento_1,void*elemento_2){
		t_entrenador *entrenador_1 = elemento_1;
		t_entrenador *entrenador_2 = elemento_2;
		return distancia(entrenador_1,posx,posy) > distancia(entrenador_2,posx,posy);
	}

	t_list * entrenadores_mas_cercanos = list_sorted(head_entrenadores, menor_distancia);
	t_entrenador * entrenador_mas_cercano = list_get(entrenadores_mas_cercanos,0);

	log_trace(logger, "Posicion entrenador cercano: Posicion %i %i", entrenador_mas_cercano->posicion[0], entrenador_mas_cercano->posicion[1]);
	return entrenador_mas_cercano;
}
