#include "utils.h"

char* serializarOperandos(t_Package *package){

	char *serializedPackage = malloc(package->total_size);

	int offset = 0;
	int size_to_send;

	size_to_send =  sizeof(package->message_long);
	memcpy(serializedPackage + offset, &(package->message_long), size_to_send);
	offset += size_to_send;

	size_to_send =  package->message_long;
	memcpy(serializedPackage + offset, package->message, size_to_send);

	return serializedPackage;
}

void fill_package(t_Package *package){
	/* Me guardo el mensaje que manda */

	fgets(package->message, MAX_MESSAGE_SIZE, stdin);
	(package->message)[strlen(package->message)] = '\0';

	package->message_long = strlen(package->message) + 1;	// Me guardo lugar para el \0

	package->total_size = sizeof(package->message_long) + package->message_long;
	// Si, este ultimo valor es calculable. Pero a fines didacticos la calculo aca y la guardo a futuro, ya que no se modificara en otro lado.
}

void dispose_package(char **package){
	free(*package);
}
