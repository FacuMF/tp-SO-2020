#include "gamecard.h"

int main(int argv, char*archivo_config[]){

	iniciar_gamecard(archivo_config);

	pthread_create(&thread,NULL,(void*) suscribirse_a_colas_gamecard,NULL);

	iniciar_conexion_con_gameboy_gamecard();

	finalizar_gamecard();
}


void iniciar_gamecard(char*argumentos_iniciales[]){
		//Config
		iniciar_config_gamecard(argumentos_iniciales[1]);

		//logger
		iniciar_logger_gamecard();

		iniciar_chars_necesarios();

		iniciar_semaforos_gamecard();

		crear_metadata_bin();

		bitmap_bloques = bitarray_create_with_mode(bitmap_path(),cantidad_bloques()/8,LSB_FIRST);
		for(int i=0; i < cantidad_bloques();i++){
			bitarray_clean_bit(bitmap_bloques,i);
			create_file(block_path(i));
		}

}

void finalizar_gamecard(){
	// TODO : Destroy todos los semaforos
	// TODO : Ver si todos los hilos estan cerrados.
	terminar_logger(logger);
	config_destroy(config);
	pthread_mutex_destroy(&mutex_open_file);
	bitarray_destroy(bitmap_bloques);
}


void crear_metadata_bin(){
	log_trace(logger, "Crear metadata");

	create_dir(PUNTO_MONTAJE);
	create_dir(blocks_base_path());
	create_dir(concat(PUNTO_MONTAJE, FILES_BASE_PATH));
	create_dir(metadata_base_path());
	create_file(bitmap_path());

	char* metadata_bin = concat(metadata_base_path(), METADATA_FILE_NAME);
	create_file( metadata_bin );

	t_config* config_metadata = config_create(metadata_bin);

	config_set_value(config_metadata, "BLOCK_SIZE",
			config_get_string_value(config, "BLOCK_SIZE"));

	config_set_value(config_metadata, "BLOCKS",
				config_get_string_value(config, "BLOCKS"));

	config_set_value(config_metadata, "MAGIC_NUMBER",
					config_get_string_value(config, "MAGIC_NUMBER"));

	config_save(config_metadata);
	config_destroy(config_metadata);
}
