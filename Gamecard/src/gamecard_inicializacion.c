#include "gamecard.h"

void iniciar_config_gamecard(char* nombre_config) {
	char *path_config = obtener_path_gamecard(nombre_config);
	config = leer_config(path_config);

}

void iniciar_logger_gamecard(){
	string_nivel_log_minimo = config_get_string_value(config,
					"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
	char* nombre_archivo_log = config_get_string_value(config, "LOG_FILE");

	char * path_log = obtener_path_gamecard(nombre_archivo_log);
	logger = iniciar_logger(path_log, "Gamecard", log_nivel_minimo);

	log_trace(logger, "Log inicializado");
}

char *obtener_path_gamecard(char *path_leido) {
	char* path = string_new();
	string_append(&path, "./Gamecard/config/");
	string_append(&path, path_leido);
	return path;
}
void iniciar_chars_necesarios(){
	// EXTENSION
	EXTENSION = ".bin";

	PUNTO_MONTAJE= concat( config_get_string_value(config,"PUNTO_MONTAJE_TALLGRASS"), "/");
	METADATA_BASE_PATH = "Metadata";
	FILES_BASE_PATH = "Files/";
	BLOCKS_BASE_PATH = "Blocks/";
	METADATA_FILE_NAME = concat("Metadata",EXTENSION);
	BITMAP_FILE_NAME = concat("Bitmap",EXTENSION);


}
void iniciar_semaforos_gamecard(){
	pthread_mutex_init(&mutex_mandar_hilos, NULL);
	pthread_mutex_init(&mutex_bitmap, NULL);
	pthread_mutex_init(&mutex_suscripcion, NULL);
	pthread_mutex_init(&mutex_new_archivo, NULL);
	sem_init(&sem_suscripcion, 1, 0);
	pthread_mutex_init(&mutex_open_file, NULL);

}


// Manejo de archivos

/* FIJAR SI NECESITO ESTAS FUNCIONES
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
*/



// LECTURA DE ARCHIVOS

