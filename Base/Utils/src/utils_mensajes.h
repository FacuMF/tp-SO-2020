#ifndef UTILS_UTILS_MENSAJES_H_
#define UTILS_UTILS_MENSAJES_H_

#include "utils.h"

// ESTRUCTURAS DE MENSAJES

// Mensajes sin id
typedef struct{
	char* pokemon;
	int posx;
	int posy;
	int cantidad;
}t_new_pokemon;

typedef struct{
	char* pokemon;
	int posx;
	int posy;
}t_catch_pokemon;

typedef struct{
	int id_mensaje;
	bool ok_or_fail;
}t_caugth_pokemon;

typedef struct{
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
	char* pokemon;
	int posx;
	int posy;
	int cantidad;
	int id_mensaje;
}t_id_new_pokemon;

typedef struct{
	char* pokemon;
	int posx;
	int posy;
	int id_mensaje;
}t_id_catch_pokemon;

typedef struct{
	char* pokemon;
	int posx;
	int posy;
	int id_mensaje;
}t_id_appeared_pokemon;

//Mensaje subscriptor

typedef struct{
	char* cola_de_mensaje;
	int tiempo;
}t_subscriptor;


typedef struct{ //TEST MESSAGE STRUCT
	char* contenido;
}t_msjTexto;

// SERIALIZADORES
t_buffer* serializar_mensaje(t_msjTexto*);// DE UNA ESTRUCTURA CON EL MENSAJE, SACA UN BUFFER
t_msjTexto* deserializar_mensaje(t_buffer* );

// CREADORES
t_msjTexto* crear_mensaje(char*);

#endif /* UTILS_UTILS_MENSAJES_H_ */
