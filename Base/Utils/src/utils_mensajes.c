#include "utils_mensajes.h"

t_buffer* serializar_mensaje(t_msjTexto* mensaje) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = strlen(mensaje->contenido) + 1;
	buffer->stream = malloc(buffer->size);
	buffer->stream = mensaje->contenido;

	return buffer;
}


t_appeared_pokemon* deserializar_appeared_pokemon(t_buffer* buffer) {
	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));
	void* stream = buffer->stream;

	//Deserializacion
	memcpy( &(mensaje->size_pokemon), stream, sizeof(int));
	stream += sizeof(int);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy( (mensaje->pokemon), stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy( &(mensaje->posx), stream, sizeof(int));
	stream += sizeof(int);

	memcpy( &(mensaje->posy), stream, sizeof(int));
	stream += sizeof(int);


	return mensaje;
}

t_msjTexto* deserializar_mensaje(t_buffer* buffer) {
	t_msjTexto* mensaje = malloc(sizeof(t_msjTexto));
	mensaje->contenido =malloc(buffer->size);
	strcpy(mensaje->contenido,(char*)buffer->stream);

	return mensaje;
}

t_msjTexto* crear_mensaje(char* contenido){
	t_msjTexto* mensaje_test = malloc(sizeof(t_msjTexto));
	mensaje_test->contenido = malloc(strlen(contenido) + 1);
	//strcpy(mensaje_test->contenido,contenido);

	mensaje_test->contenido = contenido;

	return mensaje_test;
}
