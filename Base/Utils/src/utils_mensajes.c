#include "utils_mensajes.h"

// MENSAJE COMUN

t_buffer* serializar_mensaje(t_msjTexto* mensaje) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = strlen(mensaje->contenido) + 1;
	buffer->stream = malloc(buffer->size);
	buffer->stream = mensaje->contenido;

	return buffer;
}

t_msjTexto* deserializar_mensaje(t_buffer* buffer) {
	t_msjTexto* mensaje = malloc(sizeof(t_msjTexto));
	mensaje->contenido = malloc(buffer->size);
	strcpy(mensaje->contenido, (char*) buffer->stream);
	//memcpy(mensaje->contenido, buffer->stream, buffer->size);
	//mensaje->contenido = "hola";

	return mensaje;
}

t_msjTexto* crear_mensaje(char* contenido) {
	t_msjTexto* mensaje_test = malloc(sizeof(t_msjTexto));
	mensaje_test->contenido = malloc(strlen(contenido) + 1);
	//strcpy(mensaje_test->contenido,contenido);

	mensaje_test->contenido = contenido;

	return mensaje_test;
}

void deserializar_tipo_mensaje(int cod_op, int cliente_fd,
		char* parametros_recibidos) {
	int size;
	t_buffer * buffer;
	int valor;
	char* mensaje;
	//log_trace(logger, "Codigo de operacion: %d", cod_op);
	switch (cod_op) {
	case TEXTO:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer);
		//log_trace(logger, "Mensaje Recibido.");
		//log_trace(logger, mensaje_recibido->contenido);

		t_buffer *buffer = serializar_mensaje(mensaje_recibido);
		//log_trace(logger, "Mensaje Serializado");

		enviar_mensaje(cliente_fd, buffer, TEXTO);
		//log_trace(logger, "Mensaje Enviado");

		free(buffer);
		break;
	case APPEARED_POKEMON:
		buffer = recibir_mensaje(cliente_fd);
		t_appeared_pokemon* mensaje_appeared_pokemon =
				deserializar_appeared_pokemon(buffer);
		free(mensaje_appeared_pokemon);
		free(buffer);
		break;
	case NEW_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
		//log_trace(logger, "Mensaje new pokemon recibido");
		/*log_trace(logger,
		 "Pokemon: %s, Posicion X: %d, Posicion Y: %d, Cantidad: %d, ID: %d",
		 mensaje_new_pokemon->pokemon, mensaje_new_pokemon->posx,
		 mensaje_new_pokemon->posy, mensaje_new_pokemon->cantidad,
		 mensaje_new_pokemon->id_mensaje);*/
		free(mensaje_new_pokemon);
		free(buffer);
		break;
	case CATCH_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(
				buffer);
		//log_trace(logger, "Mensaje catch pokemon recibido");
		/*log_trace(logger, "Pokemon: %s, Posicion X: %d, Posicion Y: %d, ID: %d",
		 mensaje_catch_pokemon->pokemon, mensaje_catch_pokemon->posx,
		 mensaje_catch_pokemon->posy, mensaje_catch_pokemon->id_mensaje);*/
		free(mensaje_catch_pokemon);
		free(buffer);
		break;
		break;
	case CAUGHT_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(
				buffer);
		//log_trace(logger, "Mensaje caugth pokemon recibido");
		/*log_trace(logger, "ID: %d, OK/FAIL: %d",
		 mensaje_caugth_pokemon->id_mensaje,
		 mensaje_caugth_pokemon->ok_or_fail);*/
		free(mensaje_caught_pokemon);
		free(buffer);
		break;
		break;
	case GET_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
		//log_trace(logger, "Mensaje new pokemon recibido");
		//log_trace(logger, "Pokemon: %s", mensaje_get_pokemon->pokemon);
		free(mensaje_get_pokemon);
		free(buffer);
		break;
	case SUSCRIPTOR:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_subscriptor* mensaje_suscriptor = deserializar_suscripcion(buffer);
		//log_trace(logger, "Mensaje de suscripcion recibido");
		/*log_trace(logger,
		 "Cola de mensajes: %d, tiempo de suscripcion: %d segundos",
		 mensaje_suscriptor->cola_de_mensaje,
		 mensaje_suscriptor->tiempo);*/
		free(mensaje_suscriptor);
		free(buffer);
		break;

	case 0:
		//log_trace(logger, "Codigo invalido");
		pthread_exit(NULL);
		break;
	case -1:
		pthread_exit(NULL);
		break;
	}
}

