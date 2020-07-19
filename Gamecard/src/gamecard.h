#ifndef GAMECARD_H
#define GAMECARD_H

#include "../../Base/Utils/src/utils.h"
#include "../../Base/Utils/src/utils_mensajes.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>



// CONSTANTES CONFIG ---> NO VAN
int RETARDO_OPERACION;
int REINTENTO_CONEXION;
int REINTENTO_OPERACION;

// BITMAP
t_bitarray* bitmap_bloques;

t_log* logger; // Mismo nombre entre archivos?
t_config* config;
char* string_nivel_log_minimo;
t_log_level log_nivel_minimo;

// CONSTANT
char* PUNTO_MONTAJE;
char* METADATA_BASE_PATH;
char* FILES_BASE_PATH ;
char* BLOCKS_BASE_PATH;

char* METADATA_FILE_NAME ;
char* BITMAP_FILE_NAME;

// EXTENSIONES
char* EXTENSION;

char* concat_dirs(char* start, char* end);
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
void iniciar_gamecard(char*archivo_config[]);
void reintento_suscripcion_si_aplica_gamecard();

void finalizar_gamecard();

// FUNCIONES DE INICIALIZACION DE GAMECARD
char *obtener_path_gamecard(char *path_leido);
void iniciar_config_gamecard(char* nombre_config);
void iniciar_logger_gamecard();
void iniciar_semaforos_gamecard();
void iniciar_chars_necesarios();


// Files functions

void create_file(char* path);
void create_dir(char* path) ;
void delete_from_path(char* path);
void clean_file(char* path);
void clean_dir(char* path);

char* read_file(char* path, int size);
void write_file(char* path, char* data);

//bool verificar_posiciones_file(int x, int y, char** bloques);



// Crear archivos

t_config* read_pokemon_metadata(char* table);
t_config* read_file_metadata(char* table);
void crear_pokemon_dir(char* tableName);
void crear_pokemon_metadata_file(char* tableName);
void create_new_file_pokemon(char* pokemon);
//


// Funciones COMUNICACION con broker
void suscribirse_a_colas_gamecard();
int iniciar_conexion_broker_gamecard();
void enviar_suscripcion_al_broker(op_code tipo_mensaje);
void enviar_mensaje_suscripcion_gamecard(op_code mensaje, int conexion);
void esperar_mensajes_gamecard(void* input);

//Funciones de comunicacion con gameboy
void iniciar_conexion_con_gameboy_gamecard();
void esperar_cliente_gamecard(int socket_servidor);
void esperar_mensaje_gameboy_gamecard(void* input);


// Manejo de mensajes
int handle_mensajes_gamecard(int conexion, op_code cod_op);
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
