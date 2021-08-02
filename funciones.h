//En  este archivo van las cabeceras de las funciones.
#ifndef funciones_h_
#define funciones_h_

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

//Cabeceras
int leerArchivo(char * pathname , int filas, int columnas,float * buffer,int N,int flag);
void zoomIN(int filas, int columnas,float * buffer , float ** zoom, int factor, int N);
void delineado(int filas, int columnas,float * buffer , float ** delineados, int N);
void suavizado(int filas, int columnas,float * buffer , float ** suavizados, int N);
void printBuffer(int filas, int columnas, float * buffer);
int escribirImagen(char * salidaName , int filas, int columnas,float * buffer,int N,int flag);
int transformarGrados(int grados);


//fin de directivas
#endif // funciones_h_