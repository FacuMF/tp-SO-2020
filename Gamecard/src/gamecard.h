#ifndef UTILS_GAMECARD_
#define UTILS_GAMECARD_
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"
#include "../../Broker/src/broker.h"

#include <commons/collections/list.h>
#include <commons/bitarray.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <math.h>


// CONSTANTES CONFIG ---> NO VAN
int RETARDO_OPERACION;
int REINTENTO_CONEXION;
int REINTENTO_OPERACION;

t_log* logger; // Mismo nombre entre archivos?
t_config* config;


//SEMAFOROS
pthread_mutex_t chequeo_sem_suscrip;
sem_t suscribir;
// CONSTANT
char* PUNTO_MONTAJE;
char* METADATA_BASE_PATH = "Metadata/";
char* FILES_BASE_PATH = "Files/";
char* BLOCKS_BASE_PATH = "Blocks/";

char* METADATA_FILE_NAME = "Metadata.bin";
char* BITMAP_FILE_NAME = "Bitmap.bin";

// EXTENSIONES
char* EXTENSION = ".bin";

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



// Funciones suscripcion
void suscribirse_a_colas_gamecard();
void suscribirse_a_cola(op_code tipo_mensaje);


void* manejar_recibo_mensajes_broker(void *arg);
void handle_broker(int socket_servidor);
void recibir_mensaje_del_broker(void* input);
bool file_existing(char* path);
bool file_open(char* pokemon);
char** extraer_bloques(char* pokemon);
bool verificar_posciones_file(int x, int y, char** bloques);
void crear_file_si_no_existe(char* file, char* pokemon);
t_appeared_pokemon* convertir_a_appeared_pokemon(t_new_pokemon* pokemon);
void enviar_appeared_pokemon_a_broker(int broker, t_new_pokemon* pokemon);



// Manejo de mensajes

void gamecard_manejar_new_pokemon(t_conexion_buffer * combo);
void gamecard_manejar_catch_pokemon(t_conexion_buffer * combo);
void gamecard_manejar_get_pokemon(t_conexion_buffer * combo);

void handle_mensajes_gamecard(op_code cod_op, int* socket);

void recibir_mensajes_gamecard(int *socket);

#endif /* UTILS_GAMECARD */
