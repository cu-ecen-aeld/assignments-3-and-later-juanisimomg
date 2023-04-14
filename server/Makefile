CC = $(CROSS_COMPILE)gcc

all: aesdsocket

writer: aesdsocket.o
	$(CC) aesdsocket.o -o aesdsocket

writer.o: aesdsocket.c
	$(CC) -c aesdsocket.c -o aesdsocket.o
clean:
	rm -f *.o aesdsocket
