#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdint.h>

#define IP "127.0.0.1"
#define PUERTO "6667"
#define MAX_MESSAGE_SIZE 300

typedef struct _t_Package {
	char* message;
	uint32_t message_long;
	uint32_t total_size;			// NOTA: Es calculable. Aca lo tenemos por fines didacticos!
} t_Package;

char* serializarOperandos(t_Package*);
void fill_package(t_Package*);
void dispose_package(char**);

#endif /* UTILS_H_ */
