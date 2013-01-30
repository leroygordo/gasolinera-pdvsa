#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

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

void procesarPeticion(int socket){
  char buffer[256];
  bzero(buffer,256);
  recv(socket, buffer,256,0);
  printf(buffer);
  sleep(3);
  send(socket, buffer, 256,0);
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

  //printf("%s %d %d %d %d %d\n",nombre_centro,capacidad,inventario,tiempo,suministro,puerto);

  int socketID, newSocketID;
  socklen_t addrlen;
  struct sockaddr_in dirServ, dirCli;
  char buffer[256];
  pid_t son;

  socketID = socket(AF_INET,SOCK_STREAM,0);
  
  bzero((char*)&dirServ,sizeof(dirServ));

  dirServ.sin_family = AF_INET;
  dirServ.sin_port = htons(puerto);
  dirServ.sin_addr.s_addr = htonl(INADDR_ANY);

  bind(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ));
  
  listen(socketID,5);

  addrlen = sizeof(dirCli);

  while (TRUE) {
    newSocketID = accept(socketID,(struct sockaddr *)&dirCli,&addrlen);
    son = fork();
    if(!son) {
      procesarPeticion(newSocketID);
      exit(EXIT_SUCCESS);
    }
    else {
      close(newSocketID);
    }
  }
  close(socketID);
  exit(EXIT_SUCCESS);
}
