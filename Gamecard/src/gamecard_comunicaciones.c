#include "gamecard.h"


// Funciones generales para comunicacion con broker

void iniciar_conexion_con_gameboy_gamecard(){
	log_trace(logger, "Iniciando conexion con gameboy");
	char * ip_gameboy = config_get_string_value(config, "IP_GAMEBOY");
	char * puerto_gameboy = config_get_string_value(config, "PUERTO_GAMEBOY");

	int socket_gameboy = iniciar_conexion_servidor(ip_gameboy, puerto_gameboy);

	while (1) {
		log_trace(logger, "Esperando Cliente");
		esperar_cliente_gamecard(socket_gameboy);
	}
}
void esperar_cliente_gamecard(int socket_servidor) {

	int socket_cliente = aceptar_cliente(socket_servidor);
	log_trace(logger, "Cliente aceptado: %d.", socket_cliente);

	int * argument = malloc(sizeof(int));
	*argument = socket_cliente;
	pthread_create(&thread, NULL, (void*) esperar_mensaje_gameboy_gamecard, argument);
}

void esperar_mensaje_gameboy_gamecard(void* input){
	int conexion = *((int *) input);
	int cod_op = recibir_codigo_operacion(conexion);
	if (cod_op > 0)
		handle_mensajes_gamecard(conexion, cod_op);
	else
		log_error(logger, "Error en 'recibir_codigo_operacion'");
}




// FUNCIONES DE COMUNICACION CON BROKER
int iniciar_conexion_broker_gamecard() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	return iniciar_conexion(ip_broker, puerto_broker);
}

void suscribirse_a_colas_gamecard() {
	log_trace(logger,"Hilos de suscripcion lanzados");

	enviar_suscripcion_al_broker(NEW_POKEMON);
	enviar_suscripcion_al_broker(CATCH_POKEMON);
	enviar_suscripcion_al_broker(GET_POKEMON);

	while (1) {
		sleep(config_get_int_value(config, "TIEMPO_DE_REINTENTO_BROKER"));
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

void esperar_mensajes_gamecard(void* input) {
	int conexion = *((int *) input);
	int cod_op = 1;

	while (cod_op > 0) { // No es espera activa porque queda en recv
		cod_op = recibir_codigo_operacion(conexion);
		if (cod_op > 0){
			log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
			handle_mensajes_gamecard(conexion, cod_op);
		}else{
			log_error(logger, "Error en 'recibir_codigo_operacion'");
			reintento_suscripcion_si_aplica_gamecard();
			return;
		}
	}
}


int handle_mensajes_gamecard(int conexion, op_code cod_op){
	t_buffer * buffer = recibir_mensaje(conexion);
	int id_mensaje;

	char* tipo_mensaje = op_code_a_string(cod_op);
	log_trace(logger, "Se recibio un mensaje %s.", tipo_mensaje);
	free(tipo_mensaje);

	switch(cod_op){
	case NEW_POKEMON:
		;
		t_new_pokemon *mensaje_new = deserializar_new_pokemon(buffer);

		manejar_new_pokemon(mensaje_new);
		break;

	case CATCH_POKEMON:
		;
		t_catch_pokemon * mensaje_catch = deserializar_catch_pokemon(buffer);

		manejar_catch_pokemon(mensaje_catch);
		break;

	case GET_POKEMON:
		;
		t_get_pokemon * mensaje_get = deserializar_get_pokemon(buffer);

		manejar_get_pokemon(mensaje_get);
		break;

	default:
		log_warning(logger,"OP_CODE INVALIDO");
		break;
	}

	confirmar_recepcion(conexion, cod_op, id_mensaje);

	log_trace(logger, "Recepcion confirmada: %d %d %d", conexion, cod_op, id_mensaje);

	log_trace(logger,"Mensaje recibido manejado");

	return id_mensaje;
}






