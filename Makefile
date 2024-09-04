CFLAGS = -Wall -Wextra
CC  =gcc
OBJECTS = src/main.c
LIBRARIES = -lreadline

all:
	$(CC) $(OBJECTS) -o sushi  $(CFLAGS) $(LIBRARIES)

clear:
	rm sushi