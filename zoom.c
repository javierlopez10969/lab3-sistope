#include "funciones.h"

#define LECTURA 0
#define ESCRITURA 1
void crearProceso(int *id){
	*id = fork();
}
int main(int argc, char **argv)
{
    //Creamos el pipe para comunicarnos con el padre
    int *fd = (int *)malloc(sizeof(int) * 2);
    if (pipe(fd) == -1){
        printf("Error en syscall PIPE\n");
        exit(-1);
    }


    //recibir las variables y cambiarlas desde char a entero
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    int factor = atoi(argv[4]);
    int flag = atoi(argv[6]);
    if (flag==1)printf("Proceso ZOOM\n");
    int N = (filas * columnas * 4);
    float *buffer = (float *)malloc(sizeof(float) * N);
    
    //Leer la imagen que nos ha escrito el padre mediante el pipe
    if (flag==1)printf("Voy a leer el buffer\n");
    float rBuff[1];
    int i = 0;
    while(read(STDIN_FILENO, rBuff, sizeof(float)) > 0){
        buffer[i] = rBuff[0];
        i++;
    };
    fflush(STDIN_FILENO);
    if (flag==1)printf("Buffer leído \n");
    float *zoom = NULL;
    zoomIN(filas, columnas, buffer, &zoom, factor, N);
    //Proceso fork crea hijo    
    int pid_suavizado;    
    crearProceso(&pid_suavizado);
    if(pid_suavizado  == -1){
        printf("Error en syscall FORK\n");
        exit(-1);
    }
    else if (pid_suavizado>0){ // Padre
        //El padre escribe el buffer de la imagen mediante write
        close(fd[LECTURA]);
        write(fd[ESCRITURA], zoom, N*sizeof(float));
        close(fd[ESCRITURA]);
        wait(NULL);
        //liberar memoria despues del proceso hijo
        free(zoom);
        free(buffer);
        free(fd);
    }
    else {
        close(fd[ESCRITURA]);
        dup2(fd[LECTURA], STDIN_FILENO);
        if (flag==1){printf("Se termina el proceso de zoom\n");}
        char *args[8] = {"./suavizado", argv[1], argv[2], argv[3], argv[4], argv[5],argv[6], NULL};
        execvp(args[0],args);
    }
}