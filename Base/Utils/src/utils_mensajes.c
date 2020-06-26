#include "utils_mensajes.h"
#include "../../../Broker/src/broker.h"
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

	return mensaje;
}

t_msjTexto* crear_mensaje(char* contenido) {
	t_msjTexto* mensaje_test = malloc(sizeof(t_msjTexto));
	mensaje_test->contenido = malloc(strlen(contenido) + 1);
	//strcpy(mensaje_test->contenido,contenido);

	mensaje_test->contenido = contenido;

	return mensaje_test;
}

char* deserializar_tipo_mensaje(int cod_op, int cliente_fd) {
	int size;
	t_buffer * buffer;
	int valor;
	char* mensaje;
	char* parametros_recibidos = malloc(sizeof(char) * 100);
	switch (cod_op) {
	case TEXTO:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_msjTexto* mensaje_recibido = deserializar_mensaje(buffer);

		t_buffer *buffer = serializar_mensaje(mensaje_recibido);

		enviar_mensaje(cliente_fd, buffer, TEXTO);

		free(buffer);
		break;
	case APPEARED_POKEMON:
		buffer = recibir_mensaje(cliente_fd);
		t_appeared_pokemon* mensaje_appeared_pokemon =
				deserializar_appeared_pokemon(buffer);
		parametros_recibidos = mostrar_appeared_pokemon(
				mensaje_appeared_pokemon);
		free(mensaje_appeared_pokemon);
		free(buffer);
		break;
	case NEW_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_new_pokemon* mensaje_new_pokemon = deserializar_new_pokemon(buffer);
		parametros_recibidos = mostrar_new_pokemon(mensaje_new_pokemon);
		free(mensaje_new_pokemon);
		free(buffer);
		break;
	case CATCH_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_catch_pokemon* mensaje_catch_pokemon = deserializar_catch_pokemon(
				buffer);
		parametros_recibidos = mostrar_catch_pokemon(mensaje_catch_pokemon);
		free(mensaje_catch_pokemon);
		free(buffer);
		break;
		break;
	case CAUGHT_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_caught_pokemon* mensaje_caught_pokemon = deserializar_caught_pokemon(
				buffer);
		parametros_recibidos = mostrar_caught_pokemon(mensaje_caught_pokemon);
		free(mensaje_caught_pokemon);
		free(buffer);
		break;
		break;
	case GET_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_get_pokemon* mensaje_get_pokemon = deserializar_get_pokemon(buffer);
		parametros_recibidos = mostrar_get_pokemon(mensaje_get_pokemon);
		free(mensaje_get_pokemon);
		free(buffer);
		break;
	case SUSCRIPTOR:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_subscriptor* mensaje_suscriptor = deserializar_suscripcion(buffer);
		parametros_recibidos = mostrar_suscriptor(mensaje_suscriptor);
		free(mensaje_suscriptor);
		free(buffer);
		break;
	case CONFIRMACION:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_confirmacion* mensaje_confirmacion = deserializar_confirmacion(
				buffer);
		parametros_recibidos = mostrar_confirmacion(mensaje_confirmacion);
		free(mensaje_confirmacion);
		free(buffer);
		break;
	case LOCALIZED_POKEMON:
		;
		buffer = recibir_mensaje(cliente_fd);
		t_localized* mensaje_localized = deserializar_localized_pokemon(buffer);
		puts("pase la deserializacion");
		parametros_recibidos = mostrar_localized(mensaje_localized);
		free(mensaje_localized);
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
	return parametros_recibidos;
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
	buffer->size = sizeof(uint32_t) * 4 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	stream = serializar_cache_new_pokemon(mensaje, buffer->size);
	int offset = buffer->size;
	buffer->size += sizeof(uint32_t);
	stream = realloc(stream, buffer->size);

	memcpy(stream + offset, &(mensaje->id_mensaje), sizeof(uint32_t));

	buffer->stream = stream;
	return buffer;
}
void* serializar_cache_new_pokemon(t_new_pokemon* mensaje, int size) {

	void* stream = malloc(size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->posx), sizeof(mensaje->posx));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->posy), sizeof(mensaje->posy));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->cantidad), sizeof(mensaje->cantidad));
	offset += sizeof(uint32_t);

	return stream;
}
t_new_pokemon* deserializar_new_pokemon(t_buffer* buffer) {
	t_new_pokemon* mensaje = (t_new_pokemon*) malloc(sizeof(t_new_pokemon));
	void* stream = buffer->stream;
	mensaje = deserializar_cache_new_pokemon(stream);
	stream += 4 * sizeof(uint32_t) + mensaje->size_pokemon;
	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));

	return mensaje;
}
t_new_pokemon* deserializar_cache_new_pokemon(void* stream) {
	t_new_pokemon* mensaje = malloc(sizeof(t_new_pokemon));

	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(uint32_t);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(mensaje->posx));
	stream += sizeof(uint32_t);

	memcpy(&(mensaje->posy), stream, sizeof(mensaje->posy));
	stream += sizeof(uint32_t);

	memcpy(&(mensaje->cantidad), stream, sizeof(mensaje->cantidad));
	stream += sizeof(uint32_t);

	return mensaje;
}

