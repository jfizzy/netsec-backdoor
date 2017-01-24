CC=g++
SERVER_TARGET=bds9
CLIENT_TARGET=bdc9
SERVER_SOURCES=bdc9.cpp BDServer.cpp
CLIENT_SOURCES=BDClient.cpp
SDIR=src/
BDIR=bin/

all: clean server client

server:
	@if test -d $(BDIR); then :; else mkdir $(BDIR); fi;
	$(CC) $(addprefix $(SDIR),$(SERVER_SOURCES)) -o $(BDIR)$(SERVER_TARGET)

client:
	@if test -d $(BDIR); then :; else mkdir $(BDIR); fi;
	$(CC) $(addprefix $(SDIR),$(CLIENT_SOURCES)) -o $(BDIR)$(CLIENT_TARGET)

clean:
	rm -f *.o $(BDIR)*
	rm -f $(SDIR)*~
	rm -f *~
