CC=gcc
CFLAGS=-lm
CTHREAD=-pthread
all : lab3 lab3.o	

lab3: lab3.c funciones.c funciones.h
	$(CC) funciones.c lab3.c $(CTHREAD) -o lab3 $(CFLAGS)

clean:
	rm -f *.o all
.PHONY : clean