//MENSAJE APPEARED POKEMON
t_appeared_pokemon* crear_appeared_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje) {
	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(sizeof(char) * mensaje->size_pokemon);
	mensaje->pokemon = pokemon;
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_appeared_pokemon(t_appeared_pokemon* mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 3 + mensaje->size_pokemon * sizeof(char);

	void* stream = malloc(buffer->size);
	stream = serializar_cache_appeared_pokemon(mensaje, buffer->size);
	int offset = buffer->size;
	buffer->size += sizeof(uint32_t);
	stream = realloc(stream, buffer->size);
	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));
	buffer->stream = stream;

	return buffer;
}
void* serializar_cache_appeared_pokemon(t_appeared_pokemon* mensaje, int size) {

	void* stream = malloc(size);
	int offset = 0;
	memcpy(stream + offset, &(mensaje->size_pokemon), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, (mensaje->pokemon), (mensaje->size_pokemon));
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->posx), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, &(mensaje->posy), sizeof(uint32_t));
	offset += sizeof(uint32_t);

	return stream;
}
t_appeared_pokemon* deserializar_appeared_pokemon(t_buffer* buffer) {
	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));
	void* stream = buffer->stream;

	mensaje = deserializar_cache_appeared_pokemon(stream);
	stream += 3 * sizeof(uint32_t) + mensaje->size_pokemon;
	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));

	return mensaje;
}
t_appeared_pokemon* deserializar_cache_appeared_pokemon(void* stream) {

	t_appeared_pokemon* mensaje = malloc(sizeof(t_appeared_pokemon));

	memcpy(&(mensaje->size_pokemon), stream, sizeof(uint32_t));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy((mensaje->pokemon), stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(uint32_t));
	stream += sizeof(mensaje->posx);

	memcpy(&(mensaje->posy), stream, sizeof(uint32_t));
	stream += sizeof(mensaje->posy);

	return mensaje;
}

