CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all:
	$(MAKE) -C assembler
	$(MAKE) -C emulator

clean:
	$(MAKE) -C assembler clean
	$(MAKE) -C emulator clean