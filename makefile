COMPILER=gcc
BIN_PATH=bin
LIBS = -lreadline -lcommons -lpthread
all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/utilsClient.h Cliente/src/utilsClient.c Cliente/src/cliente.h Cliente/src/cliente.c -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/servidor.h Servidor/src/servidor.c -o $(BIN_PATH)/Server $(LIBS)

clean:
	rm -rf bin

client:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Cliente/src/utilsCliente.h Cliente/src/utilsCliente.c Cliente/src/cliente.h Cliente/src/cliente.c -o $(BIN_PATH)/Cliente $(LIBS)

server:
	mkdir -p $(BIN_PATH)
	$(COMPILER) Servidor/src/utils.h Servidor/src/utils.c Servidor/src/servidor.h Servidor/src/servidor.c -o $(BIN_PATH)/Server $(LIBS)


# Contemplamos algunos Alias para subsanar posibles diferencias de escritura

cliente:
	make client

servidor:
	make server


