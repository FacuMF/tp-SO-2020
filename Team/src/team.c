#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	inicializar_listas();
	// TT
		 bool prueba =requiero_pokemon("Cualquiera");
		 if (prueba == 1){
			 log_trace(logger, "Entre bien");
		 }else{
			 log_trace(logger,"No requiero");
		 }

	pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);

	pthread_create(&thread, NULL, (void*) enviar_requests_pokemones,NULL);

	pthread_create(&thread, NULL, (void*) iniciar_conexion_con_gameboy,NULL);

	iniciar_planificador();

	sleep(1000); //TT
	finalizar_team();
}

/// Funciones Generales
void iniciar_team(char*argumentos_iniciales[]) {
	// Calcular archivo a abrir
	char * nombre_archivo_config = malloc(sizeof(argumentos_iniciales[1]));
	strcpy(nombre_archivo_config, argumentos_iniciales[1]);
	char *path_config = obtener_path(nombre_archivo_config);


	// Obtener info de config
	config = leer_config(path_config);
	free(nombre_archivo_config);

	// Leer data sobre looger del config
	string_nivel_log_minimo = config_get_string_value(config,
			"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
	char* nombre_archivo_log = config_get_string_value(config, "LOG_FILE");

	char * path_log = obtener_path(nombre_archivo_log);
	logger = iniciar_logger(path_log, "Team", log_nivel_minimo);

	log_trace(logger, "Log inicializado");

}


void inicializar_listas(){
	head_entrenadores = cargar_entrenadores();
	pokemones_por_capturar_global = obtener_pokemones_a_capturar();
	objetivo_global = formar_lista_de_objetivos(pokemones_por_capturar_global);
	ids_mensajes_utiles = list_create();
}


void finalizar_team() {
	terminar_logger(logger);
	config_destroy(config);
}

// Cargar path de config y log
char *obtener_path(char *path_leido) {
	char* path = string_new();
	string_append(&path, "./Team/config/");
	string_append(&path, path_leido);
	return path;
}

