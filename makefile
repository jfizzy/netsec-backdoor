CC=g++
SERVER_TARGET=bds9
SERVER_SOURCES=bdc9.cpp BDServer.cpp
SDIR=src/
BDIR=bin/

all: clean server

server:
	@if test -d $(BDIR); then :; else mkdir $(BDIR); fi;
	$(CC) $(OPT) $(addprefix $(SDIR),$(SERVER_SOURCES)) -o $(BDIR)$(SERVER_TARGET)

clean:
	rm -f *.o $(BDIR)*
	rm -f $(SDIR)*~
	rm -f *~
