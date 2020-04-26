

#include "Server.h"

int main(){

	//  Obtiene los datos de la direccion de red y lo guarda en serverInfo.

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;		// No importa si uso IPv4 o IPv6
	hints.ai_flags = AI_PASSIVE;		// Asigna el address del localhost: 127.0.0.1
	hints.ai_socktype = SOCK_STREAM;	// Indica que usaremos el protocolo TCP

	getaddrinfo(NULL, PUERTO, &hints, &serverInfo); // Notar que le pasamos NULL como IP, ya que le indicamos que use localhost en AI_PASSIVE


	// 	Mediante socket(), obtengo el File Descriptor que me proporciona el sistema (un integer identificador).

	// Necesitamos un socket que escuche las conecciones entrantes */
	int listenningSocket;
	listenningSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);


	 // Necesito decirle al sistema que voy a utilizar el archivo que me proporciono para escuchar las conexiones por un puerto especifico.
	 // Todavia no estoy escuchando las conexiones entrantes!

	bind(listenningSocket,serverInfo->ai_addr, serverInfo->ai_addrlen);
	freeaddrinfo(serverInfo); // Ya no lo vamos a necesitar



	 // 	Solo me queda decirle que vaya y escuche!

	listen(listenningSocket, BACKLOG);		// IMPORTANTE: listen() es una syscall BLOQUEANTE.



	//Aceptamos la conexion entrante, y creamos un nuevo socket mediante el cual nos podamos comunicar (que no es mas que un archivo).

	//¿Por que crear un nuevo socket? Porque el anterior lo necesitamos para escuchar las conexiones entrantes. Un socket no se puede encargar de escuchar
	//las conexiones entrantes y ademas comunicarse con un cliente.

	 //	Nota: Para que el listenningSocket vuelva a esperar conexiones, necesitariamos volver a decirle que escuche, con listen();
	 //	En este ejemplo nos dedicamos unicamente a trabajar con el cliente y no escuchamos mas conexiones.

	struct sockaddr_in addr;			// Esta estructura contendra los datos de la conexion del cliente. IP, puerto, etc.
	socklen_t addrlen = sizeof(addr);

	int socketCliente = accept(listenningSocket, (struct sockaddr *) &addr, &addrlen);


	//	Ya estamos listos para recibir paquetes de nuestro cliente...

	// 	Vamos a ESPERAR (ergo, funcion bloqueante) que nos manden los paquetes, y luego imprimiremos el mensaje por pantalla.

	//	Cuando el cliente cierra la conexion, recieve_and_deserialize() devolvera 0.

	t_Package package;
	int status = 1;		// Estructura que manjea el status de los recieve.

	printf("Cliente conectado. Esperando Envío de mensajes.\n");

	while (status){
		status = recieve_and_deserialize(&package, socketCliente);
		if (status) printf("Mensaje recibido: %s", package.message);
	}


	printf("Cliente Desconectado.\n");

	close(socketCliente);
	close(listenningSocket);

	return 0;
}




