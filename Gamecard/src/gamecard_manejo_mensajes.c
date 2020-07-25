#include "gamecard.h"

// TODO FUNCIONES DE MANEJAR MENSAJE

void manejar_new_pokemon(t_new_pokemon *mensaje_new){
	char* mostrar_new = mostrar_new_pokemon(mensaje_new);
	log_trace(logger, "Manejar mensaje: %s.", mostrar_new);
	free(mostrar_new);

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
	liberar_mensaje_appeared_pokemon(mensaje_appeared);
}

void manejar_catch_pokemon(t_catch_pokemon * mensaje_catch){
	char* mostrar_catch = mostrar_catch_pokemon(mensaje_catch);
	log_trace(logger, "Manejar mensaje: %s.", mostrar_catch);
	free(mostrar_catch);

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
		restar_uno_pos_catch(mensaje_catch);
	}

	// 5. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 6. Cerrar archivo
	if(respuesta_existe_archivo){ //Si no existe archivo no entra
		cerrar_archivo_pokemon(mensaje_catch->pokemon);
	}

	// 7. Conectarse a broker y enviar resultado
	t_caught_pokemon* caught_pokemon = crear_caught_pokemon(mensaje_catch->id_mensaje, respuesta_existe_pos_archivo, mensaje_catch->id_mensaje);
	enviar_caught_pokemon_a_broker(caught_pokemon);
	liberar_mensaje_caught_pokemon(caught_pokemon);
}

void manejar_get_pokemon(t_get_pokemon * mensaje_get){
	char* mostrar_get = mostrar_get_pokemon(mensaje_get);
	log_trace(logger, "Manejar mensaje: %s.", mostrar_get);
	free(mostrar_get);

	bool respuesta_get = true;
	t_localized_pokemon* respuesta_localized;

	// 1. Verificar si el pokemon existe.
	respuesta_get = informar_no_existe_pokemon_get(mensaje_get);

	if(respuesta_get){
		// 2. Verificar si se puede abrir
		intentar_abrir_archivo(mensaje_get->pokemon);

		// 3. Obtener todas las pos y cantidades
		respuesta_localized = obtener_pos_y_cant_localized(mensaje_get);
	}else{
		t_list * lista_vacia = list_create();
		respuesta_localized = crear_localized_pokemon(mensaje_get->id_mensaje,mensaje_get->pokemon,lista_vacia, mensaje_get->id_mensaje);
	}

	// 4. Esperar los segundos definidos por config
	sleep(config_get_int_value(config,"TIEMPO_RETARDO_OPERACION"));

	// 5. Cerrar archivo
	if(respuesta_get)
		cerrar_archivo_pokemon(mensaje_get->pokemon);

	// 6. Conectarse al broker y enviar resultado
	enviar_localized_pokemon_a_broker(respuesta_localized);
	liberar_mensaje_localized_pokemon(respuesta_localized);
}

//FUNCION AUXILIAR

void manejar_bloques_pokemon(t_new_pokemon * mensaje_new){
	log_trace(logger, "Manejar bloques pokemon.");

	log_trace(logger,"Abriendo el archivo, dejando OPEN = Y");
	char ** bloques = extraer_bloques(mensaje_new->pokemon);
	char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);

	if(verificar_posiciones_file(posicion,bloques)){
		log_trace(logger, "Sumar unidad posicion.");
		sumar_unidad_posicion(mensaje_new,bloques);
	}else{
		log_trace(logger, "Agregar posicion.");
		agregar_posicion(mensaje_new,bloques);
	}
	log_trace(logger,"Posicion agregada/sumada");

	free(posicion);
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

void agregar_posicion(t_new_pokemon * mensaje_new, char** bloques) {
	char* px = string_itoa(mensaje_new->posx);
	char* py = string_itoa(mensaje_new->posy);
	char* canti = string_itoa(mensaje_new->cantidad);
	// Podriamos sacar los bloques como parametro
	int tamanio_sentencia = string_length(px) + string_length(py) + string_length(canti) + 2; // CHEQUEAR

	free(px);
	free(py);
	free(canti);

	int i = 0;
	while(bloques[i]!=NULL){

		log_trace(logger,"Entro al while");

		char* tamanio_t = block_path(atoi(bloques[i]));
		int tamanio_total = tamanio_archivo(tamanio_t) + tamanio_sentencia;
		free(tamanio_t);

		log_trace(logger,"Tamanio total del archivo al escribir la sentencia: %d",tamanio_total);
		log_trace(logger,"Tamanio de bloques: %d",tamanio_bloque());

		if(tamanio_total <= tamanio_bloque()){

			char* block_pa = block_path(atoi(bloques[i]));
			t_config* config_bloque = config_create(block_pa);
			free(block_pa);

			char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);

			char* cant = string_itoa(mensaje_new->cantidad);
			config_set_value(config_bloque, posicion, cant);

			free(cant);
			free(posicion);

			config_save(config_bloque);
			config_destroy(config_bloque);

			actualizar_size_metadata(mensaje_new->pokemon);

			return;

		}else{

			i++;

		}
	}
	log_trace(logger,"Paso el while sin entrar");
	asignar_bloque(mensaje_new,0);
}

