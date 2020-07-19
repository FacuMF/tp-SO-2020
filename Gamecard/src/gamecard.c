#include "gamecard.h"

int main(int argv, char*archivo_config[]){
	iniciar_gamecard(archivo_config);

	//pthread_create(&thread,NULL,(void*) suscribirse_a_colas_gamecard,NULL);

	pthread_create(&thread, NULL, (void*) iniciar_conexion_con_gameboy_gamecard, NULL);
	sleep(100);// TBR
	finalizar_gamecard();
}


void iniciar_gamecard(char*argumentos_iniciales[]){
		//Config
		iniciar_config_gamecard(argumentos_iniciales[1]);
		//logger
		iniciar_logger_gamecard();

		iniciar_chars_necesarios();
		set_base_path(config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));
		iniciar_semaforos_gamecard();
		bitmap_bloques = bitarray_create("bitmap_bloques",tamanio_bloque()/8);
}

void finalizar_gamecard(){
	// TODO : Destroy todos los semaforos
	// TODO : Ver si todos los hilos estan cerrados.
	terminar_logger(logger);
	config_destroy(config);
}
