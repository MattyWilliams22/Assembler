CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble

assemble: assemble.o tokenizer.o symbolTable.o assemblesetup.o assembleinstructions.o
tokenizer.o: tokenizer.h symbolTable.h
symbolTable.o: symbolTable.h
assemblesetup.o: assemblesetup.h
assembleinstructions.o: assembleinstructions.h

clean:
	$(RM) *.o assemble