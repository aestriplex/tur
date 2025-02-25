CC = gcc
CFLAGS = -Wall -pedantic -O3 -std=c2x
INCLUDE = /usr/local/include
LIB = -lgit2
OUT = tur

all: *.o
	$(CC) $(CFLAGS) -o $(OUT) $^ $(LIB) -I=$(INCLUDE)

*.o: *.c
	$(CC) $(CFLAGS) -c $^

.PHONY:clean
clean:
	rm -f *.o $(OUT)
