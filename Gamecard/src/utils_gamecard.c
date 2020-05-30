#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

// CONSTANT
char* PUNTO_MONTAJE;
char* METADATA_BASE_PATH = "Metadata/";
char* FILES_BASE_PATH = "Files/";
char* BLOCKS_BASE_PATH = "Blocks/";

char* METADATA_FILE_NAME = "Metadata.bin";
char* BITMAP_FILE_NAME = "Bitmap.bin";

// EXTENSIONES
char* EXTENSION = ".bin";

char* concat_dirs(char* start, char* end) {
    return concat(concat(start, end), "/");
}


void set_base_path(char* base) {
    PUNTO_MONTAJE = base;
}

// Metadata files?
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

char* files_base_path() {
    return concat(PUNTO_MONTAJE, FILES_BASE_PATH);
}

// PATH DEFINITIVOS

char* block_path(int block) {
    return concat(blocks_base_path(), to_block_file(block));
}

char* metadata_path() {
    return concat(metadata_base_path(), METADATA_FILE_NAME);
}

char* bitmap_path() {
    return concat(metadata_base_path(), BITMAP_FILE_NAME);
}

char* files_path() {
    return concat(?? , ??); // VER
}


void listen(int port) {
    printf("TODO: Listen %d\n", port);
}

void create_file(char* path) {
    FILE* file = fopen(path, "rb");
    if(file == NULL){
    	fopen(path, "wb");
    } else {
    	printf("El archivo ya existe"); //log?
    }
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