//MENSAJE CATCH POKEMON
t_catch_pokemon* crear_catch_pokemon(char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje) {
	t_catch_pokemon* mensaje = malloc(sizeof(t_catch_pokemon));
	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(sizeof(char) * mensaje->size_pokemon);
	strcpy(mensaje->pokemon, pokemon);
	mensaje->posx = posicion_x;
	mensaje->posy = posicion_y;
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_catch_pokemon(t_catch_pokemon* mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 3 + mensaje->size_pokemon;

	void* stream = malloc(buffer->size);
	stream = serializar_cache_catch_pokemon(mensaje, buffer->size);
	int offset = buffer->size;
	buffer->size += sizeof(uint32_t);
	stream = realloc(stream, buffer->size);
	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));

	buffer->stream = stream;
	return buffer;
}
void* serializar_cache_catch_pokemon(t_catch_pokemon* mensaje, int size) {

	int offset = 0;
	void* stream = malloc(size);
	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(mensaje->size_pokemon);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->posx), sizeof(mensaje->posx));
	offset += sizeof(mensaje->posx);

	memcpy(stream + offset, &(mensaje->posy), sizeof(mensaje->posy));
	offset += sizeof(mensaje->posy);

	return stream;
}
t_catch_pokemon* deserializar_catch_pokemon(t_buffer* buffer) {
	t_catch_pokemon* mensaje = malloc(sizeof(t_catch_pokemon));
	void* stream = buffer->stream;

	mensaje = deserializar_cache_catch_pokemon(stream);
	stream += sizeof(uint32_t) * 3 + mensaje->size_pokemon;

	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));
	return mensaje;
}
t_catch_pokemon* deserializar_cache_catch_pokemon(void* stream) {
	t_catch_pokemon* mensaje = malloc(sizeof(t_catch_pokemon));

	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->posx), stream, sizeof(mensaje->posx));
	stream += sizeof(mensaje->posx);

	memcpy(&(mensaje->posy), stream, sizeof(mensaje->posy));
	stream += sizeof(mensaje->posy);

	return mensaje;
}

//MENSAJE CAUGTH POKEMON
t_caught_pokemon* crear_caught_pokemon(int id_mensaje, int ok_fail) {
	t_caught_pokemon* mensaje = malloc(sizeof(t_caught_pokemon));
	mensaje->id_mensaje = id_mensaje;
	mensaje->ok_or_fail = (ok_fail) ? true : false;
	return mensaje;
}
t_buffer* serializar_caught_pokemon(t_caught_pokemon* mensaje) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t);

	void* stream = malloc(buffer->size);
	stream = serializar_cache_caught_pokemon(mensaje, buffer->size);
	int offset = buffer->size;
	buffer->size += sizeof(uint32_t);
	stream = realloc(stream, buffer->size);
	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));
	offset += sizeof(mensaje->id_mensaje);

	buffer->stream = stream;

	return buffer;
}
void* serializar_cache_caught_pokemon(t_caught_pokemon* mensaje, int size) {
	void* stream = malloc(size);

	memcpy(stream, &(mensaje->ok_or_fail), sizeof(uint32_t));

	return stream;
}
t_caught_pokemon* deserializar_caught_pokemon(t_buffer* buffer) {
	t_caught_pokemon* mensaje = malloc(sizeof(t_caught_pokemon));
	void* stream = buffer->stream;

	mensaje = deserializar_cache_caught_pokemon(stream);

	stream += sizeof(uint32_t);
	memcpy(&(mensaje->id_mensaje), stream, sizeof(uint32_t));

	return mensaje;
}
t_caught_pokemon* deserializar_cache_caught_pokemon(void* stream) {
	t_caught_pokemon* mensaje = malloc(sizeof(t_caught_pokemon));
	memcpy(&(mensaje->ok_or_fail), stream, sizeof(mensaje->ok_or_fail));
	return mensaje;
}

