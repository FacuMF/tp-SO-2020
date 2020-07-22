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
}

void manejar_catch_pokemon(t_catch_pokemon * mensaje_catch){
	bool respuesta_caugth = true;

	// 1. Verifico si existe
	respuesta_caugth = informar_error_no_existe_pokemon_catch(mensaje_catch);

	// 2. Verifico si se puede abrir.
	if (respuesta_caugth){
		intentar_abrir_archivo(mensaje_catch->pokemon);
	}

	// 3. Verifica que exista la posicion.
	if (respuesta_caugth){
		respuesta_caugth = informar_error_no_existe_pos_catch(mensaje_catch);
	}

	// 4. Si la unidad es 1, elimino la linea. Si es mayor, resto una unidad.
	if (respuesta_caugth){
		restar_uno_pos_catch(mensaje_catch); //TODO
	} else {
		cerrar_archivo_pokemon(mensaje_catch->pokemon);
	}

	// 5. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 6. Cerrar archivo
	if (respuesta_caugth){
		cerrar_archivo_pokemon(mensaje_catch->pokemon);
	}

	// 7. Conectarse a broker y enviar resultado
	enviar_caught_pokemon_a_broker(crear_caught_pokemon(mensaje_catch->id_mensaje,respuesta_caugth)); // cambiar la funcion para no tener (void*)

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
		respuesta_localized = obtener_pos_y_cant_localized(mensaje_get);
	}

	// 4. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 5. Cerrar archivo
	cerrar_archivo_pokemon(mensaje_get->pokemon);

	// 6. Conectarse al broker y enviar resultado
	enviar_localized_pokemon_a_broker(respuesta_localized);
}

//FUNCION AUXILIAR

void manejar_bloques_pokemon(t_new_pokemon * mensaje_new){
	t_config* config_pokemon = read_pokemon_metadata(mensaje_new->pokemon);
	config_set_value(config_pokemon,"OPEN","Y");
	char ** bloques = extraer_bloques(mensaje_new->pokemon);
	char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);
	if(verificar_posiciones_file(posicion,bloques)){
		sumar_unidad_posicion(mensaje_new,bloques);
	}else{
		agregar_posicion(mensaje_new,bloques);
	}
	config_save(config_pokemon);
	config_destroy(config_pokemon);
}

bool informar_error_no_existe_pos_catch(t_catch_pokemon* mensaje_catch){
	char ** bloques = extraer_bloques(mensaje_catch->pokemon);
	char* posicion = concatenar_posicion(mensaje_catch->posx,mensaje_catch->posy);
	if(verificar_posiciones_file(posicion,bloques)){
		return true;
	}
	else{
		log_error(logger, "Posicion del pokemon %s no existente.", mensaje_catch->pokemon);
		return false;
	}

}

void agregar_posicion(t_new_pokemon * mensaje_new, char** bloques){ // Podriamos sacar los bloques como parametro
	int tamanio_sentencia = string_length(string_itoa(mensaje_new->posx)) + string_length(string_itoa(mensaje_new->posy)) + string_length(string_itoa(mensaje_new->cantidad)) + 2; // CHEQUEAR
	int i = 0;
	while(bloques[i]!=NULL){
		int tamanio_total = tamanio_archivo(block_path(atoi(bloques[i]))) + tamanio_sentencia; // bloques[i] es char, cambiar a int (atoi)
		if(tamanio_total <= tamanio_bloque()){
			t_config* config_metadata = read_pokemon_metadata(mensaje_new->pokemon);
			t_config* config_bloque = config_create(block_path(atoi(bloques[i])));
			char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);
			config_set_value(config_bloque, posicion, string_itoa(mensaje_new->cantidad));
			actualizar_size_metadata(config_metadata, bloques);
			return;
		}else{
			i++;
		}
	}
	asignar_bloque(mensaje_new,0);
}

