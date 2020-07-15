#include "gamecard.h"
// Funciones generales

int iniciar_conexion_broker_gamecard() {
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
		sem_wait(&sem_suscripcion);
		log_info(logger, "Inicio reintento de comunicacion con Broker");
		enviar_suscripcion_al_broker(NEW_POKEMON);
		enviar_suscripcion_al_broker(CATCH_POKEMON);
		enviar_suscripcion_al_broker(GET_POKEMON);

		int val_semaforo;
		sem_getvalue(&sem_suscripcion, &val_semaforo);
		if(val_semaforo > 0)
			log_info(logger, "Reintento de comunicación con broker fallido");
		else
			log_info(logger, "Reintento de comunicación con broker logrado");
	}
}

void enviar_suscripcion_al_broker(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_broker_gamecard();
	if (socket_broker == -1){
		reintento_suscripcion_si_aplica_gamecard();
	}else {
		enviar_mensaje_suscripcion_gamecard(tipo_mensaje, socket_broker);
		int* argument = malloc(sizeof(int));
		*argument = socket_broker;
		pthread_create(&thread, NULL, (void*) esperar_mensajes_gamecard, argument);
		log_trace(logger, "Suscripcion completada");
	}
}
void reintento_suscripcion_si_aplica_gamecard(){
	pthread_mutex_lock(&mutex_suscripcion);
		int val_semaforo;
		sem_getvalue(&sem_suscripcion, &val_semaforo);
		if(val_semaforo < 1)
			sem_post(&sem_suscripcion);
	pthread_mutex_unlock(&mutex_suscripcion);
}

void enviar_mensaje_suscripcion_gamecard(op_code mensaje, int conexion){
	t_subscriptor* mensaje_suscripcion;
		mensaje_suscripcion = crear_suscripcion(mensaje, -10);

		t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
		mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

		enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
		log_trace(logger, "Mensaje suscripcion enviado");
}

void esperar_mensajes_gamecard(void* input) {
	int conexion = *((int *) input);
	int cod_op = 1;

	while (cod_op > 0) { // No es espera activa porque queda en recv
		cod_op = recibir_codigo_operacion(conexion);
		if (cod_op > 0){
			log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
			handle_mensajes_gamecard(conexion, cod_op, 0);
		}else{
			log_error(logger, "Error en 'recibir_codigo_operacion'");
			reintento_suscripcion_si_aplica_gamecard();
			return;
		}
	}
}


int handle_mensajes_gamecard(int conexion, op_code cod_op,int es_respuesta){
	t_buffer * buffer = recibir_mensaje(conexion);
	int id_mensaje;

	switch(cod_op){
	case NEW_POKEMON:
		;
		// deserializarlo aca y que gamecard_manejar_..._pokemon reciba su tipo de pokemon (ej t_new_pokemon)?
		t_new_pokemon *mensaje_new = deserializar_new_pokemon(buffer);
		log_info(logger, "Se recibio un mensaje NEW_POKEMON %s", mostrar_new_pokemon(mensaje_new)); // capaz se repite
		log_debug(logger,"Aca se maneja el new");
		//manejar_new_pokemon(mensaje_new);
		break;

	case CATCH_POKEMON:
		;
		t_catch_pokemon * mensaje_catch = deserializar_catch_pokemon(buffer);
		log_info(logger, "Se recibio un mensaje CATCH_POKEMON %s",mostrar_catch_pokemon(mensaje_catch)); // capaz se repite
		log_debug(logger,"Aca se maneja el catch");
		//manejar_catch_pokemon(mensaje_catch);
		break;

	case GET_POKEMON:
		;
		t_get_pokemon * mensaje_get = deserializar_get_pokemon(buffer);
		log_info(logger, "Se recibio un mensaje GET_POKEMON %s", mostrar_get_pokemon(mensaje_get)); // capaz se repite
		log_debug(logger, "Aca se maneja el get");
		//gamecard_manejar_get_pokemon(info_mensaje_a_manejar);
		break;

	default:
		log_error(logger,"OP_CODE INVALIDO");
		break;
	}
	if (es_respuesta) {
			// TODO: VER QUE HACER SI EL MENSAJE ES RESPUESTA
			// VER SI ES NECESARIO EL FLAG ES_RESPUESTA, SINO
			// SE CONFIRMA_RECEPCION DIRECTAMENTE.
			//list_add(ids_mensajes_utiles, &id_mensaje);
		} else {
			confirmar_recepcion(conexion, cod_op, id_mensaje);

			log_trace(logger, "Recepcion confirmada: %d %d %d", conexion, cod_op, id_mensaje);
		}
	log_trace(logger,"Mensaje recibido manejado");
	return id_mensaje;
}



void enviar_appeared_pokemon_a_broker( void *element) {
	t_appeared_pokemon * appeared_a_enviar = element;
	int socket_broker = iniciar_conexion_broker_gamecard();
		if (socket_broker > 0) {
			t_buffer*mensaje_appeared_serializado = serializar_appeared_pokemon(appeared_a_enviar);

			enviar_mensaje(socket_broker, mensaje_appeared_serializado, APPEARED_POKEMON);

			log_trace(logger, "Enviado appeared para: %s",
					appeared_a_enviar->pokemon);

			free(mensaje_appeared_serializado);

			appeared_a_enviar->id_mensaje = handle_mensajes_gamecard(
					socket_broker,recibir_codigo_operacion(socket_broker),1);
			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar appeared. Se efectuara operacion default");
			//TODO: CHEQUEAR FUNCION DEFAULT
		}
}


void enviar_caught_pokemon_a_broker( void *element) {
	t_caught_pokemon * caught_a_enviar = element;
	int socket_broker = iniciar_conexion_broker_gamecard();
		if (socket_broker > 0) {
			t_buffer*mensaje_caught_serializado = serializar_caught_pokemon(caught_a_enviar);

			enviar_mensaje(socket_broker, mensaje_caught_serializado, CAUGHT_POKEMON);

			log_trace(logger, "Enviado resultado de caught: %B",
					caught_a_enviar->ok_or_fail);

			free(mensaje_caught_serializado);

			caught_a_enviar->id_mensaje = handle_mensajes_gamecard(
					socket_broker,recibir_codigo_operacion(socket_broker),1);
			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar caught. Se efectuara operacion default");
			//TODO: CHEQUEAR FUNCION DEFAULT
		}
}

void enviar_localized_pokemon_a_broker( void *element) {
	t_localized_pokemon * localized_a_enviar = element;
	int socket_broker = iniciar_conexion_broker_gamecard();
		if (socket_broker > 0) {
			t_buffer*mensaje_localized_serializado = serializar_localized_pokemon(localized_a_enviar);

			enviar_mensaje(socket_broker, mensaje_localized_serializado, LOCALIZED_POKEMON);

			log_trace(logger, "Enviado localized para: %s. Cantidad: %d", // posiciones?
					localized_a_enviar->pokemon, localized_a_enviar->cantidad_posiciones);
			free(mensaje_localized_serializado);

			localized_a_enviar->id_mensaje = handle_mensajes_gamecard(
					socket_broker,recibir_codigo_operacion(socket_broker),1);
			close(socket_broker);
		} else {
			log_info(logger, "Error en comunicacion al intentar enviar localized. Se efectuara operacion default");
			//TODO: CHEQUEAR FUNCION DEFAULT
		}
}




