COMPILER=gcc
LIBS = -lreadline -lcommons -lpthread -lm
UTILS = Base/Utils/src/utils_comunicacion.c Base/Utils/src/utils_config.c Base/Utils/src/utils_logger.c Base/Utils/src/utils.h Base/Utils/src/utils_mensajes.c Base/Utils/src/utils_mensajes.h
CLIENTE = Base/Cliente/src/cliente.h Base/Cliente/src/cliente.c
SERVIDOR = Base/Servidor/src/servidor.h Base/Servidor/src/servidor.c
TEAM = Team/src/team.c Team/src/team.h Team/src/team_comunicacion_general.c Team/src/team_comunicacion_envio.c Team/src/team_inicializacion.c Team/src/team_entrenador_listas.c Team/src/team_entrenador.c Team/src/team_manejo_mensajes.c Team/src/team_objetivo.c Team/src/team_planificacion.c Team/src/team_deadlock.c
GAMEBOY=GameBoy/src/gameboy.c GameBoy/src/gameboy.h GameBoy/src/gameboy_carga_de_mensajes.c
BROKER = Broker/src/broker.c Broker/src/broker_cache.c Broker/src/broker_appeared.c Broker/src/broker_catch.c Broker/src/broker_caught.c Broker/src/broker_get.c Broker/src/broker_localized.c Broker/src/broker_new.c Broker/src/broker_confirmacion.c Broker/src/broker_subscriptor.c Broker/src/broker.h
GAMECARD = Gamecard/src/gamecard.c Gamecard/src/gamecard.h Gamecard/src/gamecard_comunicaciones.c  Gamecard/src/gamecard_archivos.c  Gamecard/src/gamecard_inicializacion.c 
all:
	$(COMPILER) $(UTILS) $(TEAM) -o team $(LIBS)
	$(COMPILER) $(UTILS) $(GAMEBOY) -o gameboy $(LIBS)
	$(COMPILER) $(UTILS) $(BROKER) -o broker $(LIBS)
	$(COMPILER) $(UTILS) $(GAMECARD) -o gamecard $(LIBS)

gamecard:
	mkdir -p $(BIN_PATH)
	$(COMPILER) $(UTILS) $(GAMECARD) -o gamecard $(LIBS)

broker:
	$(COMPILER) $(UTILS) $(BROKER) -o broker $(LIBS)
	
team:
	$(COMPILER) $(UTILS) $(TEAM) -o team $(LIBS)

gameboy:
	$(COMPILER) $(UTILS) $(GAMEBOY) -o gameboy $(LIBS)
	
gbb: #gameboy y broker

	$(COMPILER) $(UTILS) $(BROKER) -o broker $(LIBS)
	$(COMPILER) $(UTILS) $(GAMEBOY) -o gameboy $(LIBS)
	
	

clean:
	rm -rf