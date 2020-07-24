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
void* bitmap_memory;
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
char* concat(char *start, char *end);
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
char* pokemon_metadata_path(char* pokemon);

// SEMAFOROS
pthread_mutex_t mutex_bitmap;
pthread_mutex_t mutex_suscripcion;
sem_t sem_suscripcion;
pthread_mutex_t mutex_open_file;

//STRUCT PARA HILOS


typedef struct{
	int cod_op;
	int id_mensaje;
	t_buffer* buffer;
	int conexion;
}t_manejar_mensajes_gamecard;

// FUNCIONES GENERALES
void iniciar_gamecard(char*archivo_config[]);
void reintento_suscripcion_si_aplica_gamecard();
void finalizar_gamecard();
void crear_metadata_bin();

// FUNCIONES DE INICIALIZACION DE GAMECARD
char *obtener_path_gamecard(char *path_leido);
void iniciar_config_gamecard(char* nombre_config);
void iniciar_logger_gamecard();
void iniciar_semaforos_gamecard();
void iniciar_chars_necesarios();


// Files functions


void delete_from_path(char* path);
void clean_file(char* path);
void clean_dir(char* path);

char* read_file(char* path, int size);
void write_file(char* path, char* data);




// FUNCIONES DE GAME CARD ARCHIVOS

//Lectura de archivos
t_config* read_pokemon_metadata(char* table);
t_config* read_file_metadata(char* table);
char** extraer_bloques(char* pokemon);
char* extraer_bloques_string(char* pokemon);
bool verificar_posiciones_file(char* posicion, char** bloques);
char* leer_sentencia(char* fileName);
void intentar_abrir_archivo(char* pokemon);

// Crear archivos
void create_file(char* path);
void create_dir(char* path);
void crear_pokemon_dir(char* tableName);
void crear_pokemon_metadata_file(char* tableName);
void create_new_file_pokemon(char* pokemon);
bool file_existing(char* path);
void crear_file_si_no_existe(t_new_pokemon* mensaje_new);
void delete_from_path(char* path);
void limpiar_file(char* path);

// TAMANIO DE ARCHIVOS Y SUS ATRIBUTOS
int tamanio_bloque();
int cantidad_bloques();
int size_bytes(char* data);
bool sentencia_sobrepasa_tamanio_maximo(int posx, int posy, int cantidad);
int tamanio_archivo(char* path);
int tamanio_todos_los_bloques(char** bloques);

//  MANEJO DE STRINGS PARA ARCHIVOS
char* concatenar_posicion(int posx, int posy);
char* separar_posicion(char* palabra);
t_posicion* de_char_a_posicion(char* string_posicion);

// Manejo de asignacion de bloques
void asignar_bloque(t_new_pokemon* mensaje_new, int posicion_existente);
void asignar_bloque_vacio(t_new_pokemon* mensaje_new, int contador, int posicion_existente);
int encontrar_bloque_con_posicion(char* posicion, char** bloques);
void agregar_bloque_metadata(char* pokemon, int bloque_nuevo);
void actualizar_size_metadata(char* pokemon);

// Funciones de handle localized
t_localized_pokemon* obtener_pos_y_cant_localized(t_get_pokemon* mensaje_get);

// Funciones COMUNICACION con broker
void suscribirse_a_colas_gamecard();
int iniciar_conexion_broker_gamecard();
void enviar_suscripcion_al_broker(op_code tipo_mensaje);
void enviar_mensaje_suscripcion_gamecard(op_code mensaje, int conexion);
void esperar_mensajes_gamecard(void* input);
void enviar_caught_pokemon_a_broker( t_caught_pokemon* element);
void enviar_localized_pokemon_a_broker( t_localized_pokemon* element);
void enviar_appeared_pokemon_a_broker( t_appeared_pokemon* element);
void manejar_recibo_respuesta(int socket_broker, int cod_op);

//Funciones de comunicacion con gameboy
void iniciar_conexion_con_gameboy_gamecard();
void esperar_cliente_gamecard(int socket_servidor);
void esperar_mensaje_gameboy_gamecard(void* input);


// Manejo de mensajes
void handle_mensajes_gamecard(int conexion, op_code cod_op);
void manejar_new_pokemon(t_new_pokemon *mensaje_new);
void manejar_catch_pokemon(t_catch_pokemon * mensaje_catch);
void manejar_get_pokemon(t_get_pokemon * mensaje_get);
void manejar_mensajes_gamecard(t_manejar_mensajes_gamecard* argumentos);

// Auxiliares para Manejo de mensajes
void agregar_posicion(t_new_pokemon * mensaje_new, char** bloques);
bool informar_error_no_existe_pos_catch(t_catch_pokemon* mensaje_catch);
bool abrir_archivo(char* pokemon);
bool informar_error_no_existe_pokemon_catch(t_catch_pokemon* mensaje_catch);
bool informar_no_existe_pokemon_get(t_get_pokemon* mensaje_get);
t_appeared_pokemon * de_new_a_appeared(t_new_pokemon * mensaje_new);
void sacar_bloque_de_metadata(char* pokemon,int bloque_con_posicion);
void vaciar_bloque_bitmap(int bloque);
void save_bitmap();
void cerrar_archivo_pokemon(char* pokemon);
void sumar_unidad_posicion(t_new_pokemon* mensaje_pokemon,char** bloques);
void restar_uno_pos_catch();
void compactar_bloques(char** bloques, char* pokemon);
void copactar_bloques_si_corresponde(int bloque_que_comparo, int bloque_contra_el_que_comparo,char** bloques, char* pokemon);
void manejar_bloques_pokemon(t_new_pokemon * mensaje_new);
int cantidad_bytes_de_mas(char* sentencia1, char* sentencia2);


#endif /* GAMECARD_H */
