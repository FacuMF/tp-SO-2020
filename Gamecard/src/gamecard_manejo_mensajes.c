#include "gamecard.h"

// TODO FUNCIONES DE MANEJAR MENSAJE

void manejar_new_pokemon(t_new_pokemon *mensaje_new){
	log_trace( logger, "Manejar mensaje: %s.", mostrar_new_pokemon(mensaje_new) );

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
	t_appeared_pokemon * mensaje_appeared = de_new_a_appeared(mensaje_new);
	enviar_appeared_pokemon_a_broker(mensaje_appeared);
}

void manejar_catch_pokemon(t_catch_pokemon * mensaje_catch){
	bool respuesta_existe_archivo = true;
	bool respuesta_existe_pos_archivo = false;

	// 1. Verifico si existe
	respuesta_existe_archivo = informar_error_no_existe_pokemon_catch(mensaje_catch); // Si existe -> true;

	// 2. Verifico si se puede abrir.
	if (respuesta_existe_archivo){ //Si no existe archivo no entra
		intentar_abrir_archivo(mensaje_catch->pokemon);
	}

	// 3. Verifica que exista la posicion.
	if (respuesta_existe_archivo){ //Si no existe archivo no entra
		respuesta_existe_pos_archivo = informar_error_no_existe_pos_catch(mensaje_catch); // Si existe -> true;
	}

	// 4. Si la unidad es 1, elimino la linea. Si es mayor, resto una unidad.
	if (respuesta_existe_pos_archivo){ //Si no posicion en archivo no entra => bool respuesta_pos queda en false;
		restar_uno_pos_catch(mensaje_catch); //TODO
	}

	// 5. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 6. Cerrar archivo
	if(respuesta_existe_archivo){ //Si no existe archivo no entra
		cerrar_archivo_pokemon(mensaje_catch->pokemon);
	}

	// 7. Conectarse a broker y enviar resultado
	enviar_caught_pokemon_a_broker(crear_caught_pokemon(mensaje_catch->id_mensaje,respuesta_existe_pos_archivo));
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
	log_trace(logger, "Manejar bloques pokemon.");

	log_debug(logger,"Abriendo el archivo, dejando OPEN = Y");
	char ** bloques = extraer_bloques(mensaje_new->pokemon);
	char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);

	if(verificar_posiciones_file(posicion,bloques)){
		log_debug(logger, "Sumar unidad posicion.");
		sumar_unidad_posicion(mensaje_new,bloques);
	}else{
		log_debug(logger, "Agregar posicion.");
		agregar_posicion(mensaje_new,bloques);
	}
	log_debug(logger,"Posicion agregada/sumada");

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
		log_debug(logger,"Entro al while");
		int tamanio_total = tamanio_archivo(block_path(atoi(bloques[i]))) + tamanio_sentencia;
		log_debug(logger,"Tamanio total del archivo al escribir la sentencia: %d",tamanio_total);
		log_debug(logger,"Tamanio de bloques: %d",tamanio_bloque());

		if(tamanio_total <= tamanio_bloque()){

			t_config* config_bloque = config_create(block_path(atoi(bloques[i])));
			char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);
			config_set_value(config_bloque, posicion, string_itoa(mensaje_new->cantidad));
			config_save(config_bloque);
			config_destroy(config_bloque);

			actualizar_size_metadata(mensaje_new->pokemon);

			return;

		}else{

			i++;

		}
	}
	log_debug(logger,"Paso el while sin entrar");
	asignar_bloque(mensaje_new,0);
}

int tamanio_todos_los_bloques(char** bloques){
	int tamanio=0;
	int i=0;
	while(bloques[i]!=NULL){
		tamanio += tamanio_archivo(block_path(atoi(bloques[i])));
		i++;
	}
	log_debug(logger,"Tamanio de todos los bloques del pokemon: %d",tamanio);
	return tamanio;
}



// Auxiliares.

bool abrir_archivo(char* pokemon){
	//Si abre devuelve true (-> salir del while), si no puede false(-> sigue en el while).

	pthread_mutex_lock(&mutex_open_file);

	t_config* config = read_pokemon_metadata(pokemon);
	char* estado = config_get_string_value(config,"OPEN");

	bool estaba_abierto = strcmp(estado,"Y") == 0;

	log_trace(logger, "Archivo: OPEN=%s, estaba_abierto=%i.", estado, estaba_abierto);

	if( strcmp(estado,"N") == 0 ){ // Abro el archivo. Si estaba cerrado.

		config_set_value(config, "OPEN", "Y");
		config_save(config);

	}

	config_destroy(config);

	pthread_mutex_unlock(&mutex_open_file);

	return estaba_abierto; // SI ESTABA CERRADO => AHORA LO ABRI Y DEVUELVO FALSE => Salgo del while

}

