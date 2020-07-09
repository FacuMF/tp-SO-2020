#include "utils_gamecard.c"

char* concat_dirs(char* start, char* end);
void set_base_path(char* base);
char* to_block_file(int blockNumber);

// BASE PATH

char* metadata_base_path();
char* blocks_base_path();
char* files_base_path();

// PATH DEFINITIVOS

char* block_path(int block);
char* metadata_path();
char* bitmap_path();
char* files_path();

// Files functions

void create_file(char* path);
void create_dir(char* path) ;
void delete_from_path(char* path);
void clean_file(char* path);
void clean_dir(char* path);

char* read_file(char* path, int size);
void write_file(char* path, char* data);

bool verificar_posiciones_file();



// Crear archivos

t_config* read_pokemon_metadata(char* table);
t_config* read_file_metadata(char* table);
void create_pokemon_dir(char* tableName);
void create_pokemon_metadata_file(char* tableName);
void create_new_file_pokemon(char* pokemon);



// Funciones generales

void suscribirse_a(int* conexion, int cola);
bool file_existing(char* path);
bool file_open(char* pokemon);
char* extraer_bloques(char* pokemon);
void recibir_mensajes_gamecard(int *socket);
bool verificar_posciones_file(int x, int y, char** bloques);
void crear_file_si_no_existe(char* file, t_new_pokemon pokemon);
t_appeared_pokemon convertir_a_appeared_pokemon(t_new_pokemon pokemon);
void enviar_appeared_pokemon_a_broker(int broker, t_new_pokemon pokemon);
void handle_mensajes_gamecard(int cod_op, int socket);


// Manejo de mensajes

void gamecard_manejar_new_pokemon(t_conexion_buffer * combo);

void gamecard_manejar_catch_pokemon(t_conexion_buffer * combo);

void gamecard_manejar_get_pokemon(t_conexion_buffer * combo);