//MENSAJE GET POKEMON
t_get_pokemon* crear_get_pokemon(char* pokemon, int id_mensaje) {
	t_get_pokemon* mensaje = malloc(sizeof(t_get_pokemon));

	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(sizeof(char) * mensaje->size_pokemon);
	strcpy(mensaje->pokemon, pokemon);
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_get_pokemon(t_get_pokemon*mensaje) {

	t_buffer* buffer = malloc(sizeof(t_buffer));

	buffer->size = mensaje->size_pokemon + sizeof(uint32_t);
	void* stream = malloc(buffer->size);
	stream = serializar_cache_get_pokemon(mensaje, buffer->size);

	int offset = buffer->size;
	buffer->size += sizeof(uint32_t);
	stream = realloc(stream, buffer->size);
	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));

	buffer->stream = stream;
	return buffer;
}
void* serializar_cache_get_pokemon(t_get_pokemon* mensaje, int size) {
	void* stream = malloc(size);
	int offset = 0;
	memcpy(stream, &(mensaje->size_pokemon), sizeof(mensaje->size_pokemon));
	offset += sizeof(uint32_t);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	return stream;
}
t_get_pokemon* deserializar_get_pokemon(t_buffer* buffer) {
	t_get_pokemon* mensaje = malloc(sizeof(t_get_pokemon));

	void* stream = buffer->stream; // si no funca probar reservando previamente con malloc

	mensaje = deserializar_cache_get_pokemon(stream);

	stream += sizeof(uint32_t) + mensaje->size_pokemon;
	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));

	return mensaje;
}
t_get_pokemon* deserializar_cache_get_pokemon(void* stream) {
	t_get_pokemon* mensaje = malloc(sizeof(t_get_pokemon));

	//Deserializacion
	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	return mensaje;
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

//MENSAJE LOCALIZED
t_localized* crear_localized_pokemon(int id_mensaje, char* pokemon,
		t_list* posiciones) {
	t_localized* mensaje = malloc(sizeof(t_localized));
	int cantidad_posiciones = list_size(posiciones);

	mensaje->size_pokemon = strlen(pokemon) + 1;
	mensaje->pokemon = malloc(sizeof(char) * mensaje->size_pokemon);
	strcpy(mensaje->pokemon, pokemon);
	mensaje->cantidad_posiciones = cantidad_posiciones;
	mensaje->posiciones = list_duplicate(posiciones);
	mensaje->id_mensaje = id_mensaje;

	return mensaje;
}
t_buffer* serializar_localized_pokemon(t_localized* mensaje) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) * 3 + mensaje->size_pokemon
			+ mensaje->cantidad_posiciones * sizeof(t_posicion);

	void* stream = malloc(buffer->size);
	stream = serializar_cache_localized_pokemon(mensaje, buffer->size);
	int offset = buffer->size;
	buffer->size += sizeof(uint32_t);
	stream = realloc(stream, buffer->size);

	memcpy(stream + offset, &(mensaje->id_mensaje),
			sizeof(mensaje->id_mensaje));
	offset += sizeof(mensaje->id_mensaje);

	buffer->stream = stream;
	return buffer;
}
void* serializar_cache_localized_pokemon(t_localized* mensaje, int size) {
	void* stream = malloc(size);
	int offset = 0;
	memcpy(stream + offset, &(mensaje->size_pokemon),
			sizeof(mensaje->size_pokemon));
	offset += sizeof(mensaje->size_pokemon);

	memcpy(stream + offset, mensaje->pokemon, mensaje->size_pokemon);
	offset += mensaje->size_pokemon;

	memcpy(stream + offset, &(mensaje->cantidad_posiciones),
			sizeof(mensaje->cantidad_posiciones));
	offset += sizeof(mensaje->cantidad_posiciones);

	for (int i = 0; i < mensaje->cantidad_posiciones; i++) {
		t_posicion* pos_aux = list_get(mensaje->posiciones, i);
		memcpy(stream + offset, pos_aux, sizeof(t_posicion));
		offset += sizeof(t_posicion);
	}

	return stream;
}
t_localized* deserializar_localized_pokemon(t_buffer* buffer) {
	t_localized* mensaje = malloc(sizeof(t_localized));
	void* stream = buffer->stream;

	mensaje = deserializar_cache_localized_pokemon(stream);
	stream += sizeof(uint32_t) * 2 + mensaje->size_pokemon + mensaje->cantidad_posiciones * sizeof(t_posicion);

	memcpy(&(mensaje->id_mensaje), stream, sizeof(mensaje->id_mensaje));

	return mensaje;
}
t_localized* deserializar_cache_localized_pokemon(void* stream) {
	t_localized* mensaje = malloc(sizeof(t_localized));

	memcpy(&(mensaje->size_pokemon), stream, sizeof(mensaje->size_pokemon));
	stream += sizeof(mensaje->size_pokemon);

	mensaje->pokemon = malloc(mensaje->size_pokemon);
	memcpy(mensaje->pokemon, stream, mensaje->size_pokemon);
	stream += mensaje->size_pokemon;

	memcpy(&(mensaje->cantidad_posiciones), stream,
			sizeof(mensaje->cantidad_posiciones));
	stream += sizeof(mensaje->cantidad_posiciones);

	mensaje->posiciones = list_create();
	for (int i = 0; i < mensaje->cantidad_posiciones; i++) {
		t_posicion* pos_aux = malloc(sizeof(t_posicion));
		memcpy(pos_aux, stream, sizeof(t_posicion));
		stream += sizeof(t_posicion);
		list_add(mensaje->posiciones, pos_aux);
	}

	return mensaje;
}

