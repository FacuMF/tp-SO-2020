#include "broker.h"

int main(void) {
	inicializacion_broker();
	lanzar_hilo_receptor_mensajes();
	terminar_proceso();

	return 0;
}