#include "team.h"

// GAMEBOY

void iniciar_conexion_con_gameboy() {
	log_trace(logger, "Iniciar conexion con gameboy");
	char * ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	char * puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy, puerto_gameboy);

	while (1) {
		log_trace(logger, "Esperando Cliente");
		esperar_cliente(socket_gameboy);
	}
}

void esperar_cliente(int socket_servidor) {	// Hilo coordinador

	int socket_cliente = aceptar_cliente(socket_servidor);
	log_trace(logger, "Cliente aceptado: %d.", socket_cliente);

	int * argument = malloc(sizeof(int));
	*argument = socket_cliente;
	pthread_mutex_lock(&mutex_hilos);
	pthread_create(&thread, NULL, (void*) esperar_mensaje_gameboy, argument);
	pthread_detach(thread);
	pthread_mutex_unlock(&mutex_hilos);
}

void esperar_mensaje_gameboy(void* input){
	int conexion = *((int *) input);
	free(input);

	int cod_op = recibir_codigo_operacion(conexion);
	if (cod_op > 0)
		manejar_recibo_mensajes(conexion, cod_op, 0);
	else
		log_error(logger, "Error en 'recibir_codigo_operacion'");
}

// BROKER

int iniciar_conexion_con_broker() {
	int result = iniciar_conexion(ip_broker, puerto_broker);
	if(result<0)
		return -1;
	else
		return result;
}


// MANEJO MENSAJES
void esperar_mensajes_cola(void* input) {
	int conexion = *((int *) input);
	free(input);

	int cod_op = 1;

	while (cod_op > 0) { // No es espera activa porque queda en recv
		cod_op = recibir_codigo_operacion(conexion);
		if (cod_op > 0){
			log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
			manejar_recibo_mensajes(conexion, cod_op, 0);
		}else{
			log_error(logger, "Error en 'recibir_codigo_operacion'");
			reintento_suscripcion_si_aplica();
			close(conexion);
			return;
		}
	}
}

int manejar_recibo_mensajes(int conexion, op_code cod_op, int es_respuesta) {
	t_buffer * buffer = recibir_mensaje(conexion);
	int id_mensaje;
	char * cadena_mensaje;

	switch (cod_op) {
	case APPEARED_POKEMON:
		;
		t_appeared_pokemon * mensaje_appeared = deserializar_appeared_pokemon(buffer);
		id_mensaje = mensaje_appeared->id_mensaje;

		cadena_mensaje =mostrar_appeared_pokemon(mensaje_appeared);
		log_info(logger, "Mensaje APPEARED_POKEMON: %s",cadena_mensaje);

		manejar_appeared(mensaje_appeared);

		break;
	case CAUGHT_POKEMON:
		;
		t_caught_pokemon* mensaje_caught= deserializar_caught_pokemon(buffer);
		id_mensaje = mensaje_caught->id_correlativo;

		cadena_mensaje =mostrar_caught_pokemon(mensaje_caught);
		log_info(logger, "Mensaje CAUGHT_POKEMON: %s",cadena_mensaje );

		manejar_caught(mensaje_caught,NULL);

		break;
	case LOCALIZED_POKEMON:
		;
		t_localized_pokemon* mensaje_localized= deserializar_localized_pokemon(buffer);
		id_mensaje = mensaje_localized->id_correlativo;

		cadena_mensaje = mostrar_localized(mensaje_localized);
		log_info(logger, "Mensaje LOCALIZED_POKEMON: %s",cadena_mensaje);

		manejar_localized(mensaje_localized);

		break;
	case GET_POKEMON:
		;
		t_get_pokemon* mensaje_get= deserializar_get_pokemon(buffer);
		id_mensaje = mensaje_get->id_mensaje;

		cadena_mensaje = mostrar_get_pokemon(mensaje_get);
		log_info(logger, "Mensaje GET_POKEMON: %s",cadena_mensaje);
		liberar_mensaje_get_pokemon(mensaje_get);


		log_debug(logger,"Recepcion id_mensaje: %d",id_mensaje);

		break;
	case CATCH_POKEMON:
		;
		t_catch_pokemon* mensaje_catch= deserializar_catch_pokemon(buffer);
		id_mensaje = mensaje_catch->id_mensaje;

		cadena_mensaje = mostrar_catch_pokemon(mensaje_catch);
		log_info(logger, "Mensaje CATCH_POKEMON: %s",cadena_mensaje);
		liberar_mensaje_catch_pokemon(mensaje_catch);

		log_trace(logger,"Recepcion id_mensaje: %d",id_mensaje);

		break;
	default:
		log_error(logger, "Opcode inválido.");
		break;
	}
	free(cadena_mensaje);

	if (es_respuesta) {
		pthread_mutex_lock(&mutex_ids_mensajes);
		list_add(ids_mensajes_utiles, (void*)id_mensaje);
		pthread_mutex_unlock(&mutex_ids_mensajes);
	} else {
		confirmar_recepcion(conexion, cod_op,id_mensaje);

		log_trace(logger, "Recepcion confirmada: %d %d %d", conexion, cod_op, id_mensaje);
	}

	log_trace(logger, "Mensaje recibido manejado.");


	return id_mensaje;
}
