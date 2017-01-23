CC=g++
TARGET=bdc9
SOURCES=bdc9.cpp BDServer.cpp
SDIR=src/
BDIR=bin/

all:
	@if test -d $(BDIR); then :; else mkdir $(BDIR); fi;
	$(CC) $(addprefix $(SDIR),$(SOURCES)) -o $(BDIR)$(TARGET)
	rm -f $(SDIR)*~
	rm -f *~
