---
# PokÉbola - Apuntes
---
## Proceso team
---
// Suscribirse a la msj queue. Puede funcar sin broker.
		// Proceso team se conecta al broker. un connect por cada cola de mensajes.
		// Si el broker no existe tiene que funcar igual
			// Reintentar cada x tiempo

	//Enviar mensajes GET al broker
		// Uno por cada pokemon del objetivo
			// Por cada mensaje necesito un connect -- diferente de los de suscripciones.
			// Luego de enviar el mensaje, muere.

	//Conexion con gameboy
		//Socket escucha para appeard, caught y localized.
			// Por cada mensaje se recibe un accept.
			// Para testeo


	/* ESTADOS */
	// Multiprogramacion grado 1:solo un entrenador en estado exec simultaneamente
	// Cada entrenador se crea en estado new

	/* Ante mensaje "LOCALIZED"
		//Verifico si ya tengo uno localizado de esa especie
			//Si ya tengo uno de esa especie lo descarto
		// Verifico si lo necesito
			//Si no lo necesito lo descarto
			//Si lo necesito lo guardo en el mapa
			//Planifico al entrenador mas cercano (pasar a ready) -- Mas cercano => ((Xe - Xp)2+(Ye - Yp)2)1/2 = d
				// El que esta en estado NEW o BLOQUEADO (si esta bloq porque no tiene nada para hacer)
				// Mover entrenador a la posicion del pokemon -- gastar "un ciclo de cpu"
				// Connect al broker y mandar un catch squirtle 4 4
				// Recibir ID del msj que envié
				// Cerrar la conexión.

	*/
	/* Ante mensaje "CAUGHT"
		// Verifico si es respuesta a alguno de los enviados previamente (si tengo el ID en mi lista)
			//si no, lo descarto
		// Lo saco del mapa (de la lista que representa el mapa)
		// Marco que el entrenador atrapó al pokemon
		// Verifico si tiene todos los pokemones que necesitaba
			//Si es asi, lo paso a EXIT
		//Verifico si tiene la cantidad maxima atrapada (= suma total de los que necesita)
			//Si es asi queda BLOQUEADO esperando el deadlock
		// Verifico si hay pokemons para atrapar (mensajes caught)
			//Si es asi, lo vuelvo a planificar.
		// Si no pasa nada de lo anterior
			// Paso a BLOQUEADO esperando el siguiente
	*/



---
## Estructura general del proyecto - comunicaciones
---
nota: cualq cosa hablar con art, fac, fran

### MODULO X

	stream* bufffer = serialziar_mensajeX(struct mensaje);
	enviar_mensaje(buffer, cod_op, socket)

	recibir_mensaje(socket, &buffer, &cod_op);

	struct_mensajeX mensaje = deserializar_mensajeX(buffer)
	
	switch(cod_op){
		case 1:	
			mensajeA mensaje = deserializar(buffer, cod_op);
			operacion_con_mensaje_A(mensaje);
		case 2:
			mensajeA mensaje = deserializar(buffer, cod_op);
			operacion_con_mensaje_A(mensaje);
	}

### UTILS_CONEXION

	enviar_mensaje(){
		paq_ser = serializar paquete(buffer,cod_op);
		env(socket,paquete,tamaño_paq,0);
	}


	void reciba_mensaje(socket, &buffer, &cod_op){
		...deserializar...
		return;
	}

	t_paquete* serializar_paquete(buffer, cod_op){
		...
		return paquete
	}

### UTILS_MENSAJES
	
	enum {
		MENSAJEX = 0,
		MENSAJEY = 1
		...
	} cod_op

	deserializar(buffer, cod_op){
	}

	serializar_mensajeX(struct mensajeX){
		t_buffer buffer = malloc(sizeof(buffer));
		buffer->stream...
		...
		return buffer;		

	}

	serializar_mensajeY(struct mensajeY){
		t_buffer buffer = malloc(sizeof(buffer));
		buffer->stream...
		...
		return buffer;		
	}
---
## OTROS

---
### Inicializacion de proceso team
---
Recordar que al inicializar el proceso team es necesario que pasen como parametro el archivo de config que corresponda a dicho team

---
### Manejo de makefiles
---
El servidor y el cliente compilan los dos haciendo:

    make all

o tambien haciendo solo:

    make

Los dos ejecutables quedan adentro de la carpeta bin.

Despues para ejectar, desde dos consolas distintas haces:

    ./bin/Server

    ./bin/Cliente

Listo.

Si hacen cambios, antes de compilar tienen que hacer:

    make clean

Y volver al paso 1 xd 

saludos.
