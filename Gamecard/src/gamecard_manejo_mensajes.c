#include "gamecard.h"

// TODO FUNCIONES DE MANEJAR MENSAJE

void manejar_new_pokemon(t_new_pokemon *mensaje_new){
	// TODO: CHEQUEAR COSAS QUE HAY QUE HACER, fijarse mutex


	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(mensaje_new);

	chequear_archivo_abierto(mensaje_new);

	manejar_bloques_pokemon(mensaje_new);

	// mutex unlock
		// Reintentar la operación luego de REINTENTO_OPERACION
	// Verificar si las posiciones ya existen dentro del archivo
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar el archivo
	// Conectarse al broker y enviar a APPEARED_POKEMON un mensaje con ID del mensaje recibido, pokemon, posicion en el mapa
	//enviar_appeared_pokemon_a_broker(BROKER,pokemon);
	// En caso de no poder conectarse avisar por log
}

void manejar_catch_pokemon(t_catch_pokemon * mensaje_catch){
	// TODO: VER COMO MANEJAR EL CATCH_POKEMON
	char* file = pokemon_metadata_path(mensaje_catch->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,mensaje_catch->pokemon);
	// Verificar si se puede abrir el archivo
	// Verificar si las posiciones existen dentro del archivo
	// En caso de que la cantidad sea 1 -> eliminar la linea, en caso contrario se debe decrementar una unidad
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje indicando ID del mensaje  y resultado a CAUGHT_POKEMON.
	// Si no se puede conectar al broker informar por log y continuar
}

void manejar_get_pokemon(t_get_pokemon * mensaje_get){

	char* file = pokemon_metadata_path(mensaje_get->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,mensaje_get->pokemon);
	// Verificar si se puede abrir el archivo
	// Obtener todas las posiciones y cantidades del pokemon requerido
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
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

	if (file_open(mensaje_new->pokemon)){
			int reintento =config_get_int_value(config,"TIEMPO_REINTENTO_OPERACION");
			log_trace(logger,"Archivo se encuentra abierto");
			sleep(reintento);
			log_trace(logger,"Reintentando operacion");
			manejar_new_pokemon(mensaje_new); // TODO: Ver si esta bien implementado
	}
}

void manejar_bloques_pokemon(t_new_pokemon * mensaje_new){
	char ** bloques = extraer_bloques(mensaje_new->pokemon);
	if(verificar_posiciones_file(mensaje_new->posx,mensaje_new->posy,bloques)){
		//sumar_unidad_posicion(posicion);
	}else{
		agregar_posicion(mensaje_new,bloques);
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
		tamanio += tamanio_archivo(block_path(atoi(bloques[i])))
	}
	return tamanio;
}

char* concatenar_posicion(t_new_pokemon* mensaje_new){
	char* posicion_parcial = concat(string_itoa(mensaje_new->posx),"-");
	char* posicion_definitiva = concat(posicion_parcial,string_itoa(mensaje_new->posy));
	return posicion_definitiva;
}

// Auxiliares.
bool file_open(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	char* estado = config_get_string_value(config,"OPEN");
	return !strcasecmp(estado,"Y");
}



void crear_file_si_no_existe(t_new_pokemon * mensaje_new){
	char* file = pokemon_metadata_path(mensaje_new->pokemon);
	if(!file_existing(file)){
		create_pokemon_dir(mensaje_new->pokemon);
		create_pokemon_metadata_file(mensaje_new->pokemon);
	}
}






