#include "gamecard.h"

char *concat(char *start, char *end)
{
    return string_from_format("%s%s", start, end);
}

char *concat_igual(char *start, char *end)
{
    return string_from_format("%s=%s", start, end);
}


char* concat_dirs(char* start, char* end) {
	char* conca = concat(start, end);
	char* rtn = concat(conca, "/");
	free(conca);
	return rtn;
}




char* to_block_file(int blockNumber) {
	char* block = string_itoa(blockNumber);
	char* rtn = concat(block, EXTENSION);
	free(block);
	return rtn;
}

// BASE PATH

char* metadata_base_path() {
    return concat_dirs(PUNTO_MONTAJE, METADATA_BASE_PATH); //TALLGRASS/Metadata/
}

char* blocks_base_path() {
    return concat(PUNTO_MONTAJE, BLOCKS_BASE_PATH); //TALLGRASS/Blocks/
}


char* files_base_path(char* fileName) {
	char* conca = concat(PUNTO_MONTAJE, FILES_BASE_PATH);
	char* rtn = concat_dirs(conca, fileName);
	free(conca);
	return rtn; //TALLGRASS/Files/Pikachu/
}


// ----- PATH DEFINITIVOS -----

char* block_path(int block) {
	char* bloque = blocks_base_path();
	char* a_bloque = to_block_file(block);
	char* rtn = concat(bloque, a_bloque);
	free(bloque);
	free(a_bloque);
	return rtn; //TALLGRASS/Blocks/1.bin
}

char* metadata_path() {
	char* meta = metadata_base_path();
	char* rtn = concat(meta, METADATA_FILE_NAME);
	free(meta);
	return rtn; //TALLGRASS/Metadata/Metadata.bin
}

char* bitmap_path() {
	char* meta = metadata_base_path();
	char* rtn = concat(meta, BITMAP_FILE_NAME);
	free(meta);
	return rtn; //TALLGRASS/Metadata/Bitmap.bin
}

char* pokemon_metadata_path(char* fileName) {
	char* a_file = files_base_path(fileName);
	char* rtn = concat(a_file, METADATA_FILE_NAME);
	free(a_file);
	return rtn; //TALLGRASS/Files/Pikachu/Metadata.bin
}




