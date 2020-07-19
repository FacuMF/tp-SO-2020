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


/*bool verificar_posiciones_file(){
	// TODO
	return true;
}*/

// Base de crear archivos

t_config* read_pokemon_metadata(char* table) {
    return config_create(pokemon_metadata_path(table));
}

t_config* read_file_metadata(char* table){
	return config_create(files_base_path(table));
}

void create_pokemon_dir(char* tableName) {
    create_dir(files_base_path(tableName));
    t_config* config_directorio = read_file_metadata(tableName);
    config_set_value(config_directorio, "DIRECTORY", "Y");
    config_save(config_directorio);
    config_destroy(config_directorio);
}
//CAMBIAR A FWRITE
void create_pokemon_metadata_file(char* tableName){
    create_file(pokemon_metadata_path(tableName));
    t_config* config_file = read_pokemon_metadata(tableName);
    config_save_in_file(config_file, "DIRECTORY", "N");
    config_save_in_file(config_file, "SIZE", "0"); // A DEFINIR
    config_save_in_file(config_file, "BLOCKS", "[]"); // A DEFINIR
    config_save_in_file(config_file, "OPEN", "Y"); // A DEFINIR
    config_save(config_file);
    config_destroy(config_file);
}


// Crear archivos

void create_new_file_pokemon(char* pokemon) {
    create_pokemon_dir(pokemon);
    create_pokemon_metadata_file(pokemon);
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
	t_config config_tamanio = leer_config(metadata_path());
	return config_get_int_value(config_tamanio,"BLOCK_SIZE");
}

int cantidad_bloques(){
	t_config config_cantidad = leer_config(metadata_path());
	return config_get_int_value(config_cantidad, "BLOCKS");
}

float size_bytes(char* data) {
    return 1.0 * sizeof(char) * string_length(data);
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

// MAL, BORRAR
int buscar_siguiente_bloque(){
	//int numero_bloque = config_get_int_value(config,); TODO : VER CON QUE KEY EN ARCHIVO DE CONFIG CORRESPONDE
	int numero_bloque = 0;
	while(1){
		if(numero_bloque){
			if (numero_bloque < cantidad_bloques()){
				numero_bloque +=1;
			} else {
				log_error(logger,"No hay bloques disponibles para almacenar mas informacion");
			}
		} else {
			return numero_bloque;
		}
	}
}

void asignar_bloque(t_new_pokemon* mensaje_new, char** bloques){
	int contador = 0;
	while(bitarray_test_bit(bitmap_bloques,contador)){
		contador++;
	}
	// asignar bloque al metadata pokemon, escribir la posicion y la cantidad al bloque.bin
	t_config* config_metadata = read_pokemon_metadata(mensaje_new->pokemon);
	t_config* config_bloque = block_path(contador);
	char** bloques_nuevos = agregar_bloque_metadata(bloques,contador);
	char* posicion = concatenar_posicion(mensaje_new);
	config_save_in_file(config_bloque, posicion, mensaje_new->cantidad);
	config_set_value(config_metadata, "BLOQUES", bloques_nuevos);
	bitarray_set_bit(bitmap_bloques,contador); // Esperemos que lo setee en 1
}

char** agregar_bloque_metadata(char**bloques, int bloque_nuevo){
	 // TODO, CAPAZ CON CONCAT FUNCIONA
}

/*
char* read_block(int blockNumber) {
    return read_file(block_path(blockNumber), read_block_size());
}
*/



