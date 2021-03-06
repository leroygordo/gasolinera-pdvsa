#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "estructuras.h"

# define TRUE 1
# define FALSE 0

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

int main(int argc, char **argv) {

  if(argc != 11) {
    print_use();
    exit(EXIT_FAILURE);
  }
 
  char *nombre_bomba, *fichero_centros;
  int consumo;
  
  // Lectura de argumentos
  read_arg(argv,&capacidad,&fichero_centros,&nombre_bomba,&inventario,&consumo);
  
  // Validacion de argumentos
  if (!valid_arg(capacidad,fichero_centros,nombre_bomba,inventario,consumo))
    exit(EXIT_FAILURE);

  centro *directorio_centros = NULL;

  // Creacion del directorio con los centros de distribucion que se encuentran en 'fichero_centros' 
  if (!(directorio_centros = crear_directorio(fichero_centros))) {
    printf("Error: no se pudo crear el directorio de centros de distribucion.\n");
    exit(EXIT_FAILURE); 
  }
  
  log_file_name = (char *) malloc(strlen(nombre_bomba)+8);
  sprintf(log_file_name,"log_%s.txt",nombre_bomba);

  log_file = fopen(log_file_name,"w");
  if(!log_file) {
    printf("Error: no se pudo crear el archivo log\n.");
    exit(EXIT_FAILURE);
  }
 
  fprintf(log_file,"Inventario inicial: %d litros.\n",inventario);

  pthread_t thread_inv, thread_func;
  pthread_attr_t attr1, attr2;

  pthread_attr_init(&attr1);
  pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_inv, &attr1, inventario_consumo, (void *) consumo)) {
    printf("Error: no se pudo crear el hilo para controlar el inventario.");
    exit(EXIT_FAILURE);
  }

  pthread_attr_init(&attr2);
  pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_func, &attr2, tiempo,NULL)) {
    printf("Error: no se pudo crear el hilo para controlar el funcionamiento.");
    exit(EXIT_FAILURE);
  }


  int socketID;
  centro c;
  c = *directorio_centros;
  while (t_funcionamiento > 0 ) { 
    if (capacidad - inventario >= 38000) {
      struct sockaddr_in dirServ;
      struct hostent *server;
  
      socketID = socket(AF_INET,SOCK_STREAM,0);
      server = gethostbyname(c.hostname);
     
      bzero((char *) &dirServ, sizeof(dirServ));

      dirServ.sin_family = AF_INET;
      dirServ.sin_port = htons(c.puerto);
      memcpy(&dirServ.sin_addr,server->h_addr_list[0],server->h_length);
      if(connect(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ)) == -1) {
        if(c.next == NULL)
          break;
          //c = *directorio_centros;
         else
          c = *c.next;
	continue;
      }
 
      //send(socketID,strcat(nombre_bomba),256,0);
      char buffer[256];
      bzero(buffer,256);
      strcpy(buffer,nombre_bomba);
      send(socketID,buffer,256,0);
      bzero(buffer,256);
      recv(socketID,buffer,256,0);
 
      if(buffer[0]== 'O') {
        if(c.next == NULL)
          c = *directorio_centros;
         else
          c = *c.next;
         fprintf(log_file,"Peticion: %d minutos, %s, Fallido.\n",480 - t_funcionamiento,c.nombre_centro);
      }
      else if (buffer[0] == 'D') {
        tiempoSuministro = c.send_t;
        fprintf(log_file,"Peticion: %d minutos, %s, OK.\n",480 - t_funcionamiento,c.nombre_centro);
        usleep(tiempoSuministro * 100000);
        if (inventario + 38000 >= capacidad) {
          inventario = capacidad;
        }
        else {
          inventario+=38000;
        }
        fprintf(log_file,"Llegada de la gandola: %d minutos, %d litros.\n",480 - t_funcionamiento,inventario); 
        c = *directorio_centros;
      }
    }
  }
  destruir_directorio(&directorio_centros);  
  pthread_attr_destroy(&attr1);
  pthread_attr_destroy(&attr2);
  void * status;
  pthread_join(thread_inv,&status);
  pthread_join(thread_func,&status);
  close(socketID); 
  fclose(log_file);
  exit(EXIT_SUCCESS);
}
