#include "gamecard.h"

// Manejo de archivos

void create_file(char* path) {
    FILE* file = fopen(path, "w");
    fclose(file);
}

void create_dir(char* path) {
    mkdir(path, 0777);    // 0777?
}

/* FIJAR SI NECESITO ESTAS FUNCIONES
void delete_from_path(char* path) {
    system(concat("rm -rf ", path));
}


void clean_file(char* path) {
    delete_from_path(path);
    create_file(path);
}

void clean_dir(char* path) {
    delete_from_path(path);
    create_dir(path);
}
*/

// Base de crear archivos

t_config* read_pokemon_metadata(char* table) {
	t_config* config_pokemon = config_create(pokemon_metadata_path(table));
    return config_pokemon;
}

t_config* read_file_metadata(char* table){
	return config_create(files_base_path(table));
}

void crear_pokemon_dir(char* tableName) {
	create_dir(files_base_path(tableName));
    t_config* config_directorio = read_file_metadata(tableName);
    config_set_value(config_directorio, "DIRECTORY", "Y");
    config_save(config_directorio);
    config_destroy(config_directorio);
}


void crear_pokemon_metadata_file(char* tableName){
    t_config* config_file = read_pokemon_metadata(tableName);
    config_set_value(config_file, "DIRECTORY", "N");
    config_set_value(config_file, "SIZE", "0"); // A DEFINIR
    config_set_value(config_file, "BLOCKS", "[]"); // A DEFINIR
    config_set_value(config_file, "OPEN", "Y"); // A DEFINIR
    config_save(config_file);
    config_destroy(config_file);
}


// Crear archivos

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

bool intentar_abrir_archivo(char* pokemon){
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
		crear_pokemon_dir(pokemon);
		crear_pokemon_metadata_file(pokemon);
	}
}

// Funciones Bitmap

int tamanio_bloque(){
	t_config* config_tamanio = leer_config(metadata_path());
	return config_get_int_value(config_tamanio,"BLOCK_SIZE");
}

int cantidad_bloques(){
	t_config* config_cantidad = leer_config(metadata_path());
	return config_get_int_value(config_cantidad, "BLOCKS");
}

int size_bytes(char* data) {
    return sizeof(char) * string_length(data); // 1.0 * etc
}

float tamanio_archivo(char* path){
	FILE * file = fopen(path,"r");
	fseek(file, 0L, SEEK_END);
	int tamanio = ftell(file);
	log_trace(logger,"El archivo actualmente ocupa: %d \n",tamanio);
	fclose(file);
	return tamanio;
}

char** extraer_bloques(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	return config_get_array_value(config,"BLOCKS");
}

bool verificar_posiciones_file(char* posicion, char** bloques){
	int n=0;
	while(bloques[n]!=NULL){
		t_config* config_bloque = config_create(block_path(atoi(bloques[n]))); // atoi?
		if (config_has_property(config_bloque,posicion)) return true;
		config_destroy(config_bloque);
		n++;
	}
	return false;
}


void asignar_bloque(t_new_pokemon* mensaje_new, int posicion_existente){
	int contador = 1;
	char** bloques_pokemon = extraer_bloques(mensaje_new->pokemon);
	char* posicion = concatenar_posicion(mensaje_new->posx,mensaje_new->posy);

	while(bitarray_test_bit(bitmap_bloques,contador) && contador <= cantidad_bloques()){ // No hay 0.bin?
		contador++;
	}
	if(contador > cantidad_bloques()){
		log_error("No hay bloques disponibles para asignar informacion al pokemon %s.",mensaje_new->pokemon);
		return;
	}

	t_config* config_metadata = read_pokemon_metadata(mensaje_new->pokemon);
	t_config* config_bloque_nuevo = block_path(contador);

	if(posicion_existente){
		int bloque_viejo = encontrar_bloque_con_posicion(posicion,bloques_pokemon);
		t_config* config_bloque_viejo = block_path(bloque_viejo);
		int cantidad_vieja = config_get_int_value(config_bloque_viejo,posicion);
		int cantidad_total = cantidad_vieja + mensaje_new->cantidad;
		if (sentencia_sobrepasa_tamanio_maximo(mensaje_new->posx,mensaje_new->posy,cantidad_total)){
			log_error(logger,"La sentencia sobrepasa el tamanio maximo de bloque.");
			return;
		}
		config_remove_key(config_bloque_viejo,posicion);
		config_set_value(config_bloque_nuevo, posicion, cantidad_total);

		config_save(config_bloque_viejo);
		config_destroy(config_bloque_viejo);

	}else{
		if (sentencia_sobrepasa_tamanio_maximo(mensaje_new->posx,mensaje_new->posy,mensaje_new->cantidad)){
			log_error(logger,"La sentencia sobrepasa el tamanio maximo de bloque.");
			return;
		}
		config_set_value(config_bloque_nuevo, posicion, mensaje_new->cantidad);

	}
	actualizar_size_metadata(config_metadata, bloques_pokemon);
	agregar_bloque_metadata(config_metadata,contador);
	config_save(config_bloque_nuevo);
	config_destroy(config_bloque_nuevo);
	bitarray_set_bit(bitmap_bloques,contador); // Esperemos que lo setee en 1

}

