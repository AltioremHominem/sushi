CFLAGS = -Wall -Wextra
CC = gcc
OBJECTS = src/main.c

all:
	$(CC) -o sushi $(OBJECTS) $(CFLAGS)

clear:
	rm sushi