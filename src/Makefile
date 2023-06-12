CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o tokenizer.o symbolTable.o assembleutils.o
tokenizer.o: assembleutils.h tokenizer.h symbolTable.h
symbolTable.o: symbolTable.h assembleutils.h
assembleutils.o: assembleutils.h
emulate: emulate.o

clean:
	$(RM) *.o assemble emulate