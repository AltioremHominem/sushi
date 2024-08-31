CFLAGS = -Wall -Wextra
CC = gcc
OBJECTS = src/main.c
LIBRARIES = -lreadline

all:
	$(CC) -o sushi $(OBJECTS) $(CFLAGS) $(LIBRARIES)

clear:
	rm sushi