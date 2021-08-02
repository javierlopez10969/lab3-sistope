#include <stdio.h>
#include <stdlib.h>
#include <time.h> 
#include <pthread.h>
#include "estructuras.h"
#include "funciones.h"

//Entradas: recibe el nombre de la imagen a leer (char puntero), el numero de filas, columnas, un buffer para almacenar el contenido de la imagen,
//y la cantidad de bytes necesarios para la imagen.
//Funcionamiento: abre la imagen y comprueba si esto se hizo correctamente, en caso contrario se muestra un error en pantalla, poosteriormente, se lee el contenido de la imagen y
//si la cantidad de bits difiere,significa que hubo un error en la entrega de filas y columnas, se cierra el archivo y termina la funcion
//Salidas: un entero que muestra el termino de la funcion.
int leerArchivo(char * pathname , int filas, int columnas,float * buffer,int N,int flag){
    int f1;
    if ((f1 = open(pathname,O_RDONLY)) == -1) {
        printf("Error al abrir archivo\n");
        exit(-1);
    }

    int nbytes;
    if ((nbytes = read(f1, buffer, N)) != N) {
        printf("Tamaño incorrercto de filas y columnas\n");
        exit(-1);
    }
    close(f1);
    return 1;
}

//Entradas: Numero de filas, columnas y un buffer con el contenido de la imagen
//Funcionamiento: recorre el buffer lineal de tamaño "columnas * filas", para posteriormente mostrar el contenido de este separandolo por filas
void printBuffer(int filas, int columnas, float * buffer){
    int i = 1;
    int k = 0;
    for(int j = 0 ; j < columnas *filas; j++){
      printf("%f, ",buffer[j]);
      k++;
      if(k == columnas){
          printf("\n\n fila %d \n\n", i);
          k = 0;
          i++;
      }
    }
}

//Entradas: se indica el nombre de salida para la imagen, numero de filas y columnas, un buffer con el contenido de la imagen y la cantidad de bytes necesarias para esta.
//Funcionamiento: se abre la imagen para que esta posteriormente pueda ser escrita, en caso de que existe un error este se especifica y se muestra en pantalla.
//Salidas: retorna un entero el cual indica que la funcion termino.
int escribirImagen(char * salidaName , int filas, int columnas,float * buffer,int N,int flag){

    int f2;
    if ((f2 = open(salidaName, O_WRONLY | O_CREAT , 0644)) == -1) {
        printf("Error al abrir archivo\n");
        exit(-1);
    }
    
    else if (f2  == 1) {
        printf("Se abrio correctamente\n");
        exit(-1);
    }

    if(write(f2, buffer, N) != -1){
        if (flag==1)        
            printf("Se ha escrito corectamente el archivo : %s\n" , salidaName);
    } 
    close(f2);
    
    return 1;

}

//Entradas: cantidad de filas, 
//cantidad de columnas, 
//buffer con los datos de la imagen,
//factor en el que se debehacer el zoom, N (cantidad de bytes)= Filas x Columnas x 4
//Funcionamiento: Realiza el proceso de zoomIn a la imagen de entrada
//Salidas: Nuevo buffer donde su contenido corresponde a la imagen posterior al proceso de zoom in.
void zoomIN(int filas, int columnas,float * buffer , float ** zoom,int factor, int N){
    //(filas * factor) * (columnas * factor) * 4 = (N) * factor * factor
    float * newBuffer = (float*)malloc(sizeof(float)*N*factor);
    float elemento  = buffer[0];
    //Iterador que recorre el nuevo buffer
    int k = 0;
    int i = 0 ;
    int j = 0;
    int l = 0;
    //Hacer las primeras N*factor columnas   * M filas
    while (i < filas*columnas){ 
        elemento  = buffer[i];
        //printf("buffer[%d] : %f \n",i ,elemento);
        //printf("k:%d, ",k);
        j = 0;
        while (j < factor){
            //aumentar por los lados y por abajo
            //Aumentamos por los lados
            newBuffer [k] = elemento;
            //printf("[%d] : ", k);
            //printf("%f ,",  newBuffer[k]);
            k++; 
            j++;         
        }
        if(i!=0){
            if(k%(columnas*factor) ==0){
                //printf("\n\n fila %d \n\n", l);
                l++;
            }
        }
        i++; 
    }
    i = 0;
    j = 0;
    k = 0;
    l=0;
    int z  = 0;
    float * bufferFinal = (float*)malloc(sizeof(float)*N*factor*factor);
    while (i < columnas*filas*factor*factor){
        j=0;
        while (j < factor){
            l = 0;
            k = z;
            while(l < columnas*factor){
                bufferFinal[i] = newBuffer[k]; 
                //printf("i : %d,",i);
                //printf("k : %d, \n",k);
                l++;
                i++;
                k++;
            }
            j++;
        }
        z = k;

    }
    *zoom = bufferFinal; 
}