/*--------TODO: AGREGAR FREES A CADA FUNCION--------------*/
// MENSAJE NEW POKEMON
t_new_pokemon* crear_new_pokemon(char* pokemon, int posicion_x, int posicion_y,
		int cantidad, int id_mensaje) {
	t_new_pokemon* mensaje = malloc(sizeof(t_new_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	strcpy(mensaje->pokemon, pokemon);
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->cantidad = cantidad;
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_new_pokemon(t_new_pokemon* mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 5 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(mensaje->size_pokemon);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->posx), sizeof(mensaje->posx));
	offset += sizeof(mensaje->posx);

	memcpy(stream + offset, &(mensaje->posy), sizeof(mensaje->posy));
	offset += sizeof(mensaje->posy);

	memcpy(stream + offset, &(mensaje->cantidad), sizeof(mensaje->cantidad));
	offset += sizeof(mensaje->cantidad);

	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));

	buffer->stream = stream;
	return buffer;
}
t_new_pokemon* deserializar_new_pokemon(t_buffer* buffer) {
	t_new_pokemon* mensaje = (t_new_pokemon*) malloc(sizeof(t_new_pokemon));
	void* stream = buffer->stream;
	//Deserializacion
	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(mensaje->posx));
	stream += sizeof(mensaje->posx);

	memcpy(&(mensaje->posy), stream, sizeof(mensaje->posy));
	stream += sizeof(mensaje->posy);

	memcpy(&(mensaje->cantidad), stream, sizeof(mensaje->cantidad));
	stream += sizeof(mensaje->cantidad);

	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));

	return mensaje;
}

//MENSAJE APPEARED POKEMON
t_appeared_pokemon* crear_appeared_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje) {
	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	mensaje->pokemon = pokemon;
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_appeared_pokemon(t_appeared_pokemon* mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 4 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(mensaje->size_pokemon);
	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;
	memcpy(stream + offset, &(mensaje->posx), sizeof(mensaje->posx));
	offset += sizeof(mensaje->posx);
	memcpy(stream + offset, &(mensaje->posy), sizeof(mensaje->posy));
	offset += sizeof(mensaje->posy);
	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));

	buffer->stream = stream;

	return buffer;
}
t_appeared_pokemon* deserializar_appeared_pokemon(t_buffer* buffer) {
	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));
	void* stream = buffer->stream;

	//Deserializacion
	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy((mensaje->pokemon), stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(mensaje->posx));
	stream += sizeof(mensaje->posx);

	memcpy(&(mensaje->posy), stream, sizeof(mensaje->posy));
	stream += sizeof(mensaje->posy);

	return mensaje;
}

//MENSAJE CATCH POKEMON
t_catch_pokemon* crear_catch_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje) {
	t_catch_pokemon* mensaje = malloc(sizeof(t_catch_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	strcpy(mensaje->pokemon, pokemon);
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_catch_pokemon(t_catch_pokemon* mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 4 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(mensaje->size_pokemon);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->posx), sizeof(mensaje->posx));
	offset += sizeof(mensaje->posx);

	memcpy(stream + offset, &(mensaje->posy), sizeof(mensaje->posy));
	offset += sizeof(mensaje->posy);

	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));

	buffer->stream = stream;
	return buffer;
}
t_catch_pokemon* deserializar_catch_pokemon(t_buffer* buffer) {
	t_catch_pokemon* mensaje = malloc(sizeof(t_catch_pokemon));
	void* stream = buffer->stream;
	//Deserializacion
	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(mensaje->posx));
	stream += sizeof(mensaje->posx);

	memcpy(&(mensaje->posy), stream, sizeof(mensaje->posy));
	stream += sizeof(mensaje->posy);

	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));
	return mensaje;
}

