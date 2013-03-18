#include "programa.h"

int inventario, capacidad,tiempoSuministro;
int t_funcionamiento = 480;
char *log_file_name;
FILE *log_file;

static void print_use(){
  printf("uso: bomba OPCIONES ...\n");
  printf("     OPCIONES:\n");
  printf("       -n  <nombre_bomba>\n");
  printf("       -cp <capacidad_maxima>\n");
  printf("       -i  <inventario>\n");
  printf("       -c  <consumo>\n");
  printf("       -fc  <fichero_centros>\n");
}

static void read_arg(char **argv, int *capacidad, char **fichero_centros, char **nombre_bomba, int *inventario, int *consumo) {
  int j;
  for(j = 0 ; j < 5 ; j++) {
    if(!strcmp(argv[2 * j + 1],"-cp")) {
      *capacidad = atoi(argv[2*j+2]);
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-fc")) {
      *fichero_centros = argv[2 * j + 2];
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-n")) {
      *nombre_bomba = argv[2 * j+2];
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-i")) {
      *inventario = atoi(argv[2 * j+2]);
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-c")) {
      *consumo = atoi(argv[2 * j + 2]);
      continue;
    }
    else {
      print_use();
      exit(EXIT_FAILURE);
    }
  }
}

int valid_arg(int capacidad, char *fichero_centros, char *nombre_bomba, int inventario, int consumo) {
  int valid = TRUE;
  if(capacidad < 38000 || capacidad > 380000) {
    printf("La capacidad maxima debe ser un numero comprendido entre 38000 y 380000.\n");
    valid = FALSE;
  }
  if(inventario < 0) {
    printf("El inventario debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(inventario > capacidad) {
    printf("El inventario actual no debe exceder a la capacidad maxima.\n");
    valid = FALSE;
  }
  if(consumo < 0) {
    printf("El valor de consumo debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(consumo > 1000) {
    printf("El valor de consumo no debe exceder a 1000.\n");
    valid = FALSE;
  }
  return valid;
}

void *inventario_consumo(void * tid) {
  int consumo = (int) tid;
  while (TRUE) {
    //printf("%d lts. ",inventario);
    usleep(100000);
    if(inventario == capacidad)
      fprintf(log_file,"Tanque full: %d minutos.\n",480 - t_funcionamiento);
    if(inventario - consumo > 0)
      inventario-=consumo;
    else if(inventario - consumo <= 0) {
      inventario = 0;
      fprintf(log_file,"Tanque vacio: %d minutos.\n",480 - t_funcionamiento);
    }
    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }

}

void *tiempo(void * tid) {
  while (TRUE) {
    //printf("%d min.\n",t_funcionamiento);
    usleep(100000);    
    t_funcionamiento--;
    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }
}