int tamanio_todos_los_bloques(char** bloques){
	int tamanio=0;
	int i=0;
	while(bloques[i]!=NULL){
		char* block_p = block_path(atoi(bloques[i]));
		tamanio += tamanio_archivo(block_p);
		free(block_p);
		i++;
	}
	log_trace(logger,"Tamanio de todos los bloques del pokemon: %d",tamanio);
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

	free(file);
}


bool informar_error_no_existe_pokemon_catch(t_catch_pokemon* mensaje_catch){
	char* poke_meta_path = pokemon_metadata_path(mensaje_catch->pokemon);
	char* file = poke_meta_path;
	if(file_existing(file)){
		free(poke_meta_path);
		return true;
	}
	else{
		log_error(logger,"File del pokemon %s no existente.", mensaje_catch->pokemon);
		free(poke_meta_path);
		return false;
	}

}

bool informar_no_existe_pokemon_get(t_get_pokemon* mensaje_get){
	char* poke_meta_pa = pokemon_metadata_path(mensaje_get->pokemon);
	char* file = poke_meta_pa;
	bool existe = file_existing(file);
	//free(poke_meta_pa);
	//free(file);
	return existe;
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

	char* block_p = block_path(bloque_con_posicion);
	t_config* config_bloque = config_create(block_p);


	int cantidad = config_get_int_value(config_bloque,posicion);

	if(cantidad == 1){
		config_remove_key(config_bloque,posicion);
	}
	else {
		int cantidad_previa = config_get_int_value(config_bloque,posicion);

		char* cant_previa_menos_uno = string_itoa(cantidad_previa - 1);
		config_set_value(config_bloque, posicion, cant_previa_menos_uno);
		free(cant_previa_menos_uno);

	}

	config_save(config_bloque);
	config_destroy(config_bloque);


	actualizar_size_metadata(mensaje_catch->pokemon);

	int tamanio_bloque = tamanio_archivo(block_p);

	if( tamanio_bloque == 0 ){
		sacar_bloque_de_metadata(mensaje_catch->pokemon,bloque_con_posicion);
	}

	compactar_bloques(bloques, mensaje_catch->pokemon);

	for(int i = 0; bloques[i] != NULL; i++) free(bloques[i]);
	free(bloques);
	free(posicion);
	free(block_p);

}

void compactar_bloques(char** bloques, char* pokemon){
	log_trace(logger, "Compactar bloques");

	int bloque_que_comparo = 0;
	int bloque_contra_el_que_comparo = 0;

	while( bloques[bloque_que_comparo] != NULL ){

		log_trace(logger, "En el while, comparando el bloque: %i.", bloque_que_comparo);

		while( bloques[bloque_contra_el_que_comparo] != NULL ){

			log_trace(logger, "En el while, comparando contra el bloque: %i.", bloque_contra_el_que_comparo);

			copactar_bloques_si_corresponde(bloque_contra_el_que_comparo,
							bloque_que_comparo, bloques, pokemon);

			bloque_contra_el_que_comparo++;

		}

		bloque_que_comparo++;
	}

}

void copactar_bloques_si_corresponde(int bloque_a_vaciar, int bloque_a_llenar,char** bloques, char* pokemon) {

	if(bloque_a_vaciar != bloque_a_llenar){

		log_trace(logger, "Se compararon distinos bloques.");

		int num_bloque_a_vaciar = atoi(bloques[bloque_a_vaciar]);

		char* block_vaciar_p = block_path(num_bloque_a_vaciar);
		int tamanio_que_comparo = tamanio_archivo(block_vaciar_p);


		log_trace(logger, "Bloque 1 de la comparacion: numero %i , tamanio: %i.",
				num_bloque_a_vaciar, tamanio_que_comparo);

		int num_bloque_a_llenar = atoi(bloques[bloque_a_llenar]);

		char* block_llenar_p = block_path(num_bloque_a_llenar);
		int tamanio_contra_el_que_comparo = tamanio_archivo(block_llenar_p);


		log_trace(logger, "Bloque 2 de la comparacion: numero %i , tamanio: %i.",
				num_bloque_a_llenar, tamanio_contra_el_que_comparo);

		int tamanio_sobrante_del_bloque_a_llenar = (tamanio_bloque() - tamanio_contra_el_que_comparo);

		if(  tamanio_que_comparo <= tamanio_sobrante_del_bloque_a_llenar ){

			log_trace(logger, "Se compararon los tamanios y se va a compactar.");
			//Tengo que pasar el bloque que compare => al archivo contra el que compare.
			//Voy a pasar la info del bloque 1 al 2.

			t_config* config_1 = config_create( block_vaciar_p);
			t_config* config_2 = config_create( block_llenar_p);

			t_dictionary* diccionario_1 = config_1->properties;

			void pasar_key_a_config(void* element){

				char* key = element;

					char* value = config_get_string_value(config_1, key);
					config_set_value(config_2, key, value);

					// NO PONER free(key). ni tampoco free(value) ROMPE

			}

			dictionary_iterator(diccionario_1,(void*) pasar_key_a_config);

			limpiar_file(block_vaciar_p);

			config_save(config_2);
			config_destroy(config_1);
			config_destroy(config_2);

			log_trace(logger, "Se va a actualizar size y sacar bloque del metadata.");

			actualizar_size_metadata(pokemon);

			sacar_bloque_de_metadata(pokemon,num_bloque_a_vaciar);

		}

		free(block_vaciar_p);
		free(block_llenar_p);

	}

}

