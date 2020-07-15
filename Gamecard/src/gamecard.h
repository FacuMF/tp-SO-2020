#ifndef GAMECARD_H
#define GAMECARD_H

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"
#include "../../Broker/src/broker.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>



// CONSTANTES CONFIG ---> NO VAN
int RETARDO_OPERACION;
int REINTENTO_CONEXION;
int REINTENTO_OPERACION;

t_log* logger; // Mismo nombre entre archivos?
t_config* config;

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

// SEMAFOROS
pthread_mutex_t mutex_suscripcion;
sem_t sem_suscripcion;

// FUNCIONES GENERALES
void iniciar_gamecard();
void reintento_suscripcion_si_aplica_gamecard();
// Files functions

void create_file(char* path);
void create_dir(char* path) ;
void delete_from_path(char* path);
void clean_file(char* path);
void clean_dir(char* path);

char* read_file(char* path, int size);
void write_file(char* path, char* data);

bool verificar_posiciones_file(int x, int y, char** bloques);



// Crear archivos

t_config* read_pokemon_metadata(char* table);
t_config* read_file_metadata(char* table);
void create_pokemon_dir(char* tableName);
void create_pokemon_metadata_file(char* tableName);
void create_new_file_pokemon(char* pokemon);



// Funciones COMUNICACION
void suscribirse_a_colas_gamecard();
int iniciar_conexion_broker_gamecard();
void enviar_suscripcion_al_broker(op_code tipo_mensaje);
void enviar_mensaje_suscripcion_gamecard(op_code mensaje, int conexion);
void esperar_mensajes_gamecard(void* input);


// Manejo de mensajes
int handle_mensajes_gamecard(int conexion, op_code cod_op,int es_respuesta);
//void manejar_new_pokemon();
//void manejar_catch_pokemon();
//void manejar_get_pokemon();

// Auxiliares para Manejo de mensajes
bool file_existing(char* path);
bool file_open(char* pokemon);
char** extraer_bloques(char* pokemon);
bool verificar_posciones_file(int x, int y, char** bloques);
void crear_file_si_no_existe(char* file, char* pokemon);

#endif /* GAMECARD_H */
