#include "team.h"

int counter;

int main(int argv, char*archivo_config[]) {

	iniciar_team(archivo_config);

	inicializar_listas();

	//pthread_create(&thread, NULL, (void*) suscribirse_a_colas_necesarias, NULL);

	//pthread_create(&thread, NULL, (void*) enviar_requests_pokemones,NULL);

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

	// Leer data sobre logger del config
	string_nivel_log_minimo = config_get_string_value(config,
			"LOG_NIVEL_MINIMO");
	log_nivel_minimo = log_level_from_string(string_nivel_log_minimo);
	char* nombre_archivo_log = config_get_string_value(config, "LOG_FILE");

	char * path_log = obtener_path(nombre_archivo_log);
	logger = iniciar_logger(path_log, "Team", log_nivel_minimo);

	log_trace(logger, "Log inicializado");

	// Obtener variables sobr planificacion
	quantum = config_get_int_value(config, "QUANTUM");
	estimacion_inicial = config_get_int_value(config, "ESTIMACION_INICIAL");
	retardo_ciclo_cpu = config_get_int_value(config, "RETARDO_CICLO_CPU");
	//TODO: Puede ser con coma, testear
	constante_estimacion = config_get_double_value(config, "CONSTANTE_ESTIMACION");
	desalojar = 0;

}


void inicializar_listas(){
	//TODO: en every lugar que se usen las globales usar mutex
	head_entrenadores = cargar_entrenadores();
	ids_mensajes_utiles = list_create();
	appeared_a_asignar = list_create();
	appeared_auxiliares = list_create();
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

