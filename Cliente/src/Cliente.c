

#include "Cliente.h"


int main(){

	 //  Obtiene los datos de la direccion de red y lo guarda en serverInfo.
	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// Permite que la maquina se encargue de verificar si usamos IPv4 o IPv6
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(IP, PUERTO, &hints, &serverInfo);	// Carga en serverInfo los datos de la conexion



	 // 	Obtiene un socket (un file descriptor -todo en linux es un archivo-), utilizando la estructura serverInfo que generamos antes.
	int serverSocket;
	serverSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);


	 // Ahora me conecto!

	connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo);	// No lo necesitamos mas

	printf("Conectado al servidor.\n");

	//Debemos obtener el mensaje que el usuario quiere mandar al chat, y luego serializarlo.
	//Si el mensaje es "exit", saldremos del sistema.

	int enviar = 1;
	t_Package package;
	package.message = malloc(MAX_MESSAGE_SIZE);
	char *serializedPackage;


	printf("Bienvenido al sistema, puede comenzar a escribir. Escriba 'exit' para salir.\n");

	while(enviar){

		fill_package(&package);						// Completamos el package, que contendra los datos del usuario y los datos del mensaje que vamos a enviar.

		if(!strcmp(package.message, "exit\n")) enviar = 0;		// Chequeamos si el usuario quiere salir.

		if(enviar) {
			serializedPackage = serializarOperandos(&package);
			send(serverSocket, serializedPackage, package.total_size, 0);
			dispose_package(&serializedPackage);
		}
	}

	printf("Desconectado.\n");

	/*	NUNCA nos olvidamos de liberar la memoria que pedimos.
	 *
	 *  Acordate que por cada free() que no hacemos, valgrind mata a un gatito.
	 */
	free(package.message);


	close(serverSocket);


	return 0;

}


