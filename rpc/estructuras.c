#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "estructuras.h"
#include "programa.h"


int agregar_directorio(centro** directorio, char *nombre_centro, char *hostname, int puerto) {
  CLIENT *clnt;
int  *result_1;
estructura  pedir_gasolina_1_arg;

  centro *centro_;
  if(!(centro_ = (centro *) (malloc(sizeof(centro))))) {
    printf("Error: no se pudo reservar memoria para crear el directorio de centros.\n");
    return FALSE;
  }
  centro_->nombre_centro = (char *) malloc(strlen(nombre_centro));
  strcpy(centro_->nombre_centro,nombre_centro);
  centro_->hostname = (char *) malloc(strlen(hostname));
  strcpy(centro_->hostname,hostname);
  centro_->puerto = puerto;
  centro_->next = NULL;
  {
    clnt = clnt_create (centro_->nombre_centro, CENTROPROG, CENTRO_VER, "udp");
    if (clnt == NULL) {
      clnt_pcreateerror (centro_->nombre_centro);
      exit (1);
    }
    result_1 = pedir_tiempo_1(&pedir_tiempo_1_arg, clnt);
    if (result_1 == (int *) NULL) {
      clnt_perror (clnt, "call failed");
    }
    centro_->send_t = *result_1;
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
  return TRUE;
}

centro *crear_directorio(char *fichero_centros) {
  FILE *archivo = fopen(fichero_centros,"r");
  centro *directorio;
  if (!archivo) {
    printf("Error: no se puede abrir o no existe el archivo \"%s\".\n",fichero_centros);
    return FALSE;
  }
 
  char linea[87];  
  while(fscanf(archivo,"%s",linea) != EOF) {
    char *nombre_centro = (char *) strtok(linea,"&");
    if(strlen(nombre_centro) > 40) {
      printf("Error: Nombre del centro debe ser a lo sumo de 40 caracteres.\n");
      return FALSE;
    }
    char *hostname =  (char *) strtok (NULL, "&");
    if(strlen(hostname) > 40) {
      printf("Error: Hostname debe ser a lo sumo de 40 caracteres.\n");
      return FALSE;
    }
    char *port = (char *) strtok (NULL, "&");
    if(strlen(port) != 5) {
      printf("Error: El numero de puerto debe tener 5 digitos.\n");
      return FALSE;
    }
    int puerto = atoi(port);
    if(!agregar_directorio(&directorio,nombre_centro,hostname,puerto)){
      //printf("Error: no se pudo crear el directorio de centros de distribucion.\n");
      //return FALSE;
      continue;
    }
  }
 fclose(archivo);
 return directorio;
}

void destruir_directorio(centro **directorio) {
  centro *tmp;
  tmp = *directorio;

  while(tmp) {
    *directorio = tmp->next;
    free(tmp);
    tmp = *directorio;
  }

}
