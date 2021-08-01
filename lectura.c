#include "funciones.h"

#define LECTURA 0
#define ESCRITURA 1

int main(int argc, char** argv){
    //Nombre de la imagen
    char bufferNombre[50];
    strcpy(bufferNombre, argv[1]);
    //recibir las variables y cambiarlas desde char a entero
    int filas = atoi(argv[3]); 
    int columnas = atoi(argv[4]);
    //mostrar proceso mediante bandera
    int flag = atoi(argv[7]);
    if (flag == 1) {
        printf("Proceso de Lectura\n");
    }
    int N = (filas * columnas * 4);
    //Creamos un buffer
    float *buffer = (float *)malloc(sizeof(float) * N);
    //Leer la imagen
    leerArchivo(bufferNombre, filas, columnas, buffer, N,flag);
    //Creacion pipe
    int * fd = (int*)malloc(sizeof(int) * 2);
    if(pipe(fd) == -1){
        printf("Error en syscall PIPE\n");
        exit(-1);
    }
    //Proceso fork crea hijo
    int pid_zoom;    
    if((pid_zoom = fork()) == -1){
        printf("Error en syscall FORK\n");
        exit(-1);
    }

    if(pid_zoom == 0){ // Proceso del hijo
        //Cerramos el modo escritura y abrimos el modo lectura del fd
        //Para comunicarnos con el siguiente proceso
        close(fd[ESCRITURA]); 
        dup2(fd[LECTURA], STDIN_FILENO);  
        //./zoom, nombreSalida, filas, columnas, factor, grados, flag
        char *args[8]={"./zoom", argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], NULL};     
        //Cambiamos la imagen de ejecución con excevp
        execvp(args[0],args);
    }
    else{ // Padre
        //El padre escribe el buffer de la imagen mediante write
        close(fd[LECTURA]);
        write(fd[ESCRITURA], buffer, filas*columnas*sizeof(float));
        close(fd[ESCRITURA]);
        wait(NULL);
        free(buffer);
        free(fd);
        
    }
}