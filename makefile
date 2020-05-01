COMPILER=gcc
BIN_PATH=bin
LIBS = -lreadline -lcommons -lpthread
UTILS = Utils/src
CLIENTE = Cliente/src
SERVIDOR = Servidor/src
all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS)/utils.h $(UTILS)/utils_comunicacion.c  $(UTILS)/utils_config.c $(UTILS)/utils_logger.c  $(CLIENTE)/cliente.h $(CLIENTE)/cliente.c -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) $(UTILS)/utils.h $(UTILS)/utils_comunicacion.c  $(UTILS)/utils_config.c $(UTILS)/utils_logger.c  $(SERVIDOR)/servidor.h $(SERVIDOR)/servidor.c -o $(BIN_PATH)/Server $(LIBS)

clean:
	rm -rf bin