#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	head_entrenadores = cargar_entrenadores();

	pokemones_con_repetidos = obtener_pokemones(head_entrenadores);

	objetivo_global = formar_objetivo(pokemones_con_repetidos);

	pokemones_globales_capturados = list_create();

	// To test
	agregar_atrapado_global();
	list_iterate(pokemones_globales_capturados,mostrar_objetivo);
	//suscribirse_a_colas_necesarias();
	//pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);

	//enviar_requests_pokemones(objetivo_global);
	//pthread_create(&thread, NULL, (void*) enviar_requests_pokemones,
			//objetivo_global);

	iniciar_conexion_con_gameboy();

	//lanzar_hilos(head_entrenadores);

	// TT - To Test
	//t_entrenador * entrenador_cercano = hallar_entrenador_mas_cercano(head_entrenadores, 1, 3);

	sleep(10000); //TT
	finalizar_team();
}

// Funcion Para testear
 void agregar_atrapado_global(){
	 t_objetivo * objetivo_test= malloc(sizeof(t_objetivo));
	 	objetivo_test->cantidad=1;
	 	objetivo_test->pokemon = "Pikachu";
	 	list_add(pokemones_globales_capturados,objetivo_test);
 }

// Funciones Generales
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

	log_trace(logger, "--- Log inicializado ---");

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

