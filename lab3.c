//Autores: Javier López - Felipe Sepulveda
/*
• -I filename : especifica el nombre de la imagen de entrada
• -O filename : especifica el nombre de la imagen final resultante del pipeline.
• -M n´umero : especifica el n´umero de filas de la imagen
• -N n´umero : especifica el n´umero de columnas de la imagen
• -h n´umero : especifica el n´umero de hebras.
• -r factor : factor de replicaci´on para Zoom-in
• -b n´umero : especifica el tamano del buffer de la hebra productora.
• -f: bandera que indica si se entregan resultados por consola. En caso 
de que se ingrese este flag debera mostrar las dimensiones de la imagen
 antes y despues de aplicar zoom-in.
//./lab3 -I cameraman_256x256.raw -O imagenSalida.raw -M 256 -N 256 -r 2 -h 4 -b 64 -f
*/
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "funciones.h"

#define LECTURA 0
#define ESCRITURA 1

char *nombreImagen = NULL;
char *imagenSalida = NULL;
int filas, columnas, bandera, factor, opterr, cantHebras, bufferSize,N,m;
float *buffer;
pthread_t * hebras;

//Función para crear un hilo consumidor en la eta del zoom-in
void * zoomINThread(void *params){

}

//Función para crear un hilo consumidor en la eta del zoom-in
void * suavizamoThread(void *params){

}
//Función para crear un hilo consumidor en la eta del zoom-in
void * delineadoThread(void *params){

}

//Función para crear un hilo productor
void * productora (void *params){
    //Leer la imagen de forma secuencial
    N = (filas * columnas * 4);
    buffer = (float *)malloc(sizeof(float) * N);
    leerArchivo(nombreImagen, filas, columnas, buffer, N,bandera);
    float * buffer2 = (float *)malloc(sizeof(float) * N);
    delineado(filas,columnas, buffer , &buffer2,N);
    escribirImagen(imagenSalida, filas, columnas, buffer2,N,bandera);
    //Calcular la cantidad de tamaño para cada hebra
    m = filas/cantHebras; 
    if (bandera != 0)printf("Tamaño cada hebra %d bS : %d \n",m,bufferSize);
    //Creamos cada hebra
    hebras = (pthread_t*)malloc(sizeof(pthread_t)*cantHebras);
    //recoger porcion de la imagen
    //aplicar zoom-in
    for(int i =0; i < cantHebras; i++){
        pthread_create(&hebras[i],NULL,zoomINThread,NULL);
    }
    //pthread_barrier_wait();
    // Esperamos a que todas terminen
    for(int i =0; i < cantHebras; i++){
        pthread_join(hebras[i], NULL);
    }
    //aplicar efecto de suavizado
    //pthread_barrier_wait();
    
    //aplicar efecto de delineado
    //pthread_barrier_wait();
    //Liberar las hebras consumidoras
    free(hebras);
}

int main(int argc, char **argv){
    char c;
    filas, columnas, bandera, factor, opterr, cantHebras, bufferSize,N,m = 0;
    opterr += 1;
    //el siguiente ciclo se utiliza para recibir los parametros de entrada usando getopt
    while ((c = getopt(argc, argv, "I:O:M:N:h:r:b:f:")) != -1)
        switch (c){
        case 'I':
            nombreImagen = optarg;
            break;
        case 'O':
            imagenSalida = optarg;
            break;
        case 'M':
            sscanf(optarg, "%d", &filas);
            break;
        case 'N':
            sscanf(optarg, "%d", &columnas);
            break;
        case 'h':
            sscanf(optarg, "%d", &cantHebras);
            break;
        case 'r':
            sscanf(optarg, "%d", &factor);
            break;
        case 'b':
            sscanf(optarg, "%d", &bufferSize);
            break;
        case 'f':
            bandera = 1;
            break;
        case '?':
            if (optopt == 'f'){
                bandera = 1;
                break;}
            if (optopt == 'c'){
                fprintf(stderr, "Opcion -%c requiere un argumento.\n", optopt);}

            else if (isprint(optopt))
                fprintf(stderr, "Opcion desconocida `-%c'.\n", optopt);
            else
                fprintf(stderr,
                        "Opcion con caracter desconocido `\\x%x'.\n",
                        optopt);
            return 1;

        default:
            abort();
        }
    //notificar fallos en los parametros de entrada
    if (nombreImagen == NULL || imagenSalida == NULL ||
        filas == 0 || columnas == 0 || factor == 0){
        printf("Faltan entradas para poder ejecutar el programa \n");
    }
    else if (columnas != filas){
        printf("Cantidad de filas y columnas es distinto \n");
    }
    else{
        if (bandera != 0)
            printf("Nombre imagen de entrada : %s \n Imagen salida : %s \n  filas : %d \n columnas : %d \n factor : %d \n bandera : %d\n hebras : %d \n bufferTamaño : %d \n",
            nombreImagen, imagenSalida, filas, columnas, factor, bandera,cantHebras,bufferSize);
        //creación de hilos
        pthread_t hebra; //se crea una variable de tipo pthread para al hebra productora
        //primer argumento de pthread_create: &nombre de la variable hebra
        //segundo argumento: atributos de la hebra, para usar los atributos por defecto se le entrega un NULL
        //tercer argumento: funcion definida como void *, que solo puede recibir argumentos void *
        //argumento que recibira la funcion de la hebra, debe castearse a (void *)
        pthread_create(&hebra, NULL, productora,NULL );
        pthread_join(hebra, NULL); 
        //funcion que permite al proceso principal esperar a que termine la ejecucion de la hebra
    }
    return 0;
}

// EJEMPLO DE USO : la
//./lab3 -I cameraman_256x256.raw -O imagenSalida.raw -M 256 -N 256 -r 2 -h 4 -b 64 -f
