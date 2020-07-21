#include "gamecard.h"

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

void crear_pokemon_dir(char* tableName) { // TODO: CAMBIAR A FOPEN, fwrite ETC
    create_dir(files_base_path(tableName));
    t_config* config_directorio = read_file_metadata(tableName);
    config_set_value(config_directorio, "DIRECTORY", "Y");
    config_save(config_directorio);
    config_destroy(config_directorio);
}
// TODO : CAMBIAR A FWRITE
void crear_pokemon_metadata_file(char* tableName){
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

bool verificar_posiciones_file(int x, int y, char** bloques){
	//TODO
	return true;
}

void crear_file_si_no_existe(char* file, char* pokemon){
	if(!file_existing(file)){
		crear_pokemon_dir(pokemon);
		crear_pokemon_metadata_file(pokemon);
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



char* crear_sentencia(int posx, int posy, int cantidad){
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
//Falta while a todas las sentencias
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

char* separar_posicion(char* palabra){ // Separa Ej "1-2=13" a "1-2"
	return strtok(palabra,"=");
}





