#include "gamecard.h"

t_config* read_pokemon_metadata(char* table) {
	char* poke_p = pokemon_metadata_path(table);
	log_trace(logger,"%s", poke_p);
	t_config* config_pokemon = config_create(poke_p);
	free(poke_p);
    return config_pokemon;
}

t_config* read_file_metadata(char* table){
	char* file_p = files_base_path(table);
	t_config* config_file = config_create(file_p);
	free(file_p);
	return config_file;
}

char** extraer_bloques(char* pokemon){
	t_config* config_bloques = read_pokemon_metadata(pokemon);
	char** bloques = config_get_array_value(config_bloques,"BLOCKS");
	config_destroy(config_bloques);
	return bloques;
}

bool verificar_posiciones_file(char* posicion, char** bloques){
	int n=0;
	while(bloques[n]!=NULL){

		char* block_p = block_path(atoi(bloques[n]));
		t_config* config_bloque = config_create(block_p);
		free(block_p);

		if (config_has_property(config_bloque,posicion)) {
			config_destroy(config_bloque);
			return true;
		}
		config_destroy(config_bloque);
		n++;
	}
	return false;
}

char* leer_sentencia(char* fileName){
  	FILE *file = fopen(fileName, "r");
    char *code = malloc(ftell(file));
    size_t n = 0;
    int c;
    if (file == NULL) return NULL;
    while ((c = fgetc(file)) != '\n') // La sentencia entera. Ej "1-2=5"
    {
        code[n++] = (char) c;
    }
    code[n] = '\0';
    return code; //Falta fclose
}


// Crear archivos

void create_file(char* path) {
    FILE* file = fopen(path, "w");
    fclose(file);
}

void create_dir(char* path) {
    mkdir(path, 0777);    // 0777?
}

void crear_pokemon_dir(char* tableName) {

	char* files_base_p = files_base_path(tableName);
	log_trace(logger, "Crear directorio: %s", files_base_p);
	create_dir(files_base_p);
	free(files_base_p);

	char* pm_files_base_p = concat(PUNTO_MONTAJE, FILES_BASE_PATH);
	char* metadata_bin = concat(pm_files_base_p, METADATA_FILE_NAME);


    log_trace(logger, "Crear metadata directorio: %s", metadata_bin );

    create_file( metadata_bin );

    log_trace(logger, "Ya se creo.");
	t_config* config_directorio = config_create(metadata_bin);

    config_set_value(config_directorio, "DIRECTORY", "Y");
    config_save(config_directorio);
    config_destroy(config_directorio);
    free(pm_files_base_p);
    free(metadata_bin);

}


void crear_pokemon_metadata_file(char* tableName){

	char* poke_meta_p = pokemon_metadata_path(tableName);
	log_info(logger,"Se va a crear file en: %s", poke_meta_p);
	create_file( poke_meta_p );
	log_trace(logger, poke_meta_p);
	free(poke_meta_p);

	t_config* config_file = read_pokemon_metadata(tableName);

    config_set_value(config_file, "DIRECTORY", "N");
    config_set_value(config_file, "SIZE", "0");
    config_set_value(config_file, "BLOCKS", "[]");
    config_set_value(config_file, "OPEN", "N");
    config_save(config_file);
    config_destroy(config_file);
}


void create_new_file_pokemon(char* pokemon) {
    crear_pokemon_dir(pokemon);
    crear_pokemon_metadata_file(pokemon);
}


bool file_existing(char* path){
	FILE * file = fopen(path, "rb");
	if(file == NULL){
		return false;
	} else {
	fclose(file);
	return true;
	}
}


void delete_from_path(char* path){
	char* comando = concat("rm -rf ",path);
	system(comando);
	free(comando);
}

void limpiar_file(char* path){
	delete_from_path(path);
	create_file(path);
}

void borrar_directorios(){
	delete_from_path(PUNTO_MONTAJE);
}


// TAMANIOS DE ARCHIVOS Y SUS ATRIBUTOS
int tamanio_bloque(){
	char* meta_p = metadata_path();
	t_config* config_tamanio = leer_config(meta_p);
	int tamanio = config_get_int_value(config_tamanio,"BLOCK_SIZE");
	config_destroy(config_tamanio);
	free(meta_p);
	return tamanio;
}

int cantidad_bloques(){
	char* meta_p = metadata_path();
	t_config* config_cantidad = leer_config(meta_p);
	int cantidad = config_get_int_value(config_cantidad, "BLOCKS");
	config_destroy(config_cantidad);
	free(meta_p);
	return cantidad;
}

int size_bytes(char* data) {
    return sizeof(char) * string_length(data); // 1.0 * etc
}

