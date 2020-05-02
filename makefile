COMPILER=gcc
BIN_PATH=bin
LIBS = -lreadline -lcommons -lpthread
UTILS = Utils/src/utils_comunicacion.c Utils/src/utils_config.c Utils/src/utils_logger.c Utils/src/utils.h Utils/src/utils_mensajes.c Utils/src/utils_mensajes.h
CLIENTE = Cliente/src
SERVIDOR = Servidor/src
all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(CLIENTE)/cliente.h $(CLIENTE)/cliente.c -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) $(UTILS) $(SERVIDOR)/servidor.h $(SERVIDOR)/servidor.c -o $(BIN_PATH)/Server $(LIBS)

clean:
	rm -rf bin