void crear_file_si_no_existe(t_new_pokemon * mensaje_new){
	char* file = pokemon_metadata_path(mensaje_new->pokemon);
	if(!file_existing(file)){
		log_trace(logger,"File del pokemon no existente, lo creo.");
		create_new_file_pokemon(mensaje_new->pokemon);

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


t_appeared_pokemon * de_new_a_appeared(t_new_pokemon * mensaje_new){

	t_appeared_pokemon * mensaje_appeared = crear_appeared_pokemon(mensaje_new->pokemon,
		mensaje_new->posx,mensaje_new->posy, mensaje_new->id_mensaje);

	return mensaje_appeared;
}

void intentar_abrir_archivo(char* pokemon){

	bool archivo_abierto_por_otro = abrir_archivo(pokemon);

	while ( archivo_abierto_por_otro ){
		log_warning(logger,"Archivo se encuentra abierto, se reintenta operacion"); // Podria cambiarse a log_info
		sleep( config_get_int_value(config,"TIEMPO_DE_REINTENTO_OPERACION") );

		archivo_abierto_por_otro = abrir_archivo(pokemon);
		log_trace(logger,"Reintentando operacion.");
	}

	log_trace(logger, "Se abrio el archivo.");
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
	}
	else {
		int cantidad_previa = config_get_int_value(config_bloque,posicion);
		config_set_value(config_bloque,posicion,string_itoa(cantidad_previa-1));
	}

	config_save(config_bloque);
	config_destroy(config_bloque);

	actualizar_size_metadata(mensaje_catch->pokemon);

	int tamanio_bloque = tamanio_archivo(block_path(bloque_con_posicion));

	if( tamanio_bloque == 0 ){
		sacar_bloque_de_metadata(mensaje_catch->pokemon,bloque_con_posicion); //TODO: Modificar bitmap.
	}
}

void sacar_bloque_de_metadata(char* pokemon,int bloque_con_posicion){
	t_config* config_metadata = read_pokemon_metadata(pokemon);
	char* bloques = extraer_bloques_string(pokemon);
	if(string_starts_with(bloques,concat("[",string_itoa(bloque_con_posicion)))){
		char**bloques_separados = string_split(bloques,concat(string_itoa(bloque_con_posicion),","));
		char* bloques_nuevos = string_new();
		string_append(&bloques_nuevos,bloques_separados[0]);
		string_append(&bloques_nuevos,bloques_separados[1]);
		config_set_value(config_metadata,"BLOCKS",bloques_nuevos);
		config_save(config_metadata);
		config_destroy(config_metadata);
	}
	else if(string_ends_with(bloques,concat(string_itoa(bloque_con_posicion),"]"))){
		char**bloques_separados = string_split(bloques,concat(",",string_itoa(bloque_con_posicion)));
		char* bloques_nuevos = string_new();
		string_append(&bloques_nuevos,bloques_separados[0]);
		string_append(&bloques_nuevos,bloques_separados[1]);
		config_set_value(config_metadata,"BLOCKS",bloques_nuevos);
		config_save(config_metadata);
		config_destroy(config_metadata);
	} else {
		char* aux = concat(string_itoa(bloque_con_posicion),",");
		char**bloques_separados = string_split(bloques,concat(",",aux));
		char* bloques_nuevos = string_new();
		string_append(&bloques_nuevos,bloques_separados[0]);
		string_append(&bloques_nuevos,",");
		string_append(&bloques_nuevos,bloques_separados[1]);
		config_set_value(config_metadata,"BLOCKS",bloques_nuevos);
		config_save(config_metadata);
		config_destroy(config_metadata);
	}
	vaciar_bloque_bitmap(bloque_con_posicion);
}

char* extraer_bloques_string(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	return config_get_string_value(config,"BLOCKS");
}


void vaciar_bloque_bitmap(int bloque){
	pthread_mutex_lock(&mutex_bitmap);

	bitarray_clean_bit(bitmap_bloques,bloque);
	save_bitmap();

	pthread_mutex_unlock(&mutex_bitmap);

	//TODO unlock mutex
}

void save_bitmap() {
	FILE* file_birarray = fopen( bitmap_path(), "w");

	for (int renglon = 0; renglon < (cantidad_bloques()/8) ; ++renglon) {

		for (int bit = 0; bit < 8; ++bit) {
			fprintf(file_birarray, "%i", bitarray_test_bit(bitmap_bloques, renglon*bit) );
			//log_debug(logger, "SAVE BITARRAY bloque %i: %i", i, bitarray_test_bit(bitmap_bloques, i));
		}
		fprintf(file_birarray, "\n" );
	}

	fclose(file_birarray);
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
	log_debug(logger,"Entro a summar unidad pos");
	while(bloques[n]!=NULL){
		log_debug(logger,"Entro en el while de sumar unidad posicion");
		t_config* config_bloque = config_create(block_path(atoi(bloques[n])));
		if (config_has_property(config_bloque,posicion)){
			log_debug(logger,"Encontre bloque que tiene mi misma pos");
			int cantidad_vieja = config_get_int_value(config_bloque,posicion);
			int diferencia_bytes = cantidad_bytes_de_mas(string_itoa(cantidad_vieja), string_itoa(cantidad_vieja + mensaje_pokemon->cantidad));
			int tamanio_total = tamanio_archivo(block_path(atoi(bloques[n]))) + diferencia_bytes;
			if(tamanio_total > tamanio_bloque()){
				asignar_bloque(mensaje_pokemon,1);

			} else {

				config_set_value(config_bloque,posicion, string_itoa(cantidad_vieja + mensaje_pokemon->cantidad));
				config_save(config_bloque);
			}
			config_destroy(config_bloque);
			actualizar_size_metadata(mensaje_pokemon->pokemon);
			return;
		}
		config_save(config_bloque);
		config_destroy(config_bloque);
		n++;
	}

}


int cantidad_bytes_de_mas(char* sentencia1, char* sentencia2){
	return size_bytes(sentencia2) - size_bytes(sentencia1);
}
