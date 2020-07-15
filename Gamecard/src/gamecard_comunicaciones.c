#include "gamecard.h"
// Funciones generales

int iniciar_conexion_broker() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	return iniciar_conexion(ip_broker, puerto_broker);
}

void suscribirse_a_colas_gamecard() {
	enviar_suscripcion_al_broker(NEW_POKEMON);
	enviar_suscripcion_al_broker(CATCH_POKEMON);
	enviar_suscripcion_al_broker(GET_POKEMON);
	while (1) {
		sleep(config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION"));
		sem_wait(&suscribir);
		log_info(logger, "Inicio reintento de comunicacion con Broker");
		enviar_suscripcion_al_broker(NEW_POKEMON);
		enviar_suscripcion_al_broker(CATCH_POKEMON);
		enviar_suscripcion_al_broker(GET_POKEMON);

		int val_semaforo;
		sem_getvalue(&suscripcion, &val_semaforo);
		if(val_semaforo > 0)
			log_info(logger, "Reintento de comunicación con broker fallido");
		else
			log_info(logger, "Reintento de comunicación con broker logrado");
	}
}

void enviar_suscripcion_al_broker(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_broker();
	if (socket_broker == -1){
		reintento_suscripcion_si_aplica();
	}else {
		enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);
		int* argument = malloc(sizeof(int));
		*argument = socket_broker;
		pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);
		log_trace(logger, "Suscripcion completada");
	}
}


void manejar_recibo_mensajes_broker(void *arg) {
	int conexion = *((int*)arg);
	log_trace(logger, "Aceptando broker...");
	int cod_op = 1;

	while (cod_op > 0) { // No es espera activa porque queda en recv
			cod_op = recibir_codigo_operacion(conexion);
			if (cod_op > 0){
				log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
				handle_mensajes_gamecard(conexion,cod_op,0);
			}else{
				log_error(logger, "Error en 'recibir_codigo_operacion'");
				reintento_conexion_broker();
				return;
			}
		}
}




void handle_mensajes_gamecard(op_code cod_op, int* socket,int flag_respuesta){
	t_buffer * buffer = recibir_mensaje(*socket);
	int id_mensaje;

	switch(cod_op){
	case NEW_POKEMON: // deserializarlo aca y que gamecard_manejar_..._pokemon reciba su tipo de pokemon (ej t_new_pokemon)?
		t_new_pokemon *mensaje_new = deserializar_new_pokemon(buffer);
		log_info(logger, "Se recibio un mensaje NEW_POKEMON %s", mostrar_new_pokemon(mensaje_new)); // capaz se repite
		gamecard_manejar_new_pokemon(mensaje_new);
		break;

	case CATCH_POKEMON:
		t_catch_pokemon * mensaje_catch = deserializar_catch_pokemon(buffer);
		log_trace(logger, "Se recibio un mensaje CATCH_POKEMON %s",mostrar_catch_pokemon(mensaje_catch)); // capaz se repite
		gamecard_manejar_catch_pokemon(mensaje_catch);
		break;

	case GET_POKEMON:
		t_get_pokemon * mensaje_get = deserializar_get_pokemon(buffer);
		log_trace(logger, "Se recibio un mensaje GET_POKEMON %s", mostrar_get_pokemon(mensaje_get)); // capaz se repite
		//gamecard_manejar_get_pokemon(info_mensaje_a_manejar);
		break;

	default:
		log_warning(logger, "El cliente %i cerro el socket.", socket);
		break;
	}
	if (flag_respuesta) {
			//list_add(ids_mensajes_utiles, &id_mensaje);
		} else {
			//confirmar_recepcion(conexion, cod_op, id_mensaje);

			log_trace(logger, "Recepcion confirmada: %d %d %d", conexion, cod_op, id_mensaje);
		}
}



void enviar_appeared_pokemon_a_broker( void *element) {
	t_appeared_pokemon * appeared_a_enviar = element;
	int socket_broker = iniciar_conexion_con_broker();
		if (socket_broker > 0) {
			t_buffer*mensaje_appeared_serializado = serializar_appeared_pokemon(appeared_a_enviar);

			enviar_mensaje(socket_broker, mensaje_appeared_serializado, APPEARED_POKEMON);

			log_trace(logger, "Enviado catch para: %s",
					appeared_a_enviar->pokemon);

			free(mensaje_appeared_serializado);

			appeared_a_enviar->id_mensaje = manejar_recibo_mensajes(
					socket_broker, recibir_codigo_operacion(socket_broker), 1);
			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar appeared. Se efectuara operacion default");
			//TODO: CHEQUEAR FUNCION DEFAULT
		}
}



void gamecard_manejar_new_pokemon(t_new_pokemon *mensaje_new){
	// TODO: CHEQUEAR COSAS QUE HAY QUE HACER
	char* file = pokemon_metadata_path(mensaje_new->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,mensaje_new->pokemon);
	// Verificar si se puede abrir el archivo
	// mutex lock
	if (!file_open(mensaje_new->pokemon)){ // SEMAFORO MUTEX PARA MANEJAR LOS OPEN
		//Buscar los bloques del pokemon
		char** bloques = extraer_bloques(mensaje_new->pokemon);
		if(verificar_posiciones_file(mensaje_new->posx,mensaje_new->posy,bloques)){
		}
	}
	// mutex unlock
		// Reintentar la operación luego de REINTENTO_OPERACION
	// Verificar si las posiciones ya existen dentro del archivo
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar el archivo
	// Conectarse al broker y enviar a APPEARED_POKEMON un mensaje con ID del mensaje recibido, pokemon, posicion en el mapa
	//enviar_appeared_pokemon_a_broker(BROKER,pokemon);
	// En caso de no poder conectarse avisar por log
}

void gamecard_manejar_catch_pokemon(t_catch_pokemon * mensaje_catch){
	// TODO: VER COMO MANEJAR EL CATCH_POKEMON
	char* file = pokemon_metadata_path(mensaje_catch->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,mensaje_catch->pokemon);
	// Verificar si se puede abrir el archivo
	// Verificar si las posiciones existen dentro del archivo
	// En caso de que la cantidad sea 1 -> eliminar la linea, en caso contrario se debe decrementar una unidad
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje indicando ID del mensaje  y resultado a CAUGHT_POKEMON.
	// Si no se puede conectar al broker informar por log y continuar
}

void gamecard_manejar_get_pokemon(t_get_pokemon * mensaje_get){

	char* file = pokemon_metadata_path(mensaje_get->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,mensaje_get->pokemon);
	// Verificar si se puede abrir el archivo
	// Obtener todas las posiciones y cantidades del pokemon requerido
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje con todas las posiciones y su cantidad
	// En caso de que se encuentre por lo menos una posicion para el pokemon solicitado se debera enviar un mensaje al broker a LOCALIZED_POKEMON indicando
	// 	ID del mensaje, el pokemon solicitado y la lista de posiciones y cantidad de posiciones X e Y de cada una de ellas
	// Si no se puede conectar al broker informar por log y continuar
}

//FUNCION AUXILIAR
t_appeared_pokemon* convertir_a_appeared_pokemon(t_new_pokemon* pokemon){ // ver utilidad de esta funcion
	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon -> id_mensaje = pokemon->id_mensaje;
	appeared_pokemon -> pokemon = pokemon->pokemon;
	appeared_pokemon -> posx = pokemon->posx;
	appeared_pokemon -> posy = pokemon->posy;
	appeared_pokemon -> size_pokemon = pokemon -> size_pokemon;
	return appeared_pokemon;
}

