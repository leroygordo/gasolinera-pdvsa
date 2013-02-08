CC = gcc

all: bomba centro

bomba: bomba.o estructuras.o
	$(CC) bomba.o estructuras.o -lpthread -g -o bomba

bomba.o: bomba.c
	$(CC) -c bomba.c

centro: centro.o estructuras.o
	$(CC) centro.o estructuras.o -lpthread -g -o centro

centro.o:  centro.c
	$(CC) -c centro.c

estructuras.o: estructuras.c estructuras.h
	$(CC) -c estructuras.c

clean:
	rm centro bomba *.o

centro_clean:
	rm centro centro.o

bomba_clean:
	rm bomba bomba.o