void sacar_bloque_de_metadata(char* pokemon,int bloque_con_posicion){ // mucho repeticion de code
	t_config* config_metadata = read_pokemon_metadata(pokemon);
	char* bloques = extraer_bloques_string(pokemon);
	
	char* bloque_con_pos = string_itoa(bloque_con_posicion);
	char* concat_pos_corchete_antes = concat("[", bloque_con_pos);
	char* concat_pos_corchete_despues = concat(bloque_con_pos,"]");

	if (string_starts_with(bloques, concat_pos_corchete_antes)) {

		char* concat_pos_coma = concat(bloque_con_pos, ",");
		char**bloques_separados = string_split(bloques, concat_pos_coma);
		char* bloques_nuevos = string_new();
		string_append(&bloques_nuevos,bloques_separados[0]);
		string_append(&bloques_nuevos,bloques_separados[1]);
		config_set_value(config_metadata,"BLOCKS",bloques_nuevos);
		config_save(config_metadata);
		config_destroy(config_metadata);
		
		for(int i = 0; bloques_separados[i] != NULL; i++) free(bloques_separados[i]);
		free(bloques_separados);
		free(bloques_nuevos);
		free(concat_pos_coma);

	}

	else if(string_ends_with(bloques,concat_pos_corchete_despues)){

		char* concat_coma_antes = concat(",", bloque_con_pos);
		char**bloques_separados = string_split(bloques, concat_coma_antes);
		char* bloques_nuevos = string_new();

		string_append(&bloques_nuevos,bloques_separados[0]);
		string_append(&bloques_nuevos,bloques_separados[1]);
		config_set_value(config_metadata,"BLOCKS",bloques_nuevos);
		config_save(config_metadata);
		config_destroy(config_metadata);
		
		for(int i = 0; bloques_separados[i] != NULL; i++) free(bloques_separados[i]);
		free(bloques_separados);
		free(bloques_nuevos);
		free(concat_coma_antes);
		
	}

	else {
		char* aux = concat(bloque_con_pos,",");
		char* concat_aux = concat(",", aux);
		char**bloques_separados = string_split(bloques, concat_aux);
		char* bloques_nuevos = string_new();
		string_append(&bloques_nuevos,bloques_separados[0]);
		string_append(&bloques_nuevos,",");
		string_append(&bloques_nuevos,bloques_separados[1]);
		config_set_value(config_metadata,"BLOCKS",bloques_nuevos);
		config_save(config_metadata);
		config_destroy(config_metadata);
		

		for(int i = 0; bloques_separados[i] != NULL; i++) free(bloques_separados[i]);
		free(bloques_separados);
		free(bloques_nuevos);
		free(aux);
		free(concat_aux);

	}

	vaciar_bloque_bitmap(bloque_con_posicion);

	free(bloque_con_pos);
	free(concat_pos_corchete_antes);
	free(concat_pos_corchete_despues);
	free(bloques);

}

char* extraer_bloques_string(char* pokemon){
	t_config* config_bloque = read_pokemon_metadata(pokemon);
	char* bloques = config_get_string_value(config_bloque,"BLOCKS");
	//config_destroy(config_bloque); 			TODO NO SE POR Q ROMPE ESTO AYUDA
	return bloques;
}


void vaciar_bloque_bitmap(int bloque){
	pthread_mutex_lock(&mutex_bitmap);

	bitarray_clean_bit(bitmap_bloques,bloque);
	save_bitmap();

	pthread_mutex_unlock(&mutex_bitmap);
}

void save_bitmap() {
	log_trace(logger, "Save bitmap.");

	char* bitmap_p = bitmap_path();
	FILE* file_birarray = fopen(bitmap_p, "w");
	free(bitmap_p);

	for (int renglon = 0; renglon < ( cantidad_bloques()/8 ) ; renglon++) {

		for (int bit = 0; bit < 8; bit++) {

			fprintf(file_birarray, "%i", bitarray_test_bit(bitmap_bloques, renglon*8+bit) );

			//log_debug(logger, "SAVE BITARRAY bloque %i: %i",renglon*8+bit, bitarray_test_bit(bitmap_bloques, renglon*8+bit));

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
	log_trace(logger,"Entro a summar unidad pos");
	while(bloques[n]!=NULL){
		log_trace(logger,"Entro en el while de sumar unidad posicion");
		t_config* config_bloque = config_create(block_path(atoi(bloques[n])));
		if (config_has_property(config_bloque,posicion)){
			log_trace(logger,"Encontre bloque que tiene mi misma pos");
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
