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

# define TRUE 1
# define FALSE 0

int inventario;

struct centro_servidor;
typedef struct centro_servidor centro;

struct centro_servidor {
  char *nombre_centro;
  char *hostname;
  int puerto;
  centro *next;
};


void agregar_directorio(centro** directorio, char *nombre_centro, char *hostname, int puerto) {
  centro *tmp;
  if(!(tmp = (centro *) (malloc(sizeof(centro))))) {
    printf("Error: no se pudo reservar memoria para crear el directorio de centros.\n");
    exit(EXIT_FAILURE);
  }
  tmp->nombre_centro = nombre_centro;
  tmp->hostname = hostname;
  tmp->puerto = puerto;
  tmp->next = NULL;

  tmp->next = *directorio;
  *directorio = tmp;
}

void print_dir(centro *dir) {
  centro *tmp;
  tmp = dir;
  while(tmp) {
    printf("%s %s %d\n",tmp->nombre_centro,tmp->hostname,tmp->puerto);
    tmp = tmp->next;
  }
}

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
void *print_inventario(void * tid) {
  while (TRUE) {
    sleep(3);
    //printf("inventario: %d\n",inventario);
    inventario--;
  }
}

int main(int argc, char **argv) {

  // Validacion y lectura de argumentos

  if(argc != 11){
    print_use();
    exit(EXIT_FAILURE);
  }
 
  char *nombre_bomba, *fichero_centros;
  int capacidad, consumo;
  
  read_arg(argv,&capacidad,&fichero_centros,&nombre_bomba,&inventario,&consumo);
  
  if (!valid_arg(capacidad,fichero_centros,nombre_bomba,inventario,consumo))
    exit(EXIT_FAILURE);

  //printf("%s %d %d %d %s\n",nombre_bomba,capacidad,inventario,consumo,fichero_centros);

  // Lectura del archivo con los centros de distribucion

  FILE *archivo = fopen(fichero_centros,"r");
  if (!archivo) {
    printf("Error: no se puede o no existe el archivo \"%s\".\n",fichero_centros);
    exit(EXIT_FAILURE);
  }

  char linea[80];
 
  centro *directorio_centros = NULL;

  while(fscanf(archivo,"%s",linea) != EOF) {
    char *token = (char *) strtok(linea,"&");
    char *nombre_centro = token;
    token = (char *) strtok (NULL, "&");
    char *hostname = token;
    token = (char *) strtok (NULL, "&");
    int puerto = atoi(token);
    agregar_directorio(&directorio_centros,nombre_centro,hostname,puerto);
  }

  {
    centro *tmp;
    tmp = directorio_centros;
    while(tmp) {
     printf("%s %s %d\n",tmp->nombre_centro,tmp->hostname,tmp->puerto);
     tmp = tmp->next;
    }
  } 

  exit(EXIT_SUCCESS);
 
  pthread_t thread;
  pthread_attr_t attr;

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

  if (pthread_create(&thread, &attr, print_inventario, (void *) &inventario)) {
    printf("Error: no se pudo crear el hilo.");
    exit(EXIT_FAILURE);
  }
 
  while (TRUE){
    printf("%d\n",inventario);
    sleep(5);
  }

/*  int socketID;
  struct sockaddr_in dirServ;
  struct hostent *server;
  char buffer[256];
  
  socketID = socket(AF_INET,SOCK_STREAM,0);

  server = gethostbyname(hostname);

  bzero((char *) &dirServ, sizeof(dirServ));

  dirServ.sin_family = AF_INET;
  dirServ.sin_port = htons(puerto);

  connect(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ));
  
  printf("Mensaje: ");
  bzero(buffer,256);
  fgets(buffer,256,stdin);

  send(socketID,buffer,256,0);

  recv(socketID,buffer,256,0);
  printf(buffer);

  pthread_attr_destroy(&attr);

  void * status;
  pthread_join(thread,&status);

  close(socketID);*/
  fclose(archivo);
  exit(EXIT_SUCCESS);
}
