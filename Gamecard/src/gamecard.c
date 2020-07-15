#include "gamecard.h"

void main(){

	iniciar_gamecard();
	pthread_create(&thread, NULL, (void*) suscribirse_a_colas_gamecard, NULL); // Doble pthread?
	//Terminar gamecard
}


void iniciar_gamecard(){
		config = leer_config("./Gamecard/config/gamecard.config");
		logger = iniciar_logger("./Gamecard/config/gamecard.log", "Gamecard", LOG_LEVEL_TRACE);
		set_base_path(config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"));
		pthread_mutex_init(&mutex_suscripcion, NULL);
		sem_init(&sem_suscripcion,1,0);
}
