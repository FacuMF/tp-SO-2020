#include "utils_mensajes.h"

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

	return mensaje;
}

t_msjTexto* crear_mensaje(char* contenido) {
	t_msjTexto* mensaje_test = malloc(sizeof(t_msjTexto));
	mensaje_test->contenido = malloc(strlen(contenido) + 1);
	//strcpy(mensaje_test->contenido,contenido);

	mensaje_test->contenido = contenido;

	return mensaje_test;
}

t_buffer* crear_serializar_new_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int cantidad, int id_mensaje) {
	t_id_new_pokemon* mensaje = malloc(sizeof(t_id_new_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	mensaje->pokemon = pokemon;
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->cantidad = cantidad;
	mensaje->id_mensaje = id_mensaje;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 4 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += sizeof(mensaje->pokemon);
	memcpy(stream + offset, &(mensaje->posx), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->posy), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->cantidad), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->id_mensaje), sizeof(int));

	buffer->stream = stream;

	free(mensaje->pokemon);
	free(mensaje);

	return buffer;
}

t_buffer* crear_serializar_appeared_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje) {
	t_id_appeared_pokemon* mensaje = malloc(sizeof(t_id_appeared_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	mensaje->pokemon = pokemon;
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->id_mensaje = id_mensaje;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 4 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += sizeof(mensaje->pokemon);
	memcpy(stream + offset, &(mensaje->posx), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->posy), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->id_mensaje), sizeof(int));

	buffer->stream = stream;

	free(mensaje->pokemon);
	free(mensaje);

	return buffer;
}

t_buffer* crear_serializar_catch_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje) {
	t_id_catch_pokemon* mensaje = malloc(sizeof(t_id_catch_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	mensaje->pokemon = pokemon;
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->id_mensaje = id_mensaje;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 4 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += sizeof(mensaje->pokemon);
	memcpy(stream + offset, &(mensaje->posx), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->posy), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->id_mensaje), sizeof(int));
	offset += sizeof(int);

	buffer->stream = stream;

	free(mensaje->pokemon);
	free(mensaje);

	return buffer;
}

t_buffer* crear_serializar_caught_pokemon(int id_mensaje, int ok_fail) {
	t_caugth_pokemon* mensaje = malloc(sizeof(t_caugth_pokemon));
	mensaje->id_mensaje = id_mensaje;
	mensaje->ok_or_fail = ok_fail;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 2;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->id_mensaje), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->ok_or_fail), sizeof(int));
	buffer->stream = stream;

	return buffer;
}

t_buffer* crear_serializar_get_pokemon(char* pokemon) {
	t_get_pokemon* mensaje = malloc(sizeof(t_get_pokemon));

	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(mensaje->size_pokemon);
	mensaje->pokemon = pokemon;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	buffer->stream = stream;
	return buffer;

}

t_buffer* crear_serializar_suscripcion(int cola_de_mensajes, int tiempo_de_suscripcion){
	t_subscriptor* mensaje = malloc(sizeof(t_subscriptor));

	mensaje->cola_de_mensaje = cola_de_mensajes;
	mensaje->tiempo = tiempo_de_suscripcion;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int)*2;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->cola_de_mensaje), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, &(mensaje->tiempo), sizeof(int));
	buffer->stream = stream;
	return buffer;
}
