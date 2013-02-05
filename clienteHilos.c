#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

int main() {

  int socketID, puerto, transfDatos;
  struct sockaddr_in dirServ;
  struct hostent *server;
  char buffer[256];
  char *message = "Hola, soy tu Cliente\n";

  socketID = socket(AF_INET, SOCK_STREAM, 0);
  puerto = 10223;
  server = gethostbyname("chachara");

  bzero((char *)&dirServ, sizeof(dirServ));
  dirServ.sin_family = AF_INET;
  dirServ.sin_port = htons(puerto);
  //bcopy((char *)server->h_addr, (char*)&dirServ.sin_addr.s_addr, server->h_length);
  connect(socketID, (struct sockaddr *)&dirServ, sizeof(dirServ));
  printf("Mensaje: ");
  bzero(buffer,256);
  fgets(buffer,256,stdin);
  //transfDatos = write(socketID, buffer, strlen(buffer));
  send(socketID, buffer, 256, 0);
  bzero(buffer,256);
  //transfDatos = read(socketID, buffer, 255);
  recv(socketID, buffer, 256, 0);
  printf(buffer);
  close(socketID);
 
}


