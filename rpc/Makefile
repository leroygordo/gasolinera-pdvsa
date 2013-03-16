CLIENT = bomba
SERVER = centro

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = programa.x

TARGETS_SVC.c = programa_svc.c programa_server.c programa_xdr.c 
TARGETS_CLNT.c = programa_clnt.c programa_client.c programa_xdr.c 
TARGETS = programa.h programa_xdr.c programa_clnt.c programa_svc.c programa_client.c programa_server.c

OBJECTS_CLNT = $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(TARGETS_SVC.c:%.c=%.o)
# Compiler flags 

CFLAGS += -g 
LDLIBS += -lnsl
RPCGENFLAGS = 

# Targets 

all : $(CLIENT) $(SERVER)

$(OBJECTS_CLNT) : $(TARGETS_CLNT.c) 

$(OBJECTS_SVC) : $(TARGETS_SVC.c)

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS) 

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS) -lpthread

clean:
	$(RM) core $(OBJECTS_CLNT) $(OBJECTS_SVC) $(CLIENT) $(SERVER)

