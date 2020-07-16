#include "gamecard.h"

char *concat(char *start, char *end)
{
    return string_from_format("%s%s", start, end);
}

char* concat_dirs(char* start, char* end) {
    return concat(concat(start, end), "/");
}


void set_base_path(char* base) {
    PUNTO_MONTAJE = base;
}

char* to_block_file(int blockNumber) {
    return concat(string_itoa(blockNumber), EXTENSION);
}

// BASE PATH

char* metadata_base_path() {
    return concat(PUNTO_MONTAJE, METADATA_BASE_PATH);
}

char* blocks_base_path() {
    return concat(PUNTO_MONTAJE, BLOCKS_BASE_PATH);
}


char* files_base_path(char* fileName) {
    return concat_dirs(concat(PUNTO_MONTAJE, FILES_BASE_PATH), fileName);
}


// ----- PATH DEFINITIVOS -----

char* block_path(int block) {
    return concat(blocks_base_path(), to_block_file(block));
}

char* metadata_path() {
    return concat(metadata_base_path(), METADATA_FILE_NAME);
}

char* bitmap_path() {
    return concat(metadata_base_path(), BITMAP_FILE_NAME);
}

char* pokemon_metadata_path(char* fileName) {
    return concat(files_base_path(fileName), METADATA_FILE_NAME);
}

// Manejo de archivos

void create_file(char* path) {
    FILE* file = fopen(path, "w");
    fclose(file);
}

void create_dir(char* path) {
    mkdir(path, 0777);    // 0777?
}


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


bool verificar_posiciones_file(){
	// TODO
	return true;
}
// Base de crear archivos

t_config* read_pokemon_metadata(char* table) {
    return config_create(pokemon_metadata_path(table));
}

t_config* read_file_metadata(char* table){
	return config_create(files_base_path(table));
}

void create_pokemon_dir(char* tableName) {
    create_dir(files_base_path(tableName));
    t_config* config = read_file_metadata(tableName);
    config_set_value(config, "DIRECTORY", "Y");
    config_save(config);
    config_destroy(config);
}

void create_pokemon_metadata_file(char* tableName){
    create_file(pokemon_metadata_path(tableName));
    t_config* config = read_pokemon_metadata(tableName);
    config_set_value(config, "DIRECTORY", "N");
    config_set_value(config, "SIZE", "0"); // A DEFINIR
    config_set_value(config, "BLOCKS", "[]"); // A DEFINIR
    config_set_value(config, "OPEN", "Y"); // A DEFINIR
    config_save(config);
    config_destroy(config);
}


// Crear archivos

void create_new_file_pokemon(char* pokemon) {
    create_pokemon_dir(pokemon);
    create_pokemon_metadata_file(pokemon);
}

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

bool verificar_posiciones_file(int x, int y, char** bloques){
	//TODO
	return true;
}

void crear_file_si_no_existe(char* file, char* pokemon){
	if(!file_existing(file)){
		create_pokemon_dir(pokemon);
		create_pokemon_metadata_file(pokemon);
	}
}

// Funciones Bitmap

int tamanio_bloque(){
	config = leer_config(metadata_path());
	return config_get_int_value(config,"BLOCK_SIZE");
}

int cantidad_bloques(){
	config = leer_config(metadata_path());
	return config_get_int_value(config, "BLOCKS");
}

float size_bytes(char* data) {
    return 1.0 * sizeof(char) * string_length(data);
}

float tamanio_archivo(char* path){
	FILE * file = fopen(path,"r");
	fseek(file, 0L, SEEK_END);
	int tamanio = ftell(file);
	log_trace("El archivo actualmente ocupa: %d \n",tamanio);
	fclose(file);
	return tamanio;
}

char** extraer_bloques(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	return config_get_array_value(config,"BLOCKS");
}

int buscar_siguiente_bloque(t_config config){
	int numero_bloque = 0; // inicializo numero_bloque?
	while(1){
		if(config_get_int_value(config,numero_bloque)){ // config del bitmap
			if (numero_bloque < cantidad_bloques()){
				numero_bloque +=1;
			} else {
				log_error("No hay bloques disponibles para almacenar mas informacion");
			}
		} else {
			return numero_bloque;
		}
	}
}

void asignar_bloques(int bloque){
	// Escribir en el archivo, despues fijarme su tamanio. Si es mayor al debido, retroceder.
	if(tamanio_archivo(block_path(bloque)) <= tamanio_bloque()){

	}
}

/*
char* read_block(int blockNumber) {
    return read_file(block_path(blockNumber), read_block_size());
}
*/



