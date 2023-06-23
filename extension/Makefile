CC      ?= gcc
CFLAGS  ?= -g -Wall

.SUFFIXES: .c .o

.PHONY: all clean

all: snake

snake: snake.o a_star.o bfs.o dfs.o dijkstra.o game_utils.o global.o maze.o path_finding_utils.o
snake.o: snake.h
a_star.o: a_star.h
bfs.o: bfs.h
dfs.o: dfs.h
dijkstra.o: dijkstra.h
game_utils.o: game_utils.h
global.o: global.h
maze.o: maze.h
path_finding_utils.o: path_finding_utils.h

clean:
	$(RM) *.o snake