#include "gamecard.h"

char *concat(char *start, char *end)
{
    return string_from_format("%s%s", start, end);
}

char* concat_dirs(char* start, char* end) {
    return concat(concat(start, end), "/");
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




