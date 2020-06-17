---
# PokÉbola - Apuntes
---
## Proceso team
---
### Hilos
 Inicializar el mutex de este entrenador en 0.
 Cuando el entrenador arranque y tire wait va a pasar a -1 y se bloquea.
 Cuando haya un localizado que el es el mas cercano le tire signal, pasa a 0 y se desbloquea.
 Cuando lo atrapa, vuelve a hacer wait (por el while no haya terminado su objetivo) y se bloquea de nuevo.
 Antes de tirar el signal (o al localizar, filtrar aquellos que su sem esta < 1), chequear si el sem < 0 significa que esta esperando. Si es 0 esta laburando, no me sirve.

### Atender Mensajes
// TD - To Develop - Atender Mensajes
/* Esperar Localized
 *
 * while 1 - o hasta que muera el proceso
 * 		esperar mensaje localized
 * 			cuando llega: lanza hilo atender_localized
 *
 *Atender_localized
 *		hace all the procedimiento para localized
 * */

### Resto
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
## Broker
---
###  Requerimientos

- Administrar Subsciptores
- Espera solicitudes de distintos modulos.
- Lista de subscriptores por cada cola que administra.
- Enviar a nuevos subscriptores, los mensajes cacheados

Administrar recepcion, envio y confirmacion de mensajes:

	- [X]  Recibir mensajes
	- [X]  Analizar a que cola pertenece.
	- [ ]  Identificar unoquibocamente el mensaje (ID)
	- [X]  Almacenar en dicha cola.
	- [ ]  Cachear mensajes
	- [X]  Enviar a todos los subscriptores
	- [ ]  Todo mensaje debe permanecer en la cola hasta que todos sus subs lo reciban
	- [X]  Notificacion de recepcion: Todo mnesaje debe ser confirmado por cada subscriptor, para no volver a enviarlo al mismo.
	- [X]  La recepcion y notificacion de mensajes puede diferir en el tiempo
	- [ ]  Mantener un registro de los ultimos mensajes recibidos para futuros subs
	- [X]  Mantener e informar en todo momento los estados de las colas con sus mensajes y subscriptores.
	- [ ]  Mantener su estado
	- [ ]  Borrar mensajes que fueron entregados a todos los subs.

---	
### Cache

Se debe registrar:
1. ID mensaje
2. Tipo mensaje
3. Subs a los que se le envio
4. Subs que retornaron
	
Dos esquemas de admin de memoria (se elegiran por config):
- Particiones Dinamicas
- Buddy System

Tamanio minimo de particion (config).
Tamanio maximo de particion = Tamanio memoria (config)

	//Un solo malloc con toda la memoria
	stream memoria_cache = malloc ( tamanio_memoria );
		--> datos mensaje
	
	//Memoria dinamica para almacenar los datos necesarios
	t_estructura_cache* info_cache = malloc ( t_estructura_cache ); 
		--> tipo mensaje
		--> id
		--> flags... 

Agregar funciones:

	enviar_a_sub_mensajes_cache(tipo_mensaje, subscriptor);
	subscribir_cache(cola, suscriptor);
	confirmar_recepcion_cache(cola, id_mensaje, suscripcion);
	
---
### Particiones dinamicas con compactacion.
	
- Por cada valor se reserva el tamano exacto.

Procedimiento:
1. Se busca particion con tamanio suficiente.
 - Fist fit
 - Best fit
 x. Si no se encuentra se pasara al paso 2 o al 3.
	
2. Se compatara la memoria.
 x. Paso 1

3. Se elimina una particion de datos. 
 - Fifo 
 - Lru
 x. Paso 1
 
---
### Buddy System

- Por cada valor se reserva la cantidad de memoria mas cercana que sea potencia de 2.

Procedimiento: 

3. Se elimina una particion de datos. 
 - Fifo 
 - Lru
 x. Paso 1
 
---
### Dump de cache

- Se iniciara cuando se recibe la senal SIGUSR1. Tiene que ser inicializada y manejada.

Se quiere ver:
- Particiones asignadas/libres
- Direccion de comienzo y fin
- Tamanio en bytes
- Tiempo en LRU
- Cola de mensaje
- ID mensaje

---
### Pseudocodigo

	// Leo config
	// Funcion enviar mensaje a
	// ETCs

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
