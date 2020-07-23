#include "broker.h"

void manejar_mensaje_confirmacion(t_conexion_buffer *combo) {
	t_buffer * buffer = malloc(sizeof(t_buffer));
	int socket_cliente= combo->conexion;

	memcpy(buffer, combo->buffer, sizeof(t_buffer));
	free(combo->buffer);
	free(combo);

	t_confirmacion* mensaje_confirmacion = deserializar_confirmacion(buffer);

	confirmar_recepcion_en_cache(mensaje_confirmacion, socket_cliente);

	liberar_confirmacion(mensaje_confirmacion);
	pthread_exit(NULL);
}


void confirmar_recepcion_en_cache(t_confirmacion* mensaje_confirmacion, int socket_cliente){
	log_trace(logger, "se confirmo");
	pthread_mutex_lock(&mutex_memoria_cache);

	void confirmar_recepcion_de_cliente(void* particion){
		if(mensaje_confirmacion->mensaje == ((t_mensaje_cache*) particion)->id){

			list_add( ((t_mensaje_cache*) particion)->subscribers_recibidos, (void*) socket_cliente);
			log_trace(logger, "Se confirmo la recepcion del mensaje %i en cache, por el suscriptor %i.", (((t_mensaje_cache*) particion)->id), socket_cliente);

		}
	}

	list_iterate(struct_admin_cache, confirmar_recepcion_de_cliente);

	pthread_mutex_unlock(&mutex_memoria_cache);

}

t_list* get_cola_segun_tipo(int tipo_mensaje) {
	t_list* cola_de_mensajes;
	switch (tipo_mensaje) {
	case APPEARED_POKEMON:
		cola_de_mensajes = appeared_pokemon;
		break;
	case NEW_POKEMON:
		cola_de_mensajes = new_pokemon;
		break;
	case CAUGHT_POKEMON:
		cola_de_mensajes = caught_pokemon;
		break;
	case CATCH_POKEMON:
		cola_de_mensajes = catch_pokemon;
		break;
	case GET_POKEMON:
		cola_de_mensajes = get_pokemon;
		break;
	case LOCALIZED_POKEMON:
		cola_de_mensajes = localized_pokemon;
		break;
	}
	return cola_de_mensajes;
}

