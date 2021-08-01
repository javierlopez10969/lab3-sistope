#include "funciones.h"

#define LECTURA 0
#define ESCRITURA 1
void crearProceso(int *id){
	*id = fork();
}
int main(int argc, char **argv){
    //Creamos el pipe para comunicarnos con el padre
    int *fd = (int *)malloc(sizeof(int) * 2);
    if (pipe(fd) == -1){
        printf("Error en syscall PIPE\n");
        exit(-1);
    }
    //Nombre de la imagen de salida
    char nombreSalida[50];
    strcpy(nombreSalida, argv[1]);
    //recibir las variables y cambiarlas desde char a entero
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    int factor = atoi(argv[4]);
    int grados = atoi(argv[5]);
    int flag = atoi(argv[6]);
    int N = (filas * columnas * factor * factor * 4);
    float *buffer = (float *)malloc(sizeof(float) * N);
    float * rotado = NULL;
    if (flag==1){printf("Proceso Rotación y escritura\n");
    printf("filas : %d columnas : %d \n",columnas*factor,filas*factor);}
    //lectura del buffer
    float rBuff[1];
    int i = 0;
    while(read(STDIN_FILENO, rBuff, sizeof(float)) > 0){
        buffer[i] = rBuff[0];
        i++;
    };
    //limpiar STDIN_FILENO
    fflush(STDIN_FILENO);
    if (flag==1)printf("Buffer leído \n");
    rotar(filas * factor,columnas * factor,grados,buffer , &rotado,N,flag);
    escribirImagen(nombreSalida,filas*factor, columnas*factor,rotado,N,flag);
    free(rotado);
    free(buffer);
    free(fd);
}
