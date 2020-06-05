#include "broker.h"

 int asignar_id_appeared_pokemon(t_appeared_pokemon* mensaje){
	 int id = id_appeared_pokemon;
	 id_appeared_pokemon++;
	 mensaje->id_mensaje = id;
	 return id;
 }

void devolver_appeared_pokemon(int socket_cliente ,t_appeared_pokemon mensaje_appeared_pokemon){
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(mensaje_appeared_pokemon);
	enviar_mensaje(socket_cliente, mensaje_serializado, APPEARED_POKEMON);
}

/*
 void almacenar_en_cola_appeared_pokemon(t_mensaje_appeared_pokemon mensaje){

 //TODO
 enviar_a_todos_los_subs_appeared_pokemon(mensaje);
 return 0;
 }
 void enviar_a_todos_los_subs_appeared_pokemon(mensaje){
 //TODO
 return 0;
 }
 void cachear_appeared_pokemon(t_mensaje_appeared_pokemon mensaje){
 //TODO
 return 0;
 }
*/
