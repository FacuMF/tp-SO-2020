#include "gameboy.h"

void cargar_parametros_appeared_pokemon(char* pokemon, int* pos_x, int* pos_y,
		int* id_mensaje, char** arg, int modulo) {
	string_append(&pokemon, arg[3]);
	*pos_x = atoi(arg[4]);
	*pos_y = atoi(arg[5]);
	if (modulo == broker)
		*id_mensaje = atoi(arg[6]);
	else if (modulo == team)
		*id_mensaje = -1;
}
void cargar_parametros_new_pokemon(char* pokemon, int* pos_x, int* pos_y,
		int* cantidad, int* id_mensaje, char** arg, int modulo) {
	memcpy(pokemon,arg[3], strlen(arg[3]));
	*pos_x = atoi(arg[4]);
	*pos_y = atoi(arg[5]);
	*cantidad = atoi(arg[6]);
	if (modulo == broker)
		*id_mensaje = -1;
	else if (modulo == gamecard)
		*id_mensaje = atoi(arg[7]);
}

void cargar_parametros_caught_pokemon(int* id_mensaje_correlativo, int* ok_fail, char** arg) {
	*id_mensaje_correlativo = atoi(arg[3]);
	*ok_fail = ok_fail_to_int(arg[4]);
}

int ok_fail_to_int(char* str){
	if(string_equals_ignore_case(str,"OK"))
		return 1;
	if(string_equals_ignore_case(str,"FAIL"))
		return 0;
	return -1;

}

void cargar_parametros_catch_pokemon(char* pokemon, int* pos_x, int* pos_y,
		int* id_mensaje, char** arg, int modulo) {
	string_append(&pokemon, arg[3]);
	*pos_x = atoi(arg[4]);
	*pos_y = atoi(arg[5]);
	if (modulo == gamecard)
		*id_mensaje = atoi(arg[6]);
	else if (modulo == broker)
		*id_mensaje = -1;
}

void cargar_parametros_get_pokemon(char* pokemon, int* id_mensaje, char** arg, int modulo) {
	string_append(&pokemon, arg[3]);
	if (modulo == gamecard)
		*id_mensaje = atoi(arg[6]);
	else if (modulo == broker)
		*id_mensaje = -1;
}
void cargar_parametros_suscriptor(int* cola_de_mensajes, int* tiempo_suscripcion, char** arg) {
	*cola_de_mensajes = string_a_tipo_mensaje(arg[2]);
	*tiempo_suscripcion = atoi(arg[3]);
}
