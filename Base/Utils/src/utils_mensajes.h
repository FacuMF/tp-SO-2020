#ifndef UTILS_UTILS_MENSAJES_H_
#define UTILS_UTILS_MENSAJES_H_

#include "utils.h"

// ESTRUCTURAS DE MENSAJES
typedef struct{
	int id_mensaje;
	bool ok_or_fail;
}t_caught_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int id_mensaje;
}t_get_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int cantidad;
	int id_mensaje;
}t_new_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int id_mensaje;
}t_catch_pokemon;

typedef struct{
	int size_pokemon;
	char* pokemon;
	int posx;
	int posy;
	int id_mensaje;
}t_appeared_pokemon;

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

//DESERIALIZAR MENSAJE SEGUN TIPO
void deserializar_tipo_mensaje(int, int, char*);

//CREACION, SERIALIZACION Y DESERIALIZACION DE DISTINTOS TIPOS DE MENSAJES
t_new_pokemon* crear_new_pokemon(char*, int, int, int, int);
t_buffer* serializar_new_pokemon(t_new_pokemon*);
t_new_pokemon* deserializar_new_pokemon(t_buffer* buffer);

t_appeared_pokemon* crear_appeared_pokemon(char*, int, int, int);
t_buffer* serializar_appeared_pokemon(t_appeared_pokemon*);
t_appeared_pokemon* deserializar_appeared_pokemon(t_buffer* buffer);


t_catch_pokemon* crear_catch_pokemon(char*, int, int, int);
t_buffer* serializar_catch_pokemon(t_catch_pokemon*);
t_catch_pokemon* deserializar_catch_pokemon(t_buffer* buffer);

t_caught_pokemon* crear_caught_pokemon(int, bool);
t_buffer* serializar_caught_pokemon(t_caught_pokemon*);
t_caught_pokemon* deserializar_caught_pokemon(t_buffer* buffer);

t_get_pokemon* crear_get_pokemon(char*,int);
t_buffer* serializar_get_pokemon(t_get_pokemon*);
t_get_pokemon* deserializar_get_pokemon(t_buffer* buffer);

t_subscriptor* crear_suscripcion(int,int);
t_buffer* serializar_suscripcion(t_subscriptor*);
t_subscriptor* deserializar_suscripcion(t_buffer* buffer);

//Muestro de mensajes
char* mostrar_new_pokemon(t_new_pokemon*);
char* mostrar_appeared_pokemon(t_appeared_pokemon*);
char* mostrar_catch_pokemon(t_catch_pokemon*);
char* mostrar_caught_pokemon(t_caught_pokemon*);
char* mostrar_get_pokemon(t_get_pokemon*);
char* mostrar_suscriptor(t_subscriptor*);


#endif /* UTILS_UTILS_MENSAJES_H_ */
