#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

# define TRUE 1
# define FALSE 0

static void print_use(){
  printf("uso: bomba OPCIONES ...\n");
  printf("     OPCIONES:\n");
  printf("       -n  <nombre_bomba>\n");
  printf("       -cp <capacidad_maxima>\n");
  printf("       -i  <inventario>\n");
  printf("       -c  <consumo>\n");
  printf("       -fc  <fichero_centros>\n");
}

int main(int argc, char **argv) {

  // Validacion y lectura de argumentos

  if(argc != 11){
    print_use();
    exit(EXIT_FAILURE);
  }
 
  char *nombre_bomba, *fichero_centros;
  int capacidad, inventario, consumo;

  {
   int j;
   for(j = 0 ; j < 5 ; j++) {
     if(!strcmp(argv[2 * j + 1],"-cp")) {
       capacidad = atoi(argv[2*j+2]);
     }
     else if(!strcmp(argv[2 * j + 1],"-fc")) {
       fichero_centros = argv[2 * j + 2];
     }
     else if(!strcmp(argv[2 * j + 1],"-n")) {
       nombre_bomba = argv[2 * j+2];
     }
     else if(!strcmp(argv[2 * j + 1],"-i")) {
       inventario = atoi(argv[2 * j+2]);
     }
     else if(!strcmp(argv[2 * j + 1],"-c")) {
       consumo = atoi(argv[2 * j + 2]);
     }
     else {
        print_use();
        exit(EXIT_FAILURE);
     }
   }
  }

  printf("%s %d %d %d %s\n",nombre_bomba,capacidad,inventario,consumo,fichero_centros);
}
