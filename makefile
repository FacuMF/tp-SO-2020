COMPILER=gcc
BIN_PATH=bin
LIBS = -lreadline -lcommons -lpthread
UTILS = Base/Utils/src/utils_comunicacion.c Base/Utils/src/utils_config.c Base/Utils/src/utils_logger.c Base/Utils/src/utils.h Base/Utils/src/utils_mensajes.c Base/Utils/src/utils_mensajes.h
CLIENTE = Base/Cliente/src
SERVIDOR = Base/Servidor/src
TEAM = Team/src/team.c Team/src/team.h
GAMEBOY=GameBoy/src/gameboy.c GameBoy/src/gameboy.h
all:
	mkdir -p $(BIN_PATH)
	#$(COMPILER) $(UTILS) $(CLIENTE)/cliente.h $(CLIENTE)/cliente.c -o $(BIN_PATH)/Cliente 	$(LIBS)
	#$(COMPILER) $(UTILS) $(SERVIDOR)/servidor.h $(SERVIDOR)/servidor.c -o $(BIN_PATH)/Server $(LIBS)
	$(COMPILER) $(UTILS) $(TEAM) -o $(BIN_PATH)/Team $(LIBS)
	$(COMPILER) $(UTILS) $(GAMEBOY) -o $(BIN_PATH)/GameBoy $(LIBS)

team:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(TEAM) -o $(BIN_PATH)/Team $(LIBS)

gameboy:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(GAMEBOY) -o $(BIN_PATH)/GameBoy $(LIBS)

clienteservidor:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(CLIENTE)/cliente.h $(CLIENTE)/cliente.c -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) $(UTILS) $(SERVIDOR)/servidor.h $(SERVIDOR)/servidor.c -o $(BIN_PATH)/Server $(LIBS)
	

clean:
	rm -rf bin