bool sentencia_sobrepasa_tamanio_maximo(int posx, int posy, int cantidad){
	char* px = string_itoa(posx);
	char* py = string_itoa(posy);
	char* cant = string_itoa(cantidad);
	int tamanio_sentencia = string_length(px) + string_length(py) + string_length(cant) + 2;

	free(px);
	free(py);
	free(cant);

	return tamanio_sentencia > tamanio_bloque();
}

int tamanio_archivo(char* path){
	FILE * file = fopen(path,"r");
	fseek(file, 0L, SEEK_END);
	int tamanio = ftell(file);
	log_trace(logger,"El archivo actualmente ocupa: %d \n",tamanio);
	fclose(file);
	return tamanio;
}


// MANEJO DE STRINGS
char* concatenar_posicion(int posx, int posy){
	char* posicion_parcial = concat(string_itoa(posx), "-");
	char* posicion_definitiva = concat(posicion_parcial, string_itoa(posy));

	free(posicion_parcial);

	return posicion_definitiva;
}
char* separar_posicion(char* palabra){ // Separa Ej "1-2=13" a "1-2"
	return strtok(palabra,"=");
}

t_posicion* de_char_a_posicion(char* string_posicion){ // Ej "1-2"
	t_posicion* posicion = malloc(sizeof(t_posicion));
	char** aux = string_split(string_posicion,"-");
	posicion->x = atoi(aux[0]);
	posicion->y = atoi(aux[1]);
	return posicion;
}



// Manejo de asignacion de bloques

void asignar_bloque(t_new_pokemon* mensaje_new, int posicion_existente){
	int contador = 0;


	while(bitarray_test_bit(bitmap_bloques,contador) && contador <= cantidad_bloques()){
		log_trace(logger,"Bit: %d estado: %d",contador, bitarray_test_bit(bitmap_bloques,contador));
		contador++;
	}

	if(contador > cantidad_bloques()){
		log_error(logger,"No hay bloques disponibles para asignar informacion al pokemon %s.",mensaje_new->pokemon);
		return;
	}
	log_trace(logger,"Va a proceder a asignar el bloque vacio:  %d",contador);

	asignar_bloque_vacio(mensaje_new, contador, posicion_existente);

}

void asignar_bloque_vacio(t_new_pokemon* mensaje_new, int contador, int posicion_existente){
	char** bloques_pokemon = extraer_bloques(mensaje_new->pokemon);
	char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);

	char* block_p = block_path(contador);
	t_config* config_bloque_nuevo = config_create(block_p);
	free(block_p);

	if(posicion_existente){
		log_trace(logger,"La posicion ya existia, se va a borrar la sentencia y escribir en otro bloque");
		int bloque_viejo = encontrar_bloque_con_posicion(posicion,bloques_pokemon);
		char* block_p = block_path(bloque_viejo);
		t_config* config_bloque_viejo = config_create(block_p);
		free(block_p);
		int cantidad_vieja = config_get_int_value(config_bloque_viejo,posicion);
		int cantidad_total = cantidad_vieja + mensaje_new->cantidad;

		if (sentencia_sobrepasa_tamanio_maximo(mensaje_new->posx,mensaje_new->posy,cantidad_total)){
			log_error(logger,"La sentencia sobrepasa el tamanio maximo de bloque.");
			return;
		}

		config_remove_key(config_bloque_viejo,posicion);
		char* cant_t = string_itoa(cantidad_total);
		config_set_value(config_bloque_nuevo, posicion, cant_t);
		free(cant_t);

		config_save(config_bloque_viejo);
		config_destroy(config_bloque_viejo);

	}else{
		if (sentencia_sobrepasa_tamanio_maximo(mensaje_new->posx,mensaje_new->posy,mensaje_new->cantidad)){
			log_error(logger,"La sentencia sobrepasa el tamanio maximo de bloque.");
			return;
		}
		log_trace(logger,"Se escribe la posicion en otro bloque ya que no existia");
		char* cant_n = string_itoa(mensaje_new->cantidad);
		config_set_value(config_bloque_nuevo, posicion, cant_n);
		free(cant_n);

	}
	log_trace(logger,"Termino agregar bloque vacio");
	agregar_bloque_metadata(mensaje_new->pokemon,contador);
	config_save(config_bloque_nuevo);
	config_destroy(config_bloque_nuevo);
	actualizar_size_metadata(mensaje_new->pokemon); // el save y destroy esta puesto en el actualizar

	pthread_mutex_lock(&mutex_bitmap);
	bitarray_set_bit(bitmap_bloques,contador);
	save_bitmap();
	pthread_mutex_unlock(&mutex_bitmap);

	free(posicion);
	for(int i=0;bloques_pokemon[i] != NULL;i++) free(bloques_pokemon[i]);
	free(bloques_pokemon);
}

