CC = gcc

all: bomba centro

bomba: bomba.o
	$(CC) bomba.o -lpthread -g -o bomba

bomba.o: bomba.c
	$(CC) -c bomba.c

centro: centro.o
	$(CC) centro.o -lpthread -g -o centro

centro.o:  centro.c
	$(CC) -c centro.c

clean:
	rm centro bomba *.o

centro_clean:
	rm centro centro.o

bomba_clean:
	rm bomba bomba.o


