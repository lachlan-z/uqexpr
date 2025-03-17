CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 -g

all: uqexpr

uqexpr: uqexpr.c
	$(CC) -o uqexpr $(CFLAGS) uqexpr.c
