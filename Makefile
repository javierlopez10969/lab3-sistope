CC=gcc
CFLAGS=-lm
CTHREAD=-pthread
all : lab3 lab3.o lectura lectura.o zoom zoom.o suavizado suavizado.o	

lab3: lab3.c funciones.c funciones.h
	$(CC) funciones.c $(CTHREAD) lab3.c -o lab3 $(CFLAGS)

lectura: lectura.c funciones.c funciones.h
	$(CC) funciones.c lectura.c -o lectura $(CFLAGS)

zoom: zoom.c funciones.c funciones.h
	$(CC) funciones.c zoom.c -o zoom $(CFLAGS)

suavizado: suavizado.c funciones.c funciones.h
	$(CC) funciones.c suavizado.c -o suavizado $(CFLAGS)

clean:
	rm -f *.o all
.PHONY : clean
