CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=gnu99 -g
INCLUDES = -I/local/courses/csse2310/include
LIBS = -L/local/courses/csse2310/lib -ltinyexpr -lm

all: uqexpr

uqexpr: uqexpr.c
	$(CC) -o uqexpr $(CFLAGS) $(INCLUDES) uqexpr.c $(LIBS)
