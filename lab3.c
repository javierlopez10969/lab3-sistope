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


//variables que se utilizaran para almacenar datos
char *nombreImagen = NULL;
char *imagenSalida = NULL;
int filas, columnas, bandera, factor, opterr, cantHebras, bufferSize,N,m;
float *buffer,*bufferZoom, *bufferSuavizado,*bufferDelineado;

//variable del tipo pthread que representa una hebra
pthread_t * hebras;
//Mutex puntero para control el flujo de las hebras
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


/* ### Funciones necesarias para el uso de Hebras (Thread) ### */

//Entradas: recibe un dato del tipo void*, debido a que pthread solo permite este tipo de datos.
//Funcionamiento: Se utiliza para crear un hilo consumidor en la etapa del Zoom-In
void * zoomINThread(void *params){
    pthread_mutex_lock(&mutex);
    int x;
	x = *((int *)params);//se castea el argumento de void* a int
    if (bandera != 0){
	    printf("Soy una hebra y recibi el argumento: %d \n", x);
    }
    zoomIN(filas,columnas,buffer,&bufferZoom,factor,N);
    pthread_mutex_unlock(&mutex);
}

//Entradas: recibe un dato del tipo void*, debido a que pthread solo permite este tipo de datos.
//Funcionamiento: Se utiliza para crear un hilo consumidor en la etapa Suavizado
void * suavizadoThread(void *params){
    pthread_mutex_lock(&mutex);
    int x;
	x = *((int *)params);//se castea el argumento de void* a int
    if (bandera != 0){
	    printf("Soy una hebra y recibi el argumento: %d \n", x);
    }
    suavizado(filas*factor,columnas*factor, bufferZoom , &bufferSuavizado,N*factor*factor);
    pthread_mutex_unlock(&mutex);
}

//Entradas: recibe un dato del tipo void*, debido a que pthread solo permite este tipo de datos.
//Funcionamiento: Se utiliza para crear un hilo consumidor en la etapa Delineado
void * delineadoThread(void *params){
    pthread_mutex_lock(&mutex);
    int x;
	x = *((int *)params);//se castea el argumento de void* a int
    if (bandera != 0){
	    printf("Soy una hebra y recibi el argumento: %d \n", x);
    }

    delineado(filas*factor,columnas*factor, bufferSuavizado , &bufferDelineado,N*factor*factor);
    pthread_mutex_unlock(&mutex);
}

//Entradas: recibe un dato del tipo void*, debido a que pthread solo permite este tipo de datos.
//Funcionamiento: Se utiliza para crear un hilo consumidor que escribe una imagen
void * escrituraThread(void *params){
    pthread_mutex_lock(&mutex);
    int x;
	x = *((int *)params);//se castea el argumento de void* a int
    if (bandera != 0){
	    printf("Soy una hebra de escritura y recibi el argumento: %d \n", x);
    }
    escribirImagen(imagenSalida, filas*factor, columnas*factor, bufferDelineado, N*factor*factor, bandera);
    pthread_mutex_unlock(&mutex);
}

//Entradas: recibe un dato del tipo void*, debido a que pthread solo permite este tipo de datos.
//Funcionamiento: Se utiliza para crear un hilo productor
void * productora (void *params){
    //Leer la imagen de forma secuencial
    N = (filas * columnas * 4);
    buffer = (float *)malloc(sizeof(float) * N);
    leerArchivo(nombreImagen, filas, columnas, buffer, N,bandera);
    //Calcular la cantidad de tamaño para cada hebra
    m = filas/cantHebras; 
    if (bandera != 0){
        printf("Tamaño cada hebra %d bS : %d \n",m,bufferSize);
        printf("Cantidad de hebras %d \n",cantHebras);
    }
    //Creamos cada hebra
    hebras = (pthread_t*)malloc(sizeof(pthread_t)*cantHebras);
    //recoger porcion de la imagen
    //Aplicar ZOOM-IN
    bufferZoom = NULL;
    int *argumento = (int *)malloc(sizeof(int));
    *argumento = 0;
    pthread_t hebraZoom; 
    pthread_create(&hebraZoom , NULL, zoomINThread,(void *)argumento );
    pthread_join(hebraZoom, NULL); 
    /*
    for(int i =0; i < cantHebras; i++){
        pthread_mutex_lock(&mutex);
        pthread_create(&hebras[i],NULL,zoomINThread,(void *)argumento);
        pthread_mutex_unlock(&mutex);
        *argumento += m;
    }
    // Esperamos a que todas terminen
    for(int i =0; i < cantHebras; i++){
        pthread_join(hebras[i], NULL);
    }
    */

    //Aplicar efecto de suavizado
    //pthread_barrier_wait();
    *argumento = 200;
    bufferSuavizado = NULL;
    pthread_t hebraSuavizado; 
    pthread_create(&hebraSuavizado , NULL, suavizadoThread,(void *)argumento );
    pthread_join(hebraSuavizado, NULL);

    //aplicar efecto de delineado
    //pthread_barrier_wait();
    bufferDelineado = NULL;
    *argumento = 115;
    pthread_t hebraDelineado; 
    pthread_create(&hebraDelineado , NULL, delineadoThread,(void *)argumento );
    pthread_join(hebraDelineado, NULL); 
    //Aplicar escritura de la imagen
    *argumento = 300;
    pthread_t hebraEscritura; 
    pthread_create(&hebraEscritura , NULL, escrituraThread ,(void *)argumento );
    pthread_join(hebraEscritura, NULL); 
    //Liberar las hebras consumidoras
    free(hebras);
}


// ### SECCIÓN DE MAIN ###
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
