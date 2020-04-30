COMPILER=gcc
BIN_PATH=bin
<<<<<<< HEAD
LIBS = -lreadline -lcommons -lpthread
all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/utilsClient.h Cliente/src/utilsClient.c Cliente/src/cliente.h Cliente/src/cliente.c -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/servidor.h Servidor/src/servidor.c -o $(BIN_PATH)/Server $(LIBS)
=======

all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/utils.h Cliente/src/utils.c Cliente/src/Cliente.h Cliente/src/Cliente.c -o $(BIN_PATH)/Cliente
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/Server.h Servidor/src/Server.c -o $(BIN_PATH)/Server
>>>>>>> 4b1a67521de5619bfe22f67e00b7cf62f89ebc38

clean:
	rm -rf bin

client:
	mkdir -p $(BIN_PATH)
<<<<<<< HEAD
	$(COMPILER) Cliente/src/utilsCliente.h Cliente/src/utilsCliente.c Cliente/src/cliente.h Cliente/src/cliente.c -o $(BIN_PATH)/Cliente $(LIBS)

server:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/servidor.h Servidor/src/servidor.c -o $(BIN_PATH)/Server $(LIBS)
=======
	$(COMPILER) Cliente/src/utils.h Cliente/src/utils.c Cliente/src/Cliente.h Cliente/src/Cliente.c -o $(BIN_PATH)/Cliente

server:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/Server.h Servidor/src/Server.c -o $(BIN_PATH)/Server
>>>>>>> 4b1a67521de5619bfe22f67e00b7cf62f89ebc38


# Contemplamos algunos Alias para subsanar posibles diferencias de escritura

cliente:
	make client

servidor:
	make server


<<<<<<< HEAD

=======
# Sistemas Operativos, Universidad Tecnologica Nacional, Facultad Regional Buenos Aires. 2014.
>>>>>>> 4b1a67521de5619bfe22f67e00b7cf62f89ebc38
