CC = gcc
CFLAGS = -Wall -Wextra

all: menu thunder

menu: src/menu.c
	$(CC) $(CFLAGS) -o menu src/menu.c

thunder: src/thunder.c
	$(CC) $(CFLAGS) -o thunder src/thunder.c

clean:
	rm -f menu thunder
