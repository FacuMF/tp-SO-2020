#ifndef UTILS_UTILS_MENSAJES_H_
#define UTILS_UTILS_MENSAJES_H_

#include "utils.h"

// ESTRUCTURAS DE MENSAJES

// Mensajes sin id
typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int cantidad;
}t_new_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
}t_catch_pokemon;

typedef struct{
	int id_mensaje;
	bool ok_or_fail;
}t_caugth_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
}t_get_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
}t_appeared_pokemon;

// Mensajes con id

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int cantidad;
	int id_mensaje;
}t_id_new_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int id_mensaje;
}t_id_catch_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int id_mensaje;
}t_id_appeared_pokemon;

//Mensaje subscriptor

typedef struct{
	int cola_de_mensaje;
	int tiempo;
}t_subscriptor;


typedef struct{ //TEST MESSAGE STRUCT
	char* contenido;
}t_msjTexto;

// SERIALIZADORES
t_buffer* serializar_mensaje(t_msjTexto*);// DE UNA ESTRUCTURA CON EL MENSAJE, SACA UN BUFFER


t_msjTexto* deserializar_mensaje(t_buffer* );
t_appeared_pokemon* deserializar_appeared_pokemon(t_buffer* buffer);

// CREADORES
t_msjTexto* crear_mensaje(char*);

//CREACION DE DISTINTOS TIPOS DE MENSAJES
t_buffer* crear_serializar_new_pokemon(char* pokemon, int posicion_x, int posicion_y, int cantidad, int id_mensaje);
t_id_new_pokemon* deserializar_new_pokemon(t_buffer* buffer);

//t_buffer* crear_serializar_appeared_pokemon(char* pokemon, int posicion_x, int posicion_y, int id_mensaje);
//t_id_appeared_pokemon* deserializar_appeared_pokemon(t_buffer* buffer);


t_buffer* crear_serializar_catch_pokemon(char* pokemon, int posicion_x, int posicion_y, int id_mensaje);
t_id_catch_pokemon* deserializar_catch_pokemon(t_buffer* buffer);

t_buffer* crear_serializar_caught_pokemon(int id_mensaje, int ok_fail);
t_caugth_pokemon* deserializar_caught_pokemon(t_buffer* buffer);

t_buffer* crear_serializar_get_pokemon(char* pokemon);
t_get_pokemon* deserializar_get_pokemon(t_buffer* buffer);

t_buffer* crear_serializar_suscripcion(int cola_de_mensajes, int tiempo_de_suscripcion);
t_subscriptor* deserializar_suscripcion(t_buffer* buffer);

#endif /* UTILS_UTILS_MENSAJES_H_ */