int encontrar_bloque_con_posicion(char* posicion, char** bloques){
	int n=0;
	while(bloques[n]!=NULL){
		char* block_p = block_path(atoi(bloques[n]));
		t_config* config_bloque = config_create(block_p);
		free(block_p);

		if (config_has_property(config_bloque,posicion)){
			config_destroy(config_bloque);
			return atoi(bloques[n]);
		}
		config_destroy(config_bloque);
		n++;
	}
	log_trace(logger,"No se encontro bloque que contiene la posicion: %s",posicion);
	return 0; // return 0 si no encuentra el bloque?
}



void agregar_bloque_metadata(char* pokemon, int bloque_nuevo) {
	char* poke_meta = pokemon_metadata_path(pokemon);
	t_config* config_metadata = config_create(poke_meta);
	log_trace(logger,"Path del metadata yendo a agregar el bloque: %s",poke_meta);
	free(poke_meta);
	
	char* bloques = config_get_string_value(config_metadata,"BLOCKS");
	char* bloques_final;


	char* bloque_n = string_itoa(bloque_nuevo);
	char* conca_coma = concat(bloque_n, ",");
	char* conca_cor = concat(bloque_n, "]");
	char* conca1 = concat("[", conca_coma);
	char* conca2 = concat(",", conca_coma);
	char* conca3 = concat(",", conca_cor);
	char* conca4 = concat("[", conca_cor);

	bool esta_al_principio = string_contains(bloques, conca1);
	bool esta_al_medio = string_contains(bloques, conca2);
	bool esta_al_final = string_contains(bloques, conca3);
	bool es_el_unico = string_contains(bloques, conca4);



	if(esta_al_principio || esta_al_medio || esta_al_final || es_el_unico) return;

	if(strlen(bloques)==2){
		char* bloque_parcial = concat("[", bloque_n);
		bloques_final = concat(bloque_parcial, "]");
		free(bloque_parcial);
	}else{
		bloques[strlen(bloques)-1] = ',';
		char* aux1 = concat(bloques,bloque_n);
		bloques_final = concat(aux1,"]");
		free(aux1);
	}


	config_set_value(config_metadata,"BLOCKS",bloques_final);
	log_trace(logger,"Lo que se seteo: %s",config_get_string_value(config_metadata,"BLOCKS"));
	log_trace(logger,"Bloques final: %s",bloques_final);
	config_save(config_metadata);
	config_destroy(config_metadata);


	free(bloque_n);
	free(conca_coma);
	free(conca_cor);
	free(conca1);
	free(conca2);
	free(conca3);
	free(conca4);
	free(bloques_final);
	//NO HACER free(bloques) ROMPE TOD0

}

void actualizar_size_metadata(char* pokemon){
	char* meta_p = pokemon_metadata_path(pokemon);
	t_config* config_metadata = config_create(meta_p);
	int tamanio_definitivo = tamanio_todos_los_bloques(extraer_bloques(pokemon));
	char* tamanio_d = string_itoa(tamanio_definitivo);
	config_set_value(config_metadata, "SIZE", tamanio_d);

	config_save(config_metadata);
	config_destroy(config_metadata);
	free(meta_p);
	free(tamanio_d);
}

// Funciones de handle localized

t_localized_pokemon* obtener_pos_y_cant_localized(t_get_pokemon* mensaje_get){ // TODO: VER
	char** bloques = extraer_bloques(mensaje_get->pokemon);
	int n = 0;
	t_list* lista_posiciones = list_create();

	while(bloques[n]!=NULL){
		log_trace(logger, "Entra a while bloques");
		char* block_p = block_path(atoi(bloques[n]));
		t_config* config_bloque = config_create(block_p);
		//free(block_p);
		log_trace(logger, "Config bloque creado");

		t_dictionary* diccionario_bloque = config_bloque->properties;

		log_trace(logger, "Por hacer iteractor");

		void agregar_posicion_a_lista(void* element){
			char* key = element;
			t_posicion* posicion_dividida = de_char_a_posicion(key);

			log_trace(logger, "Posicion a agregar: %s.", key);

			list_add(lista_posiciones,posicion_dividida);

		}

		dictionary_iterator(diccionario_bloque, (void*) agregar_posicion_a_lista);


		n++;
		config_destroy(config_bloque);
	}

	t_localized_pokemon* pokemon_localized = crear_localized_pokemon(-10,mensaje_get->pokemon,lista_posiciones, mensaje_get->id_mensaje);

	//for(int i=0;bloques[i] != NULL;i++) free(bloques[i]);
	//free(bloques);
	return pokemon_localized;
}


