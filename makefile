COMPILER=gcc
BIN_PATH=bin

all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/utils.h Cliente/src/utils.c Cliente/src/Cliente.h Cliente/src/Cliente.c -o $(BIN_PATH)/Cliente
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/Server.h Servidor/src/Server.c -o $(BIN_PATH)/Server

clean:
	rm -rf bin

client:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/utils.h Cliente/src/utils.c Cliente/src/Cliente.h Cliente/src/Cliente.c -o $(BIN_PATH)/Cliente

server:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/Server.h Servidor/src/Server.c -o $(BIN_PATH)/Server


# Contemplamos algunos Alias para subsanar posibles diferencias de escritura

cliente:
	make client

servidor:
	make server


# Sistemas Operativos, Universidad Tecnologica Nacional, Facultad Regional Buenos Aires. 2014.