int tamanio_todos_los_bloques(char** bloques){
	int tamanio=0;
	int i=0;
	while(bloques[i]!=NULL){
		tamanio += tamanio_archivo(block_path(atoi(bloques[i])));
	}
	return tamanio;
}



// Auxiliares.

bool abrir_archivo(char* pokemon){
	//Si abre devuelve true (-> salir del while), si no puede false(-> sigue en el while).

	pthread_mutex_lock(&mutex_open_file);

	t_config* config = read_pokemon_metadata(pokemon);
	char* estado = config_get_string_value(config,"OPEN");

	pthread_mutex_unlock(&mutex_open_file);

	return !strcasecmp(estado,"Y"); // VER SI SE NECESITA ! (DA 0 SI SON IGUALES)

}

void crear_file_si_no_existe(t_new_pokemon * mensaje_new){
	char* file = pokemon_metadata_path(mensaje_new->pokemon);
	if(!file_existing(file)){
		log_trace(logger,"File del pokemon no existente, lo creo.");
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
	while ( abrir_archivo(pokemon) ){
		log_trace(logger,"Archivo se encuentra abierto, se reintenta operacion"); // Podria cambiarse a log_info
		sleep( config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION") );
		log_trace(logger,"Reintentando operacion");
	}
}

void restar_uno_pos_catch(t_catch_pokemon* mensaje_catch){
	//Si la unidad es 1, elimino la linea. Si es mayor, resto una unidad.
	char** bloques = extraer_bloques(mensaje_catch->pokemon);
	char* posicion = concatenar_posicion(mensaje_catch->posx,mensaje_catch->posy);
	int bloque_con_posicion = encontrar_bloque_con_posicion(posicion, bloques);
	t_config* config_bloque = config_create(block_path(bloque_con_posicion));
	int cantidad = config_get_int_value(config_bloque,posicion);
	if(cantidad == 1){
		config_remove_key(config_bloque,posicion);
	} else {
		int cantidad_previa = config_get_int_value(config_bloque,posicion);
		config_set_value(config_bloque,posicion,string_itoa(cantidad_previa-1));
	}
	actualizar_size_metadata(read_pokemon_metadata(mensaje_catch->pokemon),bloques);
	config_save(config_bloque);
	config_destroy(config_bloque);
}

		
void cerrar_archivo_pokemon(char* pokemon){
	pthread_mutex_lock(&mutex_open_file);
	t_config* config_pokemon = read_pokemon_metadata(pokemon);
	config_set_value(config_pokemon,"OPEN","N");

	config_save(config_pokemon);
	config_destroy(config_pokemon);

	pthread_mutex_unlock(&mutex_open_file);
}

void sumar_unidad_posicion(t_new_pokemon* mensaje_pokemon,char** bloques){ // Capaz se puede optimizar (usando for se puede salir)
	int n=0;
	char* posicion = concatenar_posicion(mensaje_pokemon->posx, mensaje_pokemon->posy);
	while(bloques[n]!=NULL){
			t_config* config_bloque = config_create(block_path(atoi(bloques[n]))); // atoi?
			if (config_has_property(config_bloque,posicion)){
				int cantidad_vieja = config_get_int_value(config_bloque,posicion);
				int diferencia_bytes = cantidad_bytes_de_mas(string_itoa(cantidad_vieja), string_itoa(cantidad_vieja + mensaje_pokemon->cantidad));
				if(tamanio_archivo(block_path(atoi(bloques[n]))) == tamanio_bloque() && diferencia_bytes > 0){
					asignar_bloque(mensaje_pokemon,1);
				}
				config_set_value(config,posicion, string_itoa(cantidad_vieja + mensaje_pokemon->cantidad));
			}
			config_save(config_bloque);
			config_destroy(config_bloque);
			n++;
		}

}

int cantidad_bytes_de_mas(char* sentencia1, char* sentencia2){
	return size_bytes(sentencia2) - size_bytes(sentencia1);
}
