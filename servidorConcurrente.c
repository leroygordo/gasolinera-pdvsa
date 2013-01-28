#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void procesaPeticion(int socket){
  char buffer[256];
  bzero(buffer,256);
  // transfDatos = read(newSocketID, buffer, 255);
  recv(socket, buffer,256,0);
  printf(buffer);
  //bzero(buffer,256);
  //transfDatos = write(newSocketID, buffer, 255);
  sleep(100);
  send(socket, buffer, 256,0);
} 

int main(){

  int socketID, puerto, newSocketID, transfDatos;
  socklen_t addrlen;
  struct sockaddr_in dirServ, dirCli;
  char buffer[256];
  char *message = "Hola, soy tu Servidor\n";
  pid_t hijo;
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
  hijo = fork();
  if (hijo == 0) {
    //close(socketID);
    procesaPeticion(newSocketID);
    exit(0);
  }
  else
    close(newSocketID);
  }
  close(socketID);
 }