//Entradas: Numero de filas y columnas, el buffer original con el contenido de la imagen, un buffer el cual se encuentra vacio (NULL);
//y N que corresponde a la cantidad de bytes (filas * columnas * 4)
//Funcionamiento: realiza el proceso de suavizado, el cual consiste en revisa cada posicion posible para los bytes,
//dependiendo de esta calcula la media aritmetica de su vecindario y añade esto a un nuevo buffer del mismo tamaño.
//Salidas: Nuevo buffer el cual corresponde a la imagen posterior a su proceso de suavizado.
void suavizado(int filas, int columnas,float * buffer , float ** suavizados, int N){
    float * newBuffer = (float*)malloc(sizeof(float)*(columnas*filas*4));
    //Iterador que recorre el nuevo buffer
    int i = 0 ;
    float promedio = 0 ;
    while (i < filas*columnas){
        //printf(" %d -", i);
        //Si es el primer valor primera esquina superior izquierda
        if (i == 0){
            promedio = (buffer[i+1] + buffer[i+columnas] + buffer[i+columnas+1])/3;
            newBuffer [i] = promedio;
        }
        //Si el iterador es la segunda esquina, esquina superior derecha
        else if(i == columnas -1){
            promedio = (buffer[i-1] + buffer[i+ columnas] + buffer[i+ columnas -1])/3;
            newBuffer [i] = promedio;
        }
        //esquina inferior izquerda de la imagen
        else if(i == (columnas * filas) - columnas){
            promedio = (buffer[i+1] + buffer[i-columnas] + buffer[i-columnas+1]) / 3;
            newBuffer [i] = promedio;
        }
        //esquina inferior derecha de la imagen
        else if(i == ((columnas * filas) - 1)){
            promedio = (buffer[i-1] + buffer[i-columnas] + buffer[i-columnas-1]) / 3;
            newBuffer [i] = promedio;
            
        }
        //Caso orilla superior
        else if (i != 0 && i < columnas-1){
            promedio = (buffer[i-1] +buffer[i+1] + buffer[i+columnas] + buffer[i+columnas-1] + buffer[i+columnas+1] ) / 5;
            newBuffer [i] = promedio;
        }
        //Caso orilla izquierda
        else if (i != 0 && i != (columnas * filas) - columnas && i%columnas == 0){
            promedio = (buffer[i+1] + buffer[i-columnas] + buffer[i-columnas+1] + buffer[i+columnas] + buffer[i+columnas+1]) / 5;
            newBuffer [i] = promedio;
        }
        //Caso orilla derecha
        else if(i != (columnas * filas)-1 && i != columnas -1 && i%(columnas-1) == 0 ){
            promedio = (buffer[i-1] + buffer[i-columnas] + buffer[i-columnas-1] + buffer[i+columnas] + buffer[i+columnas-1]) / 5;
            newBuffer [i] = promedio;
        }
        //Caso orilla inferior
        else if(i > (columnas * filas) - columnas && i < (columnas * filas) - 1 ){
            promedio = (buffer[i-1] + buffer[i+1] + buffer[i-columnas] + buffer[i-columnas-1] + buffer[i-columnas+1] ) / 5;
            newBuffer [i] = promedio;
        }
        //Caso de al medio con 6 vecinos.
        else{
            promedio = (buffer[i+1] + buffer[i-1] + buffer[i-columnas] + buffer[i-columnas-1] + buffer[i-columnas+1] + buffer[i+columnas] + buffer[i+columnas-1] + buffer[i+columnas + 1]) / 6 ; 
            newBuffer [i] = promedio;
        }
        //Si es 
        i++;
    }
    *suavizados = newBuffer;
}


