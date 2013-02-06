#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

# define TRUE 1
# define FALSE 0

int busy = 0;
int tiempo, inventario, capacidad;
int t_funcionamiento = 24;
char *log_file_name;
FILE *log_file;

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

static void read_arg(char **argv,int argc, char **nombre_centro,int *capacidad,int *inventario,int *suministro,int *puerto,int *tiempo) {
  // getopt no parsea argumentos cuyo flag es mas de un caracter
  int cp = FALSE;
  {
   int j;
   for(j = 0 ; j < 6 ; j++)
     if(!strcmp(argv[2*j + 1],"-cp")) {
       cp = TRUE;
       *capacidad = atoi(argv[2*j+2]);
       break;
     }
  }
  
  if(!cp) {
    print_use();
    exit(EXIT_FAILURE);
  }
  else 
    while(TRUE){
      int options = getopt(argc,argv,"n:i:t:s:p:c:");
      if (options == -1)
        break;
      switch(options) {
        case 'n':
          *nombre_centro = optarg;
          break;      
        case 'i':
          *inventario = atoi(optarg);
          break;
        case 't':
          *tiempo = atoi(optarg);
          break;
        case 's':
          *suministro = atoi(optarg);
          break;
        case 'p':
          // Para hacer simple el programa, el numero de puerto se valida de una vez aca.
          if(strlen(optarg) != 5) {
            printf("Error: El numero de puerto debe tener 5 digitos.\n");
            exit(EXIT_FAILURE);
          }
          *puerto = atoi(optarg);
          break;
        case 'c':
          break;
        default:
          print_use();
          exit(EXIT_FAILURE);
      }
    }
}

int valid_arg(char *nombre_centro,int capacidad,int inventario,int suministro,int tiempo) {
  int valid = TRUE;
  if(strlen(nombre_centro) > 40) {
    printf("Error: Nombre del centro debe ser a lo sumo de 40 caracteres.\n");
    valid = FALSE;
  }
  if(capacidad < 38000 || capacidad > 3800000) {
    printf("La capacidad maxima debe ser un numero comprendido entre 38000 y 3800000.\n");
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
  if(suministro < 0) {
    printf("El valor de suministro debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(suministro > 10000) {
    printf("El valor de suministro no debe exceder a 10000.\n");
    valid = FALSE;
  }
  if(tiempo < 0) {
    printf("El valor del tiempo debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(tiempo > 180) {
    printf("El valor del tiempo no debe exceder a 180.\n");
    valid = FALSE;
  }
  return valid;
}

void *procesarPeticion(void *tid){
  int socket = (int) tid;
  char buffer[256];
  bzero(buffer,256);

  if (recv(socket,buffer,256,0) < 0){
    error("Error recibiendo los datos");
  };
 
  if (busy == 0) {  
    *(&busy) = 1;
    if(buffer[0] == '-') { 
      bzero(buffer,256);
      sprintf(buffer,"%d",tiempo);
    }
    else{
      sleep(tiempo);
      strcpy(buffer,"Disponible");
    }
  }
  else
    strcpy(buffer,"Ocupado");


  if (send(socket,strcat(buffer,"\n"),256,0) < 0) {
    error("Error mandando los datos");
  }
  //Hay que arreglar algo aca. Cuando el servidor esta ocupado, lo dice pero setea el flag busy a 0
  *(&busy) = 0;
  close(socket);
  pthread_exit(0);
} 

void *inventario_suministro(void * tid) {
  int suministro = (int) tid;
  while (TRUE) {
    printf("%d lts. \n",inventario);
    sleep(3);
    if(inventario + suministro < capacidad)
      inventario+=suministro;
    else if(inventario + suministro >= capacidad)
      inventario = capacidad;
    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }

}

void *tiempo_funcionamiento(void * tid) {
  while (TRUE) {
    printf("%d min. ",t_funcionamiento);
    sleep(3);
    t_funcionamiento--;
    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }
}

int main(int argc, char **argv) {

  // Validacion y lectura de argumentos

  if(argc != 13){
    print_use();
    exit(EXIT_FAILURE);
  }

  char * nombre_centro;
  int suministro, puerto;

  read_arg(argv,argc,&nombre_centro,&capacidad,&inventario,&suministro,&puerto,&tiempo);

  if (!valid_arg(nombre_centro,capacidad,inventario,suministro,tiempo))
   exit(EXIT_FAILURE); 

  log_file_name = (char *) malloc(strlen(nombre_centro)+8);
  sprintf(log_file_name,"log_%s.txt",nombre_centro);

  log_file = fopen(log_file_name,"w");
  if(!log_file) {
    printf("Error: no se pudo crear el archivo log el archivo\n.");
    exit(EXIT_FAILURE);
  }

  fprintf(log_file,"Inventario inicial: %d litros.\n",inventario);

  pthread_t thread_inv, thread_func;
  pthread_attr_t attr1, attr2;

  pthread_attr_init(&attr1);
  pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_inv, &attr1, inventario_suministro, (void *) suministro)) {
    printf("Error: no se pudo crear el hilo para controlar el inventario.");
    exit(EXIT_FAILURE);
  }

  pthread_attr_init(&attr2);
  pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_func, &attr2, tiempo_funcionamiento,NULL)) {
    printf("Error: no se pudo crear el hilo para controlar el funcionamiento.");
    exit(EXIT_FAILURE);
  }

  while (t_funcionamiento > 0);
  
  exit(1);

  int socketID, newSocketID;
  socklen_t addrlen;
  struct sockaddr_in dirServ, dirCli;
  char buffer[256];
  pid_t son;
  pthread_t h;

  socketID = socket(AF_INET,SOCK_STREAM,0);
  if (socketID == 0) {
    error("Error abriendo el socket");
  }

  bzero((char*)&dirServ,sizeof(dirServ));

  dirServ.sin_family = AF_INET;
  dirServ.sin_port = htons(puerto);
  dirServ.sin_addr.s_addr = htonl(INADDR_ANY);
  
  if (bind(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ)) < 0 ) {
    error("Error en la conexion");
  }
  
  listen(socketID,5);

  addrlen = sizeof(dirCli);

  while (TRUE) {
    newSocketID = accept(socketID,(struct sockaddr *)&dirCli,&addrlen);
    if (newSocketID < 0) {
      error("Error aceptando la conexion");
    }
    pthread_create(&h,NULL,procesarPeticion, (void *)newSocketID);
    //busy = 0;
  }  
  
  close(socketID);
  exit(EXIT_SUCCESS);
}
