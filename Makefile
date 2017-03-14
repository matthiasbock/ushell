
CC = colorgcc

CFLAGS += -I ./
CFLAGS += -I ../ucurses/

all: ushell.o

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

