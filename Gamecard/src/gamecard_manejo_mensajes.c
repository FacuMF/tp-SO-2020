#include "gamecard.h"

// TODO FUNCIONES DE MANEJAR MENSAJE

void manejar_new_pokemon(t_new_pokemon *mensaje_new){
	// TODO : AGREGAR  SEAFORO PARA SICRO DE MENSAJE
	// TODO: CHEQUEAR COSAS QUE HAY QUE HACER, fijarse mutex

	// 1. Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(mensaje_new);

	// 2. Verifico si puedo abrir, y reintento si no.
	intentar_abrir_archivo(mensaje_new->pokemon);

	// 3. Verifica si las pos existen, si no la crea al final. //TODO
	manejar_bloques_pokemon(mensaje_new); // revisar, cambiar estado open.

	// 4. Esperar para cerrar
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 5. Cerrar archivo
	cerrar_archivo_pokemon(mensaje_new->pokemon);

	// 6. Enviar mensaje al broker
	t_appeared_pokemon * mensaje_appeared= de_new_a_appeared(mensaje_new);
	enviar_appeared_pokemon_a_broker(mensaje_appeared);

	//TBR
	// Reintentar la operación luego de REINTENTO_OPERACION
	// Verificar si las posiciones ya existen dentro del archivo
	// Esperar la cantidad de segundos definidos en config
	// Cerrar el archivo
	// Conectarse al broker y enviar a APPEARED_POKEMON un mensaje con ID del mensaje recibido, pokemon, posicion en el mapa
	//enviar_appeared_pokemon_a_broker(BROKER,pokemon);
	// En caso de no poder conectarse avisar por log
}

void manejar_catch_pokemon(t_catch_pokemon * mensaje_catch){
	bool respuesta_caugth = true;

	// 1. Verifico si existe
	respuesta_caugth = informar_error_no_existe_pokemon_catch(mensaje_catch);

	// 2. Verifico si se puede abrir.
	intentar_abrir_archivo(mensaje_catch->pokemon);

	// 3. Verifica que exista la posicion.
	if (respuesta_caugth){
		respuesta_caugth = informar_error_no_existe_pos_catch(mensaje_catch);
	}

	// 4. Si la unidad es 1, elimino la linea. Si es mayor, resto una unidad.
	restar_uno_pos_catch(); //TODO

	// 5. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 6. Cerrar archivo
	cerrar_archivo_pokemon(mensaje_catch->pokemon);

	// 7. Conectarse a broker y enviar resultado
	enviar_caught_pokemon_a_broker( (void*) crear_caught_pokemon(respuesta_caugth) );


	//TBR
	//char* file = pokemon_metadata_path(mensaje_catch->pokemon);
	// Verificar si existe pokemon en el Filesystem
	//crear_file_si_no_existe(file,mensaje_catch->pokemon);
	// Verificar si se puede abrir el archivo
	// Verificar si las posiciones existen dentro del archivo
	// En caso de que la cantidad sea 1 -> eliminar la linea, en caso contrario se debe decrementar una unidad
	// Esperar la cantidad de segundos definidos en config
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje indicando ID del mensaje  y resultado a CAUGHT_POKEMON.
	// Si no se puede conectar al broker informar por log y continuar
}

void manejar_get_pokemon(t_get_pokemon * mensaje_get){
	bool respuesta_get = true;
	t_localized_pokemon* respuesta_localized;

	// 1. Verificar si el pokemon existe.
	respuesta_get = informar_no_existe_pokemon_get(mensaje_get);

	// 2. Verificar si se puede abrir
	intentar_abrir_archivo(mensaje_get->pokemon);

	// 3. Obtener todas las pos y cantidades
	if(respuesta_get){
		respuesta_localized = obtener_pos_y_cant_localized(mensaje_get); //TODO
	}

	// 4. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 5. Cerrar archivo
	cerrar_archivo_pokemon(mensaje_get->pokemon);

	// 6. Conectarse al broker y enviar resultado
	enviar_localized_pokemon_a_broker(respuesta_localized);

	//TBR
	//char* file = pokemon_metadata_path(mensaje_get->pokemon);
	// Verificar si existe pokemon en el Filesystem
	//crear_file_si_no_existe(file,mensaje_get->pokemon);
	// Verificar si se puede abrir el archivo
	// Obtener todas las posiciones y cantidades del pokemon requerido
	// Esperar la cantidad de segundos definidos en config
	//sleep(RETARDO_OPERACION);
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje con todas las posiciones y su cantidad
	// En caso de que se encuentre por lo menos una posicion para el pokemon solicitado se debera enviar un mensaje al broker a LOCALIZED_POKEMON indicando
	// 	ID del mensaje, el pokemon solicitado y la lista de posiciones y cantidad de posiciones X e Y de cada una de ellas
	// Si no se puede conectar al broker informar por log y continuar
}

//FUNCION AUXILIAR
t_appeared_pokemon* convertir_a_appeared_pokemon(t_new_pokemon* pokemon){
	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon -> id_mensaje = pokemon->id_mensaje;
	appeared_pokemon -> pokemon = pokemon->pokemon;
	appeared_pokemon -> posx = pokemon->posx;
	appeared_pokemon -> posy = pokemon->posy;
	appeared_pokemon -> size_pokemon = pokemon -> size_pokemon;
	return appeared_pokemon;
}