//MENSAJE CAUGTH POKEMON
t_caught_pokemon* crear_caught_pokemon(int id_mensaje, bool ok_fail) {
	t_caught_pokemon* mensaje = malloc(sizeof(t_caught_pokemon));
	mensaje->id_mensaje = id_mensaje;
	mensaje->ok_or_fail = ok_fail;

	return mensaje;
}
t_buffer* serializar_caught_pokemon(t_caught_pokemon* mensaje) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 2;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));
	offset += sizeof(mensaje->id_mensaje);

	memcpy(stream + offset, &(mensaje->ok_or_fail),
			sizeof(mensaje->ok_or_fail));
	buffer->stream = stream;

	return buffer;
}
t_caught_pokemon* deserializar_caught_pokemon(t_buffer* buffer) {
	t_caught_pokemon* mensaje = malloc(sizeof(t_caught_pokemon));
	void* stream = buffer->stream;

	//Deserializacion
	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));
	stream += sizeof(mensaje->id_mensaje);
	memcpy(&(mensaje->ok_or_fail), stream, sizeof(mensaje->ok_or_fail));

	return mensaje;
}

//MENSAJE GET POKEMON
t_get_pokemon* crear_get_pokemon(char* pokemon, int id_mensaje) {
	t_get_pokemon* mensaje = malloc(sizeof(t_get_pokemon));

	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	strcpy(mensaje->pokemon, pokemon);
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_get_pokemon(t_get_pokemon*mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = mensaje->size_pokemon + sizeof(mensaje->size_pokemon)
			+ sizeof(mensaje->id_mensaje);

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(mensaje->size_pokemon);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));

	// buffer->stream = malloc(buffer->size)
	buffer->stream = stream;
	return buffer;
}
t_get_pokemon* deserializar_get_pokemon(t_buffer* buffer) {
	t_get_pokemon* mensaje = malloc(sizeof(t_get_pokemon));

	void* stream = buffer->stream;// si no funca probar reservando previamente con malloc

	//Deserializacion
	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));

	return mensaje;

	/*
	 * t_msjTexto* mensaje = malloc(sizeof(t_msjTexto));
	 mensaje->contenido = malloc(buffer->size);
	 strcpy(mensaje->contenido, (char*) buffer->stream);
	 */

}

//MENSAJE SUSCRIPTOR
t_subscriptor* crear_suscripcion(int cola_de_mensajes,
		int tiempo_de_suscripcion) {
	t_subscriptor* mensaje = malloc(sizeof(t_subscriptor));

	mensaje->cola_de_mensaje = cola_de_mensajes;
	mensaje->tiempo = tiempo_de_suscripcion;

	return mensaje;
}
t_buffer* serializar_suscripcion(t_subscriptor* mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 2;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->cola_de_mensaje),
			sizeof(mensaje->cola_de_mensaje));
	offset += sizeof(mensaje->cola_de_mensaje);

	memcpy(stream + offset, &(mensaje->tiempo), sizeof(mensaje->tiempo));

	buffer->stream = stream;
	return buffer;
}

t_subscriptor* deserializar_suscripcion(t_buffer* buffer) {
	t_subscriptor* mensaje = malloc(sizeof(t_subscriptor));
	void* stream = buffer->stream;

	//Deserializacion
	memcpy(&(mensaje->cola_de_mensaje), stream, sizeof(int));
	stream += sizeof(int);
	memcpy(&(mensaje->tiempo), stream, sizeof(int));

	return mensaje;
}

//Muestro de mensajes

