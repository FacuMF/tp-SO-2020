---
# PokÉbola - Apuntes
---
---
## Estructura general del proyecto - comunicaciones
---
nota: cualq cosa hablar con art, fac, fran
---
### MODULO X
---
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

---
### UTILS_CONEXION
---

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


---
### UTILS_MENSAJES
---
//nota: 
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
