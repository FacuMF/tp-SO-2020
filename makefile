COMPILER=gcc
BIN_PATH=bin
LIBS = -lreadline -lcommons -lpthread -lm
UTILS = Base/Utils/src/utils_comunicacion.c Base/Utils/src/utils_config.c Base/Utils/src/utils_logger.c Base/Utils/src/utils.h Base/Utils/src/utils_mensajes.c Base/Utils/src/utils_mensajes.h
CLIENTE = Base/Cliente/src/cliente.h Base/Cliente/src/cliente.c
SERVIDOR = Base/Servidor/src/servidor.h Base/Servidor/src/servidor.c
TEAM = Team/src/team.c Team/src/team.h Team/src/team_planificacion.c Team/src/team_entrenadores.c Team/src/team_objetivo.c
GAMEBOY=GameBoy/src/gameboy.c GameBoy/src/gameboy.h
BROKER = Broker/src/broker.c Broker/src/broker.h
GAMECARD = Gamecard/src/gamecard.c Gamecard/src/gamecard.h Gamecard/src/utils_gamecard.c  
all:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(CLIENTE) -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) $(UTILS) $(SERVIDOR) -o $(BIN_PATH)/Server $(LIBS)
	$(COMPILER) $(UTILS) $(TEAM) -o $(BIN_PATH)/ProcesoTeam $(LIBS)
	$(COMPILER) $(UTILS) $(GAMEBOY) -o $(BIN_PATH)/GameBoy $(LIBS)
	$(COMPILER) $(UTILS) $(BROKER) -o $(BIN_PATH)/Broker $(LIBS)
	#$(COMPILER) $(UTILS) $(GAMECARD) -o $(BIN_PATH)/GameCard $(LIBS)

gamecard:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(GAMECARD) -o $(BIN_PATH)/GameCard $(LIBS)

broker:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(BROKER) -o $(BIN_PATH)/Broker $(LIBS)
	
team:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(TEAM) -o $(BIN_PATH)/Team $(LIBS)

gameboy:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(GAMEBOY) -o $(BIN_PATH)/GameBoy $(LIBS)

clienteservidor:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(CLIENTE) -o $(BIN_PATH)/Cliente 	$(LIBS)
	$(COMPILER) $(UTILS) $(SERVIDOR) -o $(BIN_PATH)/Server $(LIBS)
	

clean:
	rm -rf bin