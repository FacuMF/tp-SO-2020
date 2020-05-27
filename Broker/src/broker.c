#include "broker.h"

int main(void) {
	// Inicio logger
	logger = iniciar_logger("./Broker/config/broker.log","Team");
	log_trace(logger,"--- Log inicializado ---");

	// Leer configuracion
	config =leer_config("./Broker/config/broker.config");
	log_trace(logger,"Config creada");

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

	/*
	pthread_create( iniciar_conexion() )

	iniciar_conexion(){
		while(True) {
			listen(socketEscucha, )   //Se bloquea y solo sigue si alguien se le conecta
			log( El socket esta escuchando algo, se pasa a aceptar. )
			lanzoHilo(handleMenssage)
		}
	}

	handleMenssaje() {
		int code_op = recibir_codigo_operacion();
		process_request(cod_op, *socket);
	}

	proces_request() {
		if( Subscripcion ){
			deserializar_subscription()
			agregar_subscriptor_a_cola()
			enviar_a_subscriptor_mensajes_cola() \\Se le envian todos los mensajes que ya estan almacenados en la cola.

		}
		if( Mensaje ){
			deserializar_mensjae()
			asignar_id()
			informar_id_a_cliente()
			almacenar_en_cola()
			enviar_a_todos_los_subs() \\Y esperar confirmacion
			cachear_mensaje()
		}
	}
	*/

	terminar_logger(logger);
	config_destroy(config);

}


//  REQUERIMIENTOS 
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