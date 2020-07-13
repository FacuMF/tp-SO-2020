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

pthread_mutex_t chequeo_sem_suscrip;
sem_t suscribir;

char *concat(char *start, char *end)
{
    return string_from_format("%s%s", start, end);
}

char* concat_dirs(char* start, char* end) {
    return concat(concat(start, end), "/");
}


void set_base_path(char* base) {
    PUNTO_MONTAJE = base;
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

// Manejo de tamaño

int array_length(void** array) {
    int n = 0;
    while (array[n])
    {
        n++;
    }
    return n;
}

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

void create_pokemon_dir(char* tableName) {
    create_dir(files_base_path(tableName));
    t_config* config = read_file_metadata(tableName);
    config_set_value(config, "DIRECTORY", "Y");
    config_save(config);
    config_destroy(config);
}

void create_pokemon_metadata_file(char* tableName){
    create_file(pokemon_metadata_path(tableName));
    t_config* config = read_pokemon_metadata(tableName);
    config_set_value(config, "DIRECTORY", "N");
    config_set_value(config, "SIZE", "0"); // A DEFINIR
    config_set_value(config, "BLOCKS", "[]"); // A DEFINIR
    config_set_value(config, "OPEN", "Y"); // A DEFINIR
    config_save(config);
    config_destroy(config);
}


// Crear archivos

void create_new_file_pokemon(char* pokemon) {
    create_pokemon_dir(pokemon);
    create_pokemon_metadata_file(pokemon);
}



// Funciones generales

int iniciar_conexion_broker() {
	char * ip_broker = config_get_string_value(config, "IP_BROKER");
	char * puerto_broker = config_get_string_value(config, "PUERTO_BROKER");
	return iniciar_conexion(ip_broker, puerto_broker);
}

void suscribirse_a_colas_gamecard() {
	suscribirse_a(NEW_POKEMON);
	suscribirse_a(CATCH_POKEMON);
	suscribirse_a(GET_POKEMON);
	while (1) {
		sleep(config_get_int_value(config, "TIEMPO_DE_REINTENTO_CONEXION"));
		sem_wait(&suscribir);
		log_info(logger, "Inicio reintento de comunicacion con Broker");
		suscribirse_a(NEW_POKEMON);
		suscribirse_a(CATCH_POKEMON);
		suscribirse_a(GET_POKEMON);

		int val_semaforo;
		sem_getvalue(&suscripcion, &val_semaforo);
		if(val_semaforo > 0)
			log_info(logger, "Reintento de comunicación con broker fallido");
		else
			log_info(logger, "Reintento de comunicación con broker logrado");
	}
}

void suscribirse_a(op_code tipo_mensaje) {
	int socket_broker = iniciar_conexion_broker();
	if (socket_broker == -1){
		//reintento_suscripcion_si_aplica();         VER CON NACHO
	}else {
		enviar_mensaje_suscripcion(tipo_mensaje, socket_broker);
		/*											Ya lo tengo en lanzar_hilo_espera_broker?
		int* argument = malloc(sizeof(int));
		*argument = socket_broker;
		pthread_create(&thread, NULL, (void*) esperar_mensajes_cola, argument);
		 */
		log_trace(logger, "Suscripcion completada");
	}
}

void esperar_broker(void *arg) {
	char* PUERTO_BROKER = config_get_string_value(config, "PUERTO_BROKER");
	char* IP_BROKER = config_get_string_value(config, "IP_BROKER");
	int conexion = iniciar_conexion_servidor(ip, puerto); // Diferencia entre iniciar_conexion?
	log_trace(logger, "Aceptando broker...");
	int cod_op = 1;

	while (cod_op > 0) { // No es espera activa porque queda en recv
			cod_op = recibir_codigo_operacion(conexion);
			if (cod_op > 0){
				log_trace(logger, "Mensaje recibido, cod_op: %i.", cod_op);
				handle_mensajes_gamecard(cod_op,&conexion);
			}else{
				log_error(logger, "Error en 'recibir_codigo_operacion'");
				//reintento_conexion_broker(); TODO
				return;
			}
		}

}

void lanzar_hilo_espera_broker() {
	int* argument = malloc(sizeof(int)); // chequear, socket?
	pthread_create(&thread, NULL, (void*) esperar_broker, argument);
	//pthread_detach(thread);	// Si termina el hilo, que sus recursos se liberen automaticamente
}



bool file_existing(char* path){
	FILE * file = fopen(path, "rb");
	if(file == NULL){
		fclose(file);
		return false;
	} else {
	fclose(file);
	return true;
	}
}

bool file_open(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	char* estado = config_get_string_value(config,"OPEN");
	if (estado == "Y"){
		return true;
	} else {
		return false;
	}
}

char** extraer_bloques(char* pokemon){
	t_config* config = read_pokemon_metadata(pokemon);
	return config_get_array_value(config,"BLOCKS");
}


bool verificar_posciones_file(int x, int y, char** bloques){
	//TODO
	return true;
}

void crear_file_si_no_existe(char* file, char* pokemon){
	if(!file_existing(file)){
		create_pokemon_dir(pokemon);
		create_pokemon_metadata_file(pokemon);
	}
}


t_appeared_pokemon* convertir_a_appeared_pokemon(t_new_pokemon* pokemon){ // ver utilidad de esta funcion
	// Conectarse al broker
	t_appeared_pokemon* appeared_pokemon = malloc(sizeof(t_appeared_pokemon));
	appeared_pokemon -> id_mensaje = pokemon->id_mensaje;
	appeared_pokemon -> pokemon = pokemon->pokemon;
	appeared_pokemon -> posx = pokemon->posx;
	appeared_pokemon -> posy = pokemon->posy;
	appeared_pokemon -> size_pokemon = pokemon -> size_pokemon;
	return appeared_pokemon;
}

void enviar_appeared_pokemon_a_broker(int broker, t_new_pokemon* pokemon) {
	// Convertir new_pokemon a appeared_pokemon
	t_appeared_pokemon* appeared_pokemon = convertir_a_appeared_pokemon(pokemon);

	log_trace(logger, "Se va a enviar mensaje APPEARED_POKEMON id: %i.", appeared_pokemon->id_mensaje, broker);
	t_buffer* mensaje_serializado = malloc(sizeof(t_buffer));
	mensaje_serializado = serializar_appeared_pokemon(appeared_pokemon);
	enviar_mensaje(broker, mensaje_serializado, APPEARED_POKEMON);
	log_info(logger, "Envio de APPEARED_POKEMON %i  al broker", appeared_pokemon->id_mensaje, broker);
}



void gamecard_manejar_new_pokemon(t_conexion_buffer * combo){
	t_buffer * buffer = combo->buffer;
	int socket = combo->conexion;
	t_new_pokemon* pokemon = deserializar_new_pokemon(buffer);
	char* file = pokemon_metadata_path(pokemon->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,pokemon->pokemon);
	// Verificar si se puede abrir el archivo
	// mutex lock
	if (!file_open(pokemon->pokemon)){ // SEMAFORO MUTEX PARA MANEJAR LOS OPEN
		//Buscar los bloques del pokemon
		char** bloques = extraer_bloques(pokemon->pokemon);
		if(verificar_posiciones_file(pokemon->posx,pokemon->posy,bloques)){

		}
	}
	// mutex unlock
		// Reintentar la operación luego de REINTENTO_OPERACION
	// Verificar si las posiciones ya existen dentro del archivo
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar el archivo
	// Conectarse al broker y enviar a APPEARED_POKEMON un mensaje con ID del mensaje recibido, pokemon, posicion en el mapa
	//enviar_appeared_pokemon_a_broker(BROKER,pokemon);
	// En caso de no poder conectarse avisar por log
}

void gamecard_manejar_catch_pokemon(t_conexion_buffer * combo){
	t_buffer * buffer = combo->buffer;
	int socket = combo->conexion;
	t_catch_pokemon* pokemon = deserializar_catch_pokemon(buffer);
	char* file = pokemon_metadata_path(pokemon->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,pokemon->pokemon);
	// Verificar si se puede abrir el archivo
	// Verificar si las posiciones existen dentro del archivo
	// En caso de que la cantidad sea 1 -> eliminar la linea, en caso contrario se debe decrementar una unidad
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje indicando ID del mensaje  y resultado a CAUGHT_POKEMON.
	// Si no se puede conectar al broker informar por log y continuar
}

void gamecard_manejar_get_pokemon(t_conexion_buffer * combo){
	t_buffer * buffer = combo->buffer;
	int socket = combo->conexion;
	t_get_pokemon* pokemon = deserializar_get_pokemon(buffer);
	char* file = pokemon_metadata_path(pokemon->pokemon);
	// Verificar si existe pokemon en el Filesystem
	crear_file_si_no_existe(file,pokemon->pokemon);
	// Verificar si se puede abrir el archivo
	// Obtener todas las posiciones y cantidades del pokemon requerido
	// Esperar la cantidad de segundos definidos en config
	sleep(RETARDO_OPERACION);
	// Cerrar archivo
	// Conectarse al broker y enviar el mensaje con todas las posiciones y su cantidad
	// En caso de que se encuentre por lo menos una posicion para el pokemon solicitado se debera enviar un mensaje al broker a LOCALIZED_POKEMON indicando
	// 	ID del mensaje, el pokemon solicitado y la lista de posiciones y cantidad de posiciones X e Y de cada una de ellas
	// Si no se puede conectar al broker informar por log y continuar
}


void handle_mensajes_gamecard(op_code cod_op, int* socket){
	t_buffer * buffer = recibir_mensaje(*socket);
	t_conexion_buffer * info_mensaje_a_manejar = malloc (sizeof(t_conexion_buffer));
	info_mensaje_a_manejar->conexion = *socket;
	info_mensaje_a_manejar->buffer = buffer;

	switch(cod_op){
	case NEW_POKEMON: // deserializarlo aca y que gamecard_manejar_..._pokemon reciba su tipo de pokemon (ej t_new_pokemon)?
		log_trace(logger, "Se recibio un mensaje NEW_POKEMON"); // capaz se repite
		gamecard_manejar_new_pokemon(info_mensaje_a_manejar);


		break;

	case CATCH_POKEMON:
		log_trace(logger, "Se recibio un mensaje CATCH_POKEMON"); // capaz se repite
		gamecard_manejar_catch_pokemon(info_mensaje_a_manejar);

		break;

	case GET_POKEMON:
		log_trace(logger, "Se recibio un mensaje GET_POKEMON"); // capaz se repite
		gamecard_manejar_get_pokemon(info_mensaje_a_manejar);

		break;

	default:
		log_warning(logger, "El cliente %i cerro el socket.", socket);

		break;
	}

}




