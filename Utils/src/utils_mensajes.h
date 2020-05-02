#ifndef UTILS_UTILS_MENSAJES_H_
#define UTILS_UTILS_MENSAJES_H_

#include "utils.h"

// ESTRUCTURAS DE MENSAJES

typedef struct{ //TEST MESSAGE STRUCT
	char* contenido;
}t_msjTexto;

// SERIALIZADORES
t_buffer* serializar_mensaje(t_msjTexto*);// DE UNA ESTRUCTURA CON EL MENSAJE, SACA UN BUFFER
t_msjTexto* deserializar_mensaje(t_buffer* );

// CREADORES
t_msjTexto* crear_mensaje(char*);

#endif /* UTILS_UTILS_MENSAJES_H_ */