//MENSAJE CONFIRMACION
t_confirmacion* crear_confirmacion(int tipo_mensaje, int mje) {
	t_confirmacion* mensaje = malloc(sizeof(t_confirmacion));

	mensaje->tipo_mensaje = tipo_mensaje;
	mensaje->mensaje = mje;

	return mensaje;
}
t_buffer* serializar_confirmacion(t_confirmacion* mensaje) {
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) * 2;

	void* stream = malloc(buffer->size);
	int offset = 0;

	memcpy(stream + offset, &(mensaje->tipo_mensaje),
			sizeof(mensaje->tipo_mensaje));
	offset += sizeof(mensaje->tipo_mensaje);

	memcpy(stream + offset, &(mensaje->mensaje), sizeof(mensaje->mensaje));

	buffer->stream = stream;
	return buffer;
}
t_confirmacion* deserializar_confirmacion(t_buffer* buffer) {
	t_confirmacion* mensaje = malloc(sizeof(t_confirmacion));
	void* stream = buffer->stream;

	//Deserializacion
	memcpy(&(mensaje->tipo_mensaje), stream, sizeof(mensaje->tipo_mensaje));
	stream += sizeof(int);

	memcpy(&(mensaje->mensaje), stream, sizeof(mensaje->mensaje));

	return mensaje;
}

//Muestro de mensajes

char* mostrar_new_pokemon(t_new_pokemon* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size,
			"Pokemon: %s, Posicion X: %d, Posicion Y: %d, Cantidad: %d, Id mensaje: %d",
			mensaje->pokemon, mensaje->posx, mensaje->posy, mensaje->cantidad,
			mensaje->id_mensaje);
	return parametros;
}

char* mostrar_appeared_pokemon(t_appeared_pokemon* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size,
			"Pokemon: %s, Posicion X: %d, Posicion Y: %d, Id mensaje: %d",
			mensaje->pokemon, mensaje->posx, mensaje->posy,
			mensaje->id_mensaje);
	return parametros;
}
char* mostrar_catch_pokemon(t_catch_pokemon* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size,
			"Pokemon: %s, Posicion X: %d, Posicion Y: %d, Id mensaje: %d",
			mensaje->pokemon, mensaje->posx, mensaje->posy,
			mensaje->id_mensaje);
	return parametros;
}
char* mostrar_caught_pokemon(t_caught_pokemon* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size, "ok/fail: %d ID mensaje: %d",
			mensaje->ok_or_fail, mensaje->id_mensaje);
	return parametros;
}
char* mostrar_get_pokemon(t_get_pokemon* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size, "Pokemon: %s, Id mensaje: %d",
			mensaje->pokemon, mensaje->id_mensaje);
	return parametros;
}
char* mostrar_suscriptor(t_subscriptor* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size, "Cola de mensaje: %d, Tiempo: %d",
			mensaje->cola_de_mensaje, mensaje->tiempo);
	return parametros;
}
char* mostrar_confirmacion(t_confirmacion* mensaje) {
	int max_size = 100;
	char* parametros = malloc(sizeof(char) * max_size);
	snprintf(parametros, max_size, "Tipo de mensaje: %d, mensaje: %d",
			mensaje->tipo_mensaje, mensaje->mensaje);
	return parametros;
}
char* mostrar_localized(t_localized* mensaje) {
	int max_size = 200;
	char* parametros = malloc(sizeof(char) * max_size);

	void mostrar_posiciones_aux(void* posicion) {
		char* aux = malloc(sizeof(char) * 10);
		//puts("1");
		aux = mostrar_posiciones(posicion);
		//puts("2");
		strcat(parametros, aux);
		//puts("3");
	}

	int a = list_size(mensaje->posiciones);
	snprintf(parametros, max_size,
			"Id_mensaje: %d, Pokemon: %s, Cantidad: %d, Posiciones:",
			mensaje->id_mensaje, mensaje->pokemon,
			mensaje->cantidad_posiciones);
	puts(parametros);

	list_iterate(mensaje->posiciones, mostrar_posiciones_aux);

	return parametros;
}