//Entradas: Numero de filas y columnas, el buffer original con el contenido de la imagen, un buffer el cual se encuentra vacio (NULL);
//y N que corresponde a la cantidad de bytes (filas * columnas * 4).
//Funcionamiento: realiza el proceso de Delineado, el cual consiste en que cada posicion del vecindario (incluido el centro) debe cambiar su valor por uno pre-establecido en la mascara laplaciana
//la adicion de estos nuevos valores corresponde al nuevo valor que tendra el pixel en esa misma posicion
//Salidas: Nuevo buffer el cual corresponde a la imagen posterior a su proceso de delineado.
void delineado(int filas, int columnas,float * buffer , float ** delineados, int N){
    float * newBuffer = (float*)malloc(sizeof(float)*(columnas*filas*4));
    //Iterador que recorre el nuevo buffer
    int i = 0 ;
    float posPrima = 0;
    while (i < filas*columnas){
        //printf(" %d -", i);
        //utiliza la misma estrategia que en suavizado para encontrar a los vecinos de la posicion actual
        //la posición de cada caso siempre se considerara como el centro de la mascara
        //Si es el primer valor primera esquina superior izquierda
        if (i == 0){
            posPrima = (buffer[i+1] * -1 + buffer[i+columnas] * -1 + buffer[i+columnas+1] * -1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //Si el iterador es la segunda esquina, esquina superior derecha
        else if(i == columnas -1){
            posPrima = (buffer[i-1] *-1 + buffer[i+ columnas] *-1 + buffer[i+ columnas -1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //esquina inferior izquerda de la imagen
        else if(i == (columnas * filas) - columnas){
            posPrima = (buffer[i+1] *-1 + buffer[i-columnas] * -1 + buffer[i-columnas+1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //esquina inferior derecha de la imagen
        else if(i == ((columnas * filas) - 1)){
            posPrima = (buffer[i-1] *-1 + buffer[i-columnas] *-1 + buffer[i-columnas-1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
            
        }
        //Caso orilla superior
        else if (i != 0 && i < columnas-1){
            posPrima = (buffer[i-1] *-1 + buffer[i+1]*-1 + buffer[i+columnas]*-1 + buffer[i+columnas-1] *-1 + buffer[i+columnas+1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //Caso orilla izquierda
        else if (i != 0 && i != (columnas * filas) - columnas && i%columnas == 0){
            posPrima = (buffer[i+1] * -1 + buffer[i-columnas] *-1 + buffer[i-columnas+1] *-1 + buffer[i+columnas] *-1 + buffer[i+columnas+1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //Caso orilla derecha
        else if(i != (columnas * filas)-1 && i != columnas -1 && i%(columnas-1) == 0 ){
            posPrima = (buffer[i-1] *-1 + buffer[i-columnas] *-1 + buffer[i-columnas-1] *-1 + buffer[i+columnas] *-1 + buffer[i+columnas-1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //Caso orilla inferior
        else if(i > (columnas * filas) - columnas && i < (columnas * filas) - 1 ){
            posPrima = (buffer[i-1] *-1 + buffer[i+1] *-1 + buffer[i-columnas] *-1 + buffer[i-columnas-1] *-1 + buffer[i-columnas+1] *-1 + buffer[i] * 8);
            newBuffer [i] = posPrima;
        }
        //Caso de al medio con 6 vecinos.
        else{
            posPrima = (buffer[i+1] *-1+ buffer[i-1] *-1+ buffer[i-columnas] *-1+ buffer[i-columnas-1] *-1+ buffer[i-columnas+1] *-1+ buffer[i+columnas]*-1 + buffer[i+columnas-1] *-1 + buffer[i+columnas + 1] *-1+ buffer[i] * 8); 
            newBuffer [i] = posPrima;
        }
        //Si es 
        i++;
    }
    *delineados = newBuffer;
}
