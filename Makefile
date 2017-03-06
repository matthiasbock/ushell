
CC = colorgcc

CFLAGS += -I ./
CFLAGS += -I ../ucurses/

all: ushell.o

ushell.o: ushell.c ushell.h helper.h ../ucurses/ansi.h
	$(CC) $(CFLAGS) $< -o $@