int encontrar_bloque_con_posicion(char* posicion, char** bloques){
	int n=0;
	while(bloques[n]!=NULL){
		t_config* config_bloque = config_create(block_path(atoi(bloques[n]))); // atoi?
		if (config_has_property(config_bloque,posicion)) return n;
		config_destroy(config_bloque);
		n++;
	}
	log_trace("No se encontro bloque que contiene la posicion: %s",posicion);
	return 0; // return 0 si no encuentra el bloque?
}

bool sentencia_sobrepasa_tamanio_maximo(int posx, int posy, int cantidad){
	int tamanio_sentencia = string_length(string_itoa(posx)) + string_length(string_itoa(posy)) + string_length(string_itoa(cantidad)) + 2; // CHEQUEAR
	return tamanio_sentencia > tamanio_bloque();
}

void agregar_bloque_metadata(t_config* config_metadata, int bloque_nuevo){ // Chequear bien esta funcion
	char* bloques = config_get_string_value(config_metadata,"BLOCKS");
	bloques[strlen(bloques)-1] = ',';
	char* aux1 = concat(bloques,string_itoa(bloque_nuevo));
	char* bloques_final = concat(aux1,"]");
	config_set_value(config_metadata,"BLOCKS",bloques_final);
	config_save(config_metadata);
	config_destroy(config_metadata);

}

void actualizar_size_metadata(t_config* config_metadata, bloques){
	int tamanio_definitivo = tamanio_todos_los_bloques(bloques);
	config_set_value(config_metadata, "SIZE", tamanio_definitivo);

}


t_localized_pokemon* obtener_pos_y_cant_localized(t_get_pokemon* mensaje_get){ // TODO: VER
	char** bloques = extraer_bloques(mensaje_get->pokemon);
	int n = 0;
	t_list* lista_posiciones = list_create();
	while(bloques[n]!=NULL){
		t_config* config_bloque = config_create(block_path(bloques[n]));
		int cantidad_posiciones = config_keys_amount(config_bloque);
		for(int i = 0; i < cantidad_posiciones; i++){
			char* sentencia = leer_sentencia(block_path(bloques[n]));
			char* posicion = separar_posicion(sentencia);
			t_posicion* posicion_dividida = de_char_a_posicion(posicion);
			list_add(lista_posiciones,posicion_dividida);
			config_remove_key(config_bloque,posicion);
		}
		n++;
		config_destroy(config_bloque);
	}
	t_localized_pokemon* pokemon_localized = crear_localized_pokemon(mensaje_get->id_mensaje,mensaje_get->pokemon,lista_posiciones);
	return pokemon_localized;
}


t_posicion* de_char_a_posicion(char* string_posicion){ // Ej "1-2"
	t_posicion* posicion = malloc(sizeof(t_posicion*));
	char** aux = string_split(string_posicion,"-");
	posicion->x = atoi(aux[0]);
	posicion->y = atoi(aux[1]);
	return posicion;
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


char* separar_posicion(char* palabra){ // Separa Ej "1-2=13" a "1-2"
	return strtok(palabra,"=");
}

/*
char* read_block(int blockNumber) {
    return read_file(block_path(blockNumber), read_block_size());
}
*/

/*

char* crear_sentencia(int posx, int posy, int cantidad){ // Ej 1-2=10
	char* aux1 = concat(posx,"-");
	char* aux2 = concat(aux1,posy);
	char* aux3 = concat(aux2,"=");
	char* aux4 = concat(aux3,cantidad);
	char* definitivo = concat(aux4,"\n"); // TODO: Ver bien si se pone el renglon
	return definitivo;
}

void escribir_sentencia(char* file, char* sentencia){
	FILE *fp = fopen(file, "a");
		fwrite(sentencia, strlen(sentencia) + 1, 1, fp);
	fclose(fp);
}

void escribir_atributo(char* file, char* atributo, char*propiedad){
	char* aux = concat_igual(atributo,propiedad);
	char* resultado = concat(aux,"\n");
	escribir_sentencia(file,resultado);
}


//Falta while a todas las sentencias usar feof()
bool buscar_posicion(char* fileName, char* pos){
	FILE *file = fopen(fileName, "r");
    char *code = malloc(ftell(file));
    size_t n = 0;
    int c;
	while ((c = fgetc(file)) != '\n'){  // Sentencia entera
		code[n++] = (char) c;
	}
	code[n] = '\0';
	char* posicion_posible = separar_posicion(code); // Falta fclose
	return pos == posicion_posible; // No funciona no se por qué
}



*/

/*
void crear_pokemon_metadata_file(char* tableName){
    char* file = pokemon_metadata_path(tableName);
	create_file(file);
    escribir_atributo(file,"DIRECTORY","N");
    escribir_atributo(file,"SIZE","0");
    escribir_atributo(file,"BLOCKS","[]"); // Ver
    escribir_atributo(file,"OPEN","Y");
}
*/


/* Viejas funciones, borrar si no sirven
char* read_file(char* path, int size) {
    FILE* file = fopen(path, "rb");
    char* result = string_repeat('\0', size);
    char* input = string_repeat('\0', 2);
    while(fread(input, sizeof(char), 1, file)) {
        string_append(&result, input);
    }
    fclose(file);
    free(input); //
    return result;
}

void write_file(char* path, char* data) {
    FILE* file = fopen(path, "wb");
    fwrite(data, sizeof(char), string_length(data) + 1, file);
    fclose(file);
}
*/



