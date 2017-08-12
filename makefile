IDIR = ./include
CC=gcc
CFLAGS=-I$(IDIR)

make:
	$(CC) $(CFLAGS) src/*.c -o test && ./test
