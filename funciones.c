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
    //printBuffer(filas*factor,columnas*factor,newBuffer);
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

//Entradas: un entero que representa los grados
//Funcionamiento: si los grados superan los 360 grados, seran transformados a su equivalente entre 0 y 360 grados
//Salidas: un entero que representa los grados
int transformarGrados(int grados){
    int gradosAux= grados;
    while (gradosAux-360>0){
            gradosAux = gradosAux-360;
    }
    return gradosAux;
}

//Entradas: Numero de filas y columnas, el entero con los grados, un buffer vacio, el buffer de la imagen ya rotada-
//N que corresponde a la cantidad de bytes (filas * columnas * 4) y por ultimo la bandera para mostrar el estado actual del programa
//Funcionamiento: con el buffer de imagen (con zoomIn y suavizado aplicado) se procede a rotar dependiendo del valor de grados en las variables de entrada. escribiendo en un nuevo
//buffer los valores del arreglo post rotacion
//Salidas: retorna un buffer con el contenido de la imagen cambiado de posicion (rotacion)
void rotar(int filas, int columnas,int grados,float * buffer , float ** rotado, int N,int flag){
    float * newBuffer = (float*)malloc(sizeof(float)*(columnas*filas*4));
    grados = transformarGrados(grados);
    if (flag==1)printf("Grados :  %d\n", grados);
    //Iterador que recorre el nuevo buffer
    int i = 0 ;int k = 0; int elemento;
    //Ver que tipo de rotacion hay que hacer
    //0 o 360 : no darlo vuelta
    if (grados == 0 || grados%360==0){
        if (flag==1)printf("No rotamos\n\n");
        newBuffer = buffer; 
    }    
    //270 darlo vuelta hacia la izquierda.0000
    else if(grados%270 == 0){
        if (flag==1)printf("rotando en 270\n");
        i = filas -1;
        k = 0;
        int comienzo = filas-1;
        while (k< filas*columnas){
            elemento = buffer[i];
            newBuffer[k] = elemento;
            if (i+filas > filas*columnas){
                i = comienzo -1;
                comienzo = comienzo-1;
            }else{
                i = i +filas;
            }
            k++;
        }        
    }
    //180 darlo vuelta de cabeza
    else if(grados%180 == 0){
        if (flag==1)printf("rotando en 180\n");
        //Vamos del ultimo elemento buffer hacia el primero
        i = filas*columnas;
        k = 0;
        while (i!=0){
            elemento =  buffer[k];
            newBuffer[i] = elemento;
            i--;
            k ++;
        }
        newBuffer [0] = buffer[filas*columnas-1];
    }
    //90 darlo vuelta hacia la derecha
    else if(grados %90 == 0){
        if (flag==1)printf("rotando en 90\n");
        i = filas -1;
        k = 0;
        int comienzo = filas-1;
        while (k< filas*columnas){
            elemento = buffer[i];
            newBuffer[k] = elemento;
            if (i+filas > filas*columnas){
                i = comienzo -1;
                comienzo = comienzo-1;
            }else{
                i = i +filas;
            }
            k++;
        } 
        //invertir la imagen en 180 grados para que se aplique una rotacion en 90 grados respecto a la imagen inicial.
        float * buffer2 =NULL;
        //recursión para rotar de nuevo
        rotar(filas,columnas,180,newBuffer, &buffer2, N,flag);
        newBuffer = buffer2;
    }else{
        if (flag==1)printf("Nada : clown");
    }
    *rotado = newBuffer;    
}
