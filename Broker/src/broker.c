#include "broker.h"

int main(void) {
	// Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log","Team");
	log_trace(logger,"--- Log inicializado ---");

	// Leer configuracion
	config =leer_config("./Broker/config/broker.config");
	log_info(logger,"Config creada");

	// Inicializacion de las distintas colas de mensajes

		//  NEW_POKEMON
	t_queue* new_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  APPEARED_POKEMON
	t_queue* appeared_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  CATCH_POKEMON
	t_queue* catch_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  CAUGHT_POKEMON
	t_queue* caught_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  GET_POKEMON
	t_queue* get_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();
		//  LOCALIZED_POKEMON
	t_queue* localizad_pokemon = malloc(sizeof(t_queue));
	new_pokemon->subscriptores = list_create();
	new_pokemon->mensajes = list_create();



	//  Administrar Subsciptores

		// Espera solicitudes de distintos modulos.

		// Lista de subscriptores por cada cola que administra.

		// Enviar a nuevos subscriptores, los mensajes cacheados



	//  Administrar recepcion, envio y confirmacion de mensajes

		//  Recibir mensajes

			//  Analizar a que cola pertenece.

			//  Identificar unoquibocamente el mensaje (ID)

			//  Almacenar en dicha cola.

			//  Cachear mensajes

	//  Enviar a todos los subscriptores

			//  Todo mensaje debe permanecer en la cola hasta que todos sus
			//  subs lo reciban

			//  Notificacion de recepcion: Todo mnesaje debe ser confirmado
			//  por cada subscriptor, para no volver a enviarlo al mismo.

			//  La recepcion y notificacion de mensajes puede diferir en
			//  el tiempo



	//  Mantener un registro de los ultimos mensajes recibidos para
	//  futuros subs

	//  Mantener e infomrar en todo momento los estados de las colas
	//  con sus mensajes y subscriptores.

		//  Mantener su estado

		//  Borrar mensajes que fueron entregados a todos los subs.








	terminar_logger(logger);
	config_destroy(config);

}
