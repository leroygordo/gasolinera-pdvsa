#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


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
       continue;
     }
     else if(!strcmp(argv[2 * j + 1],"-fc")) {
       fichero_centros = argv[2 * j + 2];
       continue;
     }
     else if(!strcmp(argv[2 * j + 1],"-n")) {
       nombre_bomba = argv[2 * j+2];
       continue;
     }
     else if(!strcmp(argv[2 * j + 1],"-i")) {
       inventario = atoi(argv[2 * j+2]);
       continue;
     }
     else if(!strcmp(argv[2 * j + 1],"-c")) {
       consumo = atoi(argv[2 * j + 2]);
       continue;
     }
     else {
        print_use();
        exit(EXIT_FAILURE);
     }
   }
  }

  //  printf("%s %d %d %d %s\n",nombre_bomba,capacidad,inventario,consumo,fichero_centros);

  // Lectura del archivo con los centros de distribucion

  FILE *archivo = fopen(fichero_centros,"r");
  if (!archivo) {
    printf("Error: no se puede o no existe el archivo \"%s\".\n",fichero_centros);
    exit(EXIT_FAILURE);
  }

  char *nombre_centro, *hostname, linea[80], *token;
  int puerto;
  while(fscanf(archivo,"%s",linea) != EOF) {
    token = (char *) strtok(linea,"&");
    nombre_centro = token;
    token = (char *) strtok (NULL, "&");
    hostname = token;
    token = (char *) strtok (NULL, "&");
    puerto = atoi(token);
  }
  //printf("%s %s %d\n\n",nombre_centro,hostname,puerto);
  
  int socketID;
  struct sockaddr_in dirServ;
  struct hostent *server;
  char buffer[256];
  
  socketID = socket(AF_INET,SOCK_STREAM,0);
 
  server = gethostbyname(hostname);
  if (server == NULL) {
  fprintf(stderr,"Error no hay host");
  exit(0);
  }
  bzero((char *) &dirServ, sizeof(dirServ));

  dirServ.sin_family = AF_INET;
  bcopy((char *)server->h_addr,(char*)&dirServ.sin_addr.s_addr,server->h_length);
  dirServ.sin_port = htons(puerto);
  

   if (connect(socketID,(struct sockaddr *)&dirServ,sizeof(dirServ)) < 0)
     error("Error enla conexion");
   
  printf("Mensaje: ");
  bzero(buffer,256);
  fgets(buffer,256,stdin);

  if (send(socketID,buffer,256,0) < 0)
    error("Error mandando dato CLientes");
  

  if (recv(socketID,buffer,256,0) < 0)
    error("Error recibiendo datos Cliente");

  printf(buffer);
  close(socketID);
  fclose(archivo);
  exit(EXIT_SUCCESS);
}
