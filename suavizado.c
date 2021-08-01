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
    //recibir las variables y cambiarlas desde char a entero
    int filas = atoi(argv[2]);
    int columnas = atoi(argv[3]);
    int factor = atoi(argv[4]);
    int flag = atoi(argv[6]);
    int N = (filas * columnas * factor * factor * 4);
    float *buffer = (float *)malloc(sizeof(float) * N);

    if (flag==1){printf("Proceso Suavizado\n");
    printf("filas : %d columnas : %d \n",columnas*factor,filas*factor);}
    float rBuff[1];
    int i = 0;
    while(read(STDIN_FILENO, rBuff, sizeof(float)) > 0){
        buffer[i] = rBuff[0];
        i++;
    };
    fflush(STDIN_FILENO);
    float * suavizados = NULL;
    suavizado(filas * factor, columnas* factor,buffer , &suavizados, N);
    //Proceso fork crea hijo    
    int pid_rotacion;    
    crearProceso(&pid_rotacion);
    if(pid_rotacion  == -1){
        printf("Error en syscall FORK\n");
        exit(-1);
    }
    else if (pid_rotacion>0){ // Padre
        //El padre escribe el buffer de la imagen mediante write
        close(fd[LECTURA]);
        write(fd[ESCRITURA], suavizados, factor*factor*columnas*filas*sizeof(float));
        close(fd[ESCRITURA]);
        wait(NULL);
        //liberar memoria despues del proceso hijo
        free(suavizados);
        free(buffer);
        free(fd);
    }
    else {
        close(fd[ESCRITURA]);
        dup2(fd[LECTURA], STDIN_FILENO);
        char *args[8] = {"./rotacion", argv[1], argv[2], argv[3], argv[4], argv[5],argv[6], NULL};
        execvp(args[0],args);
    }
}