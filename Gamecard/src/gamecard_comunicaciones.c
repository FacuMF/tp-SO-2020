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
	if (cod_op > 0){
		t_handle_mensajes_gamecard* arg_handle = malloc(sizeof(t_handle_mensajes_gamecard));
		arg_handle->conexion = conexion;
		arg_handle->codigo_de_operacion=cod_op;

		handle_mensajes_gamecard(arg_handle); // No hace falta lanzar hilo.

	} else {
		log_error(logger, "Error en 'recibir_codigo_operacion'");
	}
}




// FUNCIONES DE COMUNICACION CON BROKER
int iniciar_conexion_broker_gamecard() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	int result = iniciar_conexion(ip_broker, puerto_broker);

	if(result<0)
		return -1;
	else
		return result;
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

		pthread_detach(thread);
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
	free(input);

	int cod_op = 1;

	while (cod_op > 0) { // No es espera activa porque queda en recv


		cod_op = recibir_codigo_operacion(conexion);



		if (cod_op > 0 && cod_op <= 9 ){

			log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);

			t_handle_mensajes_gamecard* arg_handle = malloc(sizeof(t_handle_mensajes_gamecard));
			arg_handle->conexion = conexion;
			arg_handle->codigo_de_operacion = cod_op;

			log_debug(logger, "Codigo de operacion: %i.", arg_handle->codigo_de_operacion);

			log_trace(logger, "Se lanza el hilo: handle_mensaje_gamecard.");

			handle_mensajes_gamecard(arg_handle);

		}else{
			log_error(logger, "Error en 'recibir_codigo_operacion' %i", cod_op);

			reintento_suscripcion_si_aplica_gamecard();
			close(conexion);
			return;
		}
	}
}

void handle_mensajes_gamecard(t_handle_mensajes_gamecard* arg_handle){

	log_debug(logger, "Codigo de operacion argumento: %i.", arg_handle->codigo_de_operacion);

	int conexion = arg_handle-> conexion;
	int cod_op = arg_handle->codigo_de_operacion;

	log_debug(logger, "Codigo de operacion cod_op: %i.", cod_op);

	t_buffer * buffer = recibir_mensaje(conexion);

	int id_mensaje;

	char* tipo_mensaje = op_code_a_string(cod_op);
	log_trace(logger, "Se recibio un mensaje %s.", tipo_mensaje);
	free(tipo_mensaje);


	t_manejar_mensajes_gamecard* argumentos = malloc(sizeof(t_manejar_mensajes_gamecard));

	argumentos->cod_op = cod_op;
	argumentos->id_mensaje = id_mensaje;
	argumentos->buffer = buffer;
	argumentos->conexion = conexion;

	pthread_create(&thread, NULL, (void*) manejar_mensajes_gamecard, argumentos);
	//manejar_mensajes_gamecard(argumentos);

	free(arg_handle);
	log_trace(logger,"Mensaje recibido manejado");

}

void manejar_mensajes_gamecard(t_manejar_mensajes_gamecard* argumentos){

	int cod_op = argumentos->cod_op;
	int id_mensaje = argumentos->id_mensaje;
	t_buffer* buffer = argumentos->buffer;
	int conexion = argumentos->conexion;

	switch(cod_op){
		case NEW_POKEMON:
			;
			t_new_pokemon *mensaje_new = deserializar_new_pokemon(buffer);

			id_mensaje = mensaje_new->id_mensaje;

			manejar_new_pokemon(mensaje_new);

			break;

		case CATCH_POKEMON:
			;
			t_catch_pokemon * mensaje_catch = deserializar_catch_pokemon(buffer);

			id_mensaje = mensaje_catch -> id_mensaje;

			manejar_catch_pokemon(mensaje_catch);

			break;

		case GET_POKEMON:
			;
			t_get_pokemon * mensaje_get = deserializar_get_pokemon(buffer);

			id_mensaje = mensaje_get -> id_mensaje;

			manejar_get_pokemon(mensaje_get);

			break;

		default:

			log_warning(logger,"OP_CODE INVALIDO");
			break;
	}


	log_debug(logger, "Se va a entrar en confirmar recepcion");

	confirmar_recepcion(conexion, cod_op, id_mensaje);

	log_trace(logger, "Recepcion confirmada: %d %d %d", conexion, cod_op, id_mensaje);




}




