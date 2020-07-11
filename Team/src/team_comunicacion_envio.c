#include "team.h"


// SUSCRIPCION
void suscribirse_a_colas_necesarias() {
	enviar_suscripcion_broker(APPEARED_POKEMON);
	enviar_suscripcion_broker(LOCALIZED_POKEMON);
	enviar_suscripcion_broker(CAUGHT_POKEMON);
}

void enviar_suscripcion_broker(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_con_broker_reintento();

	enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);

	int* argument = malloc(sizeof(int));
	*argument = socket_broker;
	pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);

	log_trace(logger, "Suscripcion completada");
}

void enviar_mensaje_suscripcion(op_code mensaje, int conexion) {
	t_subscriptor* mensaje_suscripcion;
	mensaje_suscripcion = crear_suscripcion(mensaje, -10);

	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_suscripcion(mensaje_suscripcion);

	enviar_mensaje(conexion, mensaje_serializado, SUSCRIPTOR);
	log_trace(logger, "Mensaje suscripcion enviado");
}

// OTROS

void enviar_requests_pokemones() {
	list_iterate(obtener_pokemones_necesitados_sin_repetidos(), enviar_mensaje_get);
}

void enviar_mensaje_get(void*element) {

	int socket_broker = iniciar_conexion_con_broker();

	if(socket_broker >0){
		char* pokemon = element;
		t_get_pokemon * mensaje_get = crear_get_pokemon(pokemon, -10);

		t_buffer* mensaje_serializado = serializar_get_pokemon(mensaje_get);

		enviar_mensaje(socket_broker, mensaje_serializado, GET_POKEMON);

		log_trace(logger, "Enviado get para: %s", pokemon);

		free(mensaje_serializado);

		manejar_recibo_mensajes(socket_broker,
				recibir_codigo_operacion(socket_broker), 1);

		close(socket_broker);
	}
}

void enviar_mensaje_catch(void * element) { //mismo que get
	t_entrenador * entrenador = element;

	int socket_broker = iniciar_conexion_con_broker();
	t_catch_pokemon * mensaje_catch_a_enviar = entrenador->catch_pendiente;
	if(socket_broker >0){
		t_buffer*mensaje_catch_serializado = serializar_catch_pokemon(
				mensaje_catch_a_enviar);

		enviar_mensaje(socket_broker, mensaje_catch_serializado, CATCH_POKEMON);

		log_trace(logger, "Enviado catch para: %s",
				mensaje_catch_a_enviar->pokemon);

		free(mensaje_catch_serializado);

		mensaje_catch_a_enviar->id_mensaje = manejar_recibo_mensajes(socket_broker,
				recibir_codigo_operacion(socket_broker), 1);

		close(socket_broker);
	}else{
		t_caught_pokemon * mensaje_caught = crear_caught_pokemon(99,1);
		manejar_caught(mensaje_caught,entrenador);
	}

}
