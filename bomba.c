#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

# define TRUE 1
# define FALSE 0

static void print_use(){
  printf("uso: centro OPCIONES ...\n");
  printf("     OPCIONES:\n");
  printf("       -n  <nombre_centro>\n");
  printf("       -cp <capacidad_maxima>\n");
  printf("       -i  <inventario>\n");
  printf("       -t  <tiempo>\n");
  printf("       -s  <suministro>\n");
  printf("       -p  <puerto>\n");
}

int main(int argc, char **argv) {

  // Validacion y lectura de argumentos

  if(argc != 13){
    print_use();
    exit(EXIT_FAILURE);
  }

  char * nombre_centro;
  int capacidad, inventario, tiempo, suministro, puerto;

  // getopt no parsea argumentos cuyo flag es mas de un caracter
  int cp = FALSE;
  {
   int j;
   for(j = 0 ; j < 6 ; j++)
     if(!strcmp(argv[2*j + 1],"-cp")) {
       cp = TRUE;
       capacidad = atoi(argv[2*j+2]);
       break;
     }
  }
  
  if(!cp)
    printf("uso:\n");
  else 
    while(TRUE){
      int options = getopt(argc,argv,"n:i:t:s:p:c:");
      if (options == -1)
        break;
      switch(options) {
        case 'n':
          nombre_centro = optarg;
          break;      
        case 'i':
          inventario = atoi(optarg);
          break;
        case 't':
          tiempo = atoi(optarg);
          break;
        case 's':
          suministro = atoi(optarg);
          break;
        case 'p':
          puerto = atoi(optarg);
          break;
        case 'c':
          break;
        default:
          print_use();
          break;
      }
    }

  printf("%s %d %d %d %d %d\n",nombre_centro,capacidad,inventario,tiempo,suministro,puerto);
}