char* mostrar_posiciones(t_posicion* posicion) {
	char* pos = malloc(sizeof(char) * 10);
	snprintf(pos, 10, "%d %d ", posicion->x, posicion->y);
	//puts(pos);
	return pos;
}

//Confirmar Recepcion

void confirmar_recepcion(int socket_broker, int cod_op, int id_mensaje) {
	t_confirmacion* mensaje_confirmacion = crear_confirmacion(cod_op,
			id_mensaje);
	t_buffer* buffer_confirmacion = serializar_confirmacion(
			mensaje_confirmacion);

	enviar_mensaje(socket_broker, buffer_confirmacion, CONFIRMACION);
}

t_modulo string_a_modulo(char* nombre_modulo) {
	if (string_equals_ignore_case(nombre_modulo, "TEAM")) {
		return team;
	} else if (string_equals_ignore_case(nombre_modulo, "GAMECARD")) {
		return gamecard;
	} else if (string_equals_ignore_case(nombre_modulo, "BROKER")) {
		return broker;
	} else if (string_equals_ignore_case(nombre_modulo, "SUSCRIPTOR")) {
		return SUSCRIPTOR;
	} else {
		return -1;
	}
}

op_code string_a_tipo_mensaje(char* nombre_mensaje) {
	if (string_equals_ignore_case(nombre_mensaje, "NEW_POKEMON")) {
		return NEW_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "APPEARED_POKEMON")) {
		return APPEARED_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "CATCH_POKEMON")) {
		return CATCH_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "CAUGHT_POKEMON")) {
		return CAUGHT_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "GET_POKEMON")) {
		return GET_POKEMON;
	} else if (string_equals_ignore_case(nombre_mensaje, "SUSCRIPTOR")) {
		return SUSCRIPTOR;
	} else if (string_equals_ignore_case(nombre_mensaje, "LOCALIZED_POKEMON")) {
		return LOCALIZED_POKEMON;
	} else
		return -1;
}

char* op_code_a_string(int op) {
	char* tipo_mensaje = malloc(sizeof(char) * 20);
	switch (op) {
	case APPEARED_POKEMON:
		strcpy(tipo_mensaje, "APPEARED_POKEMON");
		break;
	case NEW_POKEMON:
		strcpy(tipo_mensaje, "NEW_POKEMON");
		break;
	case CAUGHT_POKEMON:
		strcpy(tipo_mensaje, "CAUGHT_POKEMON");
		break;
	case CATCH_POKEMON:
		strcpy(tipo_mensaje, "CATCH_POKEMON");
		break;
	case GET_POKEMON:
		strcpy(tipo_mensaje, "GET_POKEMON");
		break;
	case LOCALIZED_POKEMON:
		strcpy(tipo_mensaje, "LOCALIZED_POKEMON");
		break;
	case SUSCRIPTOR:
		strcpy(tipo_mensaje, "SUSCRIPTOR");
		break;
	case CONFIRMACION:
		strcpy(tipo_mensaje, "CONFIRMACION");
		break;
	}
	return tipo_mensaje;

}
