#include "gamecard.h"

// TODO FUNCIONES DE MANEJAR MENSAJE

/*void manejar_new_pokemon(t_new_pokemon *mensaje_new){
	// TODO: CHEQUEAR COSAS QUE HAY QUE HACER
	char* file = pokemon_metadata_path(mensaje_new->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,mensaje_new->pokemon);
	// Verificar si se puede abrir el archivo
	// mutex lock
	if (!file_open(mensaje_new->pokemon)){ // SEMAFORO MUTEX PARA MANEJAR LOS OPEN
		//Buscar los bloques del pokemon
		char** bloques = extraer_bloques(mensaje_new->pokemon);
		if(verificar_posiciones_file(mensaje_new->posx,mensaje_new->posy,bloques)){
		}
	}
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
t_appeared_pokemon* convertir_a_appeared_pokemon(t_new_pokemon* pokemon){ // ver utilidad de esta funcion
	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon -> id_mensaje = pokemon->id_mensaje;
	appeared_pokemon -> pokemon = pokemon->pokemon;
	appeared_pokemon -> posx = pokemon->posx;
	appeared_pokemon -> posy = pokemon->posy;
	appeared_pokemon -> size_pokemon = pokemon -> size_pokemon;
	return appeared_pokemon;
}
*/
// FUNCIONES AUXILIARES DE MANEJO DE MENSAJES
bool file_existing(char* path){
	FILE * file = fopen(path, "rb");
	if(file == NULL){
		fclose(file);
		return false;
	} else {
	fclose(file);
	return true;
	}
}

bool file_open(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	char* estado = config_get_string_value(config,"OPEN");
	if (estado == "Y"){
		return true;
	} else {
		return false;
	}
}



void crear_file_si_no_existe(char* file, char* pokemon){
	if(!file_existing(file)){
		create_pokemon_dir(pokemon);
		create_pokemon_metadata_file(pokemon);
	}
}






