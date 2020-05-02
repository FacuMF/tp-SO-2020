#ifndef UTILS_UTILS_MENSAJES_H_
#define UTILS_UTILS_MENSAJES_H_

#include "utils.h"

// OP CODES
typedef enum
{
	TEXTO = 1,
}op_code;

// ESTRUCTURAS DE MENSAJES
typedef struct{ //TEST MESSAGE STRUCT
	char* contenido;
}t_msjTexto;

// FUNCIONES


t_buffer* serializar_mensaje(t_msjTexto);
// DE UNA ESTRUCTURA CON EL MENSAJE, SACA UN BUFFER


/* va en utils.c comun..
void enviar_mensaje(buffer, code_op){
	//paquete = generar_paquete(t_buffer* , op_code);
	//void* a_enviar = serializar_paquete(paq, &bytes);
	//send(socket_cliente, serializado, size_serializado, 0);
}
*/

#endif /* UTILS_UTILS_MENSAJES_H_ */