void chequear_archivo_abierto(t_new_pokemon *mensaje_new){


}

void manejar_bloques_pokemon(t_new_pokemon * mensaje_new){
	char ** bloques = extraer_bloques(mensaje_new->pokemon);

	if(verificar_posiciones_file(mensaje_new->posx,mensaje_new->posy,bloques)){ // TODO
		//sumar_unidad_posicion(posicion); // TODO
	}else{
		agregar_posicion(mensaje_new,bloques);
	}
}

bool informar_error_no_existe_pos_catch(t_catch_pokemon* mensaje_catch){
	char ** bloques = extraer_bloques(mensaje_catch->pokemon);

	if(verificar_posiciones_file(mensaje_catch->posx,mensaje_catch->posy,bloques)){ // TODO
		return true;
	}
	else{
		log_error(logger, "Posicion del pokemon %s no existente.", mensaje_catch->pokemon);
		return false;
	}

}

void agregar_posicion(t_new_pokemon * mensaje_new, char** bloques){
	int tamanio_pokemon = string_length(string_itoa(mensaje_new->posx)) + string_length(string_itoa(mensaje_new->posy)) + string_length(string_itoa(mensaje_new->cantidad)) + 2; // CHEQUEAR
	int i =0;
	while(bloques[i]!=NULL){
		int tamanio = tamanio_archivo(block_path(atoi(bloques[i]))) + tamanio_pokemon; // bloques[i] es char, cambiar a int
		if(tamanio <= tamanio_bloque()){
			t_config* config_metadata = read_pokemon_metadata(mensaje_new->pokemon);
			t_config* config_bloque = block_path(bloques[i]);
			char* posicion = concatenar_posicion(mensaje_new);
			config_save_in_file(config_bloque, posicion, mensaje_new->cantidad);
			int tamanio_definitivo =tamanio_todos_los_bloques(bloques);
			config_set_value(config_metadata, "SIZE", tamanio_definitivo); // TODO: SUMA DE TODOS LOS BLOQUES
			return;
		}else{
			i++;
		}
	}
	asignar_bloque(mensaje_new, bloques);
}

int tamanio_todos_los_bloques(char** bloques){
	int tamanio=0;
	int i=0;
	while(bloques[i]!=NULL){
		tamanio += tamanio_archivo(block_path(atoi(bloques[i])));
	}
	return tamanio;
}

char* concatenar_posicion(t_new_pokemon* mensaje_new){
	char* posicion_parcial = concat(string_itoa(mensaje_new->posx),"-");
	char* posicion_definitiva = concat(posicion_parcial,string_itoa(mensaje_new->posy));
	return posicion_definitiva;
}

// Auxiliares.

bool abrir_archivo(char* pokemon){ // TODO : CAMBIAR A F_READ
	//Si abre devuelve true (-> salir del while), si no puede false(-> sigue en el while).

	//todo mutex lock

	t_config* config = read_pokemon_metadata(pokemon);
	char* estado = config_get_string_value(config,"OPEN");

	// mutex unlock

	return !strcasecmp(estado,"Y");

}

void crear_file_si_no_existe(t_new_pokemon * mensaje_new){
	char* file = pokemon_metadata_path(mensaje_new->pokemon);
	if(!file_existing(file)){
		log_trace(logger,"File del pokemon no existente, Lo creo");
		crear_pokemon_dir(mensaje_new->pokemon);
		crear_pokemon_metadata_file(mensaje_new->pokemon);
	}
}

bool informar_error_no_existe_pokemon_catch(t_catch_pokemon* mensaje_catch){
	char* file = pokemon_metadata_path(mensaje_catch->pokemon);
	if(file_existing(file)){
		return true;
	}
	else{
		log_error(logger,"File del pokemon %s no existente.", mensaje_catch->pokemon);
		return false;
	}

}

bool informar_no_existe_pokemon_get(t_get_pokemon* mensaje_get){
	char* file = pokemon_metadata_path(mensaje_get->pokemon);
	return file_existing(file);
}

/*char* pokemon, int posicion_x,
		int posicion_y, int id_mensaje)
	*/
t_appeared_pokemon * de_new_a_appeared(t_new_pokemon * mensaje_new){
	t_appeared_pokemon * mensaje_appeared = crear_appeared_pokemon(mensaje_new->pokemon,
		mensaje_new->posx,mensaje_new->posy, -30);
	return mensaje_appeared;
}

void intentar_abrir_archivo(char* pokemon){
	while ( ! abrir_archivo(pokemon) ){
		log_trace(logger,"Archivo se encuentra abierto, se reintenta operacion");
		sleep(config_get_int_value(config,"TIEMPO_REINTENTO_OPERACION"));
		log_trace(logger,"Reintentando operacion");
	}
}

void restar_uno_pos_catch(){
	//Si la unidad es 1, elimino la linea. Si es mayor, resto una unidad.
	
	
	
}

t_localized_pokemon* obtener_pos_y_cant_localized(t_get_pokemon* mensaje_get){ //TODO

	t_localized_pokemon* respuesta_localized = malloc(sizeof(t_localized_pokemon*));



	return respuesta_localized;
}
		
void cerrar_archivo_pokemon(char* pokemon){
	//TODO mutex lock (el mismo sem que en open)


	// mutex unlock
}

