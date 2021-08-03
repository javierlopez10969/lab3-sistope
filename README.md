# Laboratorio 3 Sistemas Operativos 😳
Laboratorio número tres de sistemas opertativos realizado por Javier López y Felipe Sepulveda.
<p align="center"><a target="_blank"><img src="https://upload.wikimedia.org/wikipedia/commons/3/35/The_C_Programming_Language_logo.svg" width="300"></a></p>

# Comandos para ejecutar 😈
Para ejecutar estos scripts, abra una terminal y ejecute el siguiente comando para compilar:

`make`

## Entradas que recibe este el ejecutable de este laboratorio 🥵 : 
* -I filename : especifica el nombre de la imagen de entrada
* -O filename : especifica el nombre de la imagen final resultante del pipeline.
* -M n´umero : especifica el n´umero de filas de la imagen
* -N n´umero : especifica el n´umero de columnas de la imagen
* -h n´umero : especifica el n´umero de hebras.
* -r factor : factor de replicaci´on para Zoom-in
* -b n´umero : especifica el tamano del buffer de la hebra productora.
* -f: bandera que indica si se entregan resultados por consola. En caso de que se ingrese este flag debera mostrar las dimensiones de la imagen

Para ejecutar el código sin mostrar nada por pantalla su bandera debe estar en 0, por default es cero, asi que si no ingresa nada quedará en 0.
# 👉  👈

`./lab3 -I cameraman_256x256.raw -O imagenSalida.raw -M 256 -N 256 -r 2 -h 4 -b 64` 

Para mostrar las entradas por pantalla ejecute  :  

`./lab3 -I cameraman_256x256.raw -O imagenSalida.raw -M 256 -N 256 -r 2 -h 4 -b 64 -f`

Si no entrega alguna de las entradas el programa no correrá. Tampoco correrá en caso de tener una cantidad distinta entre filas y columnas. Tampoco funcionará si los grados ingresados no son múltiplos de 90. 
# 🤡
