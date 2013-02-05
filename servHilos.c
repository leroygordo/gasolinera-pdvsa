#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int busy = 0;
void procesaPeticion(void *arg){

  int socket = (int *)arg;
  char buffer[256];
  bzero(buffer,256);
  recv(socket, buffer,256,0);
  printf(buffer);
  sleep(10);
  bzero(buffer,256);
   if (busy == 1) 
     strcpy(buffer, "ocupado");
   else {
     busy = 1;
     strcpy(buffer, "descupado");
   }
  send(socket, buffer, 256,0);
  close(socket);
  pthread_exit(0);
} 

int main(){
 
  int socketID, puerto, newSocketID, transfDatos;
  socklen_t addrlen;
  struct sockaddr_in dirServ, dirCli;
  char buffer[256];
  char *message = "Hola, soy tu Servidor\n";
  pthread_t h;
  socketID = socket(AF_INET, SOCK_STREAM, 0); //Creacion socket
  puerto = 10223;
  
  //Inicializa dirServ en cero
  bzero((char*)&dirServ, sizeof(dirServ));

 
  dirServ.sin_family = AF_INET;
  dirServ.sin_port = htons (puerto);
  dirServ.sin_addr.s_addr = htonl(INADDR_ANY); // Cualquier IP local valida

  // bind() para enlace del socket con direccion IP y puerto
  bind (socketID, (struct sockaddr *) &dirServ, sizeof(dirServ));
  
  // Habilita el socket para que pueda recibir peticiones
  // 5 es el numero maximo de conexiones en la cola de espera. Las conexiones quedan en estado de espera en la cola hasta que se aceptan.
  listen(socketID,5);
  addrlen = sizeof(dirCli);
  
  while (1) {
    newSocketID = accept (socketID, (struct sockaddr *) &dirCli, &addrlen); // accept() no retorna hasta que se produzca una conexion o sea interrumpida por una senial.
    pthread_create(&h,NULL,procesaPeticion,(void *)newSocketID);
  }
  close(socketID);
 }


