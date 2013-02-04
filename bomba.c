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
int t_funcionamiento = 480;

struct centro_servidor;
typedef struct centro_servidor centro;

struct centro_servidor {
  char *nombre_centro;
  char *hostname;
  int puerto;
  int send_t;
  int busy;
  centro *next;
};


void agregar_directorio(centro** directorio, char *nombre_centro, char *hostname, int puerto) {
  centro *centro_;
  if(!(centro_ = (centro *) (malloc(sizeof(centro))))) {
    printf("Error: no se pudo reservar memoria para crear el directorio de centros.\n");
    exit(EXIT_FAILURE);
  }
  centro_->nombre_centro = (char *) malloc(strlen(nombre_centro));
  strcpy(centro_->nombre_centro,nombre_centro);
  centro_->hostname = (char *) malloc(strlen(hostname));
  strcpy(centro_->hostname,hostname);
  centro_->puerto = puerto;
  centro_->next = NULL;
  {
   int socketID = socket(AF_INET,SOCK_STREAM,0);
   struct hostent *server = gethostbyname(hostname);
   struct sockaddr_in dirServ;

   bzero((char *) &dirServ, sizeof(dirServ));
   dirServ.sin_family = AF_INET;
   dirServ.sin_port = htons(puerto);

   if(connect(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ)) == -1) {
     printf("Error: no se pudo conectar al centro para obtener tiempo de respuesta.\n");
     exit(EXIT_FAILURE);
   }
 
   send(socketID,"-\n",256,0);
   char buffer[256];
   bzero(buffer,256);
   recv(socketID,buffer,256,0);
   centro_->send_t = atoi(buffer);
   close(socketID);
  }
  centro_->busy = 0; 
  if(!*directorio)
    *directorio = centro_;
  else {
    centro *tmp = *directorio, *ant = NULL;
    while(tmp) {
      if(centro_->send_t <= tmp->send_t && !ant) {
        centro_->next = tmp;
        *directorio = centro_;
        break;
      }
      else if(centro_->send_t <= tmp->send_t && ant){
        ant->next = centro_;
        centro_->next = tmp;
        break;
      }
      else if(centro_->send_t > tmp->send_t)  {
        if(!tmp->next) {
          tmp->next = centro_;
          break;
        }
        ant = tmp;
        tmp = tmp->next;
      }
    }
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
  int consumo = (int) tid;
  while (TRUE) {
    sleep(3);
    if(inventario - consumo > 0)
      inventario-=consumo;
  }
}

void *tiempo(void * tid) {
  while (TRUE) {
    sleep(3);
    t_funcionamiento--;
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

  // Lectura del archivo con los centros de distribucion

  FILE *archivo = fopen(fichero_centros,"r");
  if (!archivo) {
    printf("Error: no se puede o no existe el archivo \"%s\".\n",fichero_centros);
    exit(EXIT_FAILURE);
  }

  char *linea;
  centro *directorio_centros = NULL;
  
  while(fscanf(archivo,"%s",linea) != EOF) {
    char *nombre_centro = (char *) strtok(linea,"&");
    char *hostname =  (char *) strtok (NULL, "&");
    int puerto = atoi((char *) strtok (NULL, "&"));
    agregar_directorio(&directorio_centros,nombre_centro,hostname,puerto);
  }

  /*{
    centro *tmp;tmp = directorio_centros;
    while (tmp) {
      printf("%d\n",tmp->send_t);
      tmp=tmp->next;
    }
  }*/

  pthread_t thread_inv, thread_func;
  pthread_attr_t attr1, attr2;

  pthread_attr_init(&attr1);
  pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_inv, &attr1, print_inventario, (void *) consumo)) {
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
  while (t_funcionamiento > 0 ) { 
    printf("%d %d\n",inventario,480 - t_funcionamiento + 1);
    if (capacidad - inventario >= 380) {
     c = *directorio_centros;
 
     struct sockaddr_in dirServ;
     struct hostent *server;
  
     socketID = socket(AF_INET,SOCK_STREAM,0);
     server = gethostbyname(c.hostname);

     bzero((char *) &dirServ, sizeof(dirServ));

     dirServ.sin_family = AF_INET;
     dirServ.sin_port = htons(c.puerto);
     if(connect(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ)) == -1) {
       continue;
     }
 
     send(socketID,strcat(nombre_bomba,"\n"),256,0);
     char buffer[256];
     bzero(buffer,256);
     recv(socketID,buffer,256,0);

     if(buffer[0] == 'O') {
       c.busy = 1;
       c = *c.next;
       continue;
     }
     else if (buffer[0] == 'D') {
       inventario+=380;
       c.busy = 0;
     }
    }
  }
  
  pthread_attr_destroy(&attr1);
  pthread_attr_destroy(&attr2);
  void * status;
  pthread_join(thread_inv,&status);
  pthread_join(thread_func,&status);
  close(socketID);
  fclose(archivo);
  exit(EXIT_SUCCESS);
}
