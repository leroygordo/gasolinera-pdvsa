#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "estructuras.h"
#include "programa.h"
#include "desafio.c"


int agregar_directorio(centro** directorio, char *nombre_centro, char *hostname, char *nombre_bomba) {
  centro *centro_;
  if(!(centro_ = (centro *) (malloc(sizeof(centro))))) {
    printf("Error: no se pudo reservar memoria para crear el directorio de centros.\n");
    return FALSE;
  }

  centro_->nombre_centro = (char *) malloc(strlen(nombre_centro));
  strcpy(centro_->nombre_centro,nombre_centro);
  centro_->hostname = (char *) malloc(strlen(hostname));
  strcpy(centro_->hostname,hostname);
  centro_->next = NULL;
  {
  CLIENT *clnt;
  clnt = clnt_create (centro_->hostname, PROGRAMA_PROG, PROGRAMA_VER, "udp");
  if (clnt == NULL) {
   clnt_pcreateerror (centro_->hostname);
   exit (EXIT_FAILURE);
  }

  desafio *d;
  d = (desafio *) malloc(sizeof(desafio));
  d->pregunta = (char *) malloc(32);

  char **p = preguntar_1(&nombre_bomba,clnt);
  if (p == (char **) NULL) {
	clnt_perror (clnt, "call failed");
  }
  strcpy(d->pregunta,*p);

  char *centro_enc  = (char *) malloc(32);
  MDString(centro_->nombre_centro,centro_enc);
 
  d->respuesta = (char *) malloc(64);
  sprintf(d->respuesta,"%s%s",d->pregunta,centro_enc);

  d->nombre_bomba = (char *) malloc(sizeof(strlen(nombre_bomba)));
  strcpy(d->nombre_bomba,nombre_bomba);

  centro_->ticket_ = responder_1(d,clnt);

  centro_->send_t = *pedir_tiempo_1(centro_->ticket_, clnt);

  free(d->pregunta);
  free(d->respuesta);
  free(d->nombre_bomba);
  free(d);
  clnt_destroy (clnt);
 }
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

centro *crear_directorio(char *fichero_centros,char *nombre_bomba) {
  FILE *archivo = fopen(fichero_centros,"r");
  centro *directorio;
  directorio = NULL;
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
   
    if(!agregar_directorio(&directorio,nombre_centro,hostname,nombre_bomba)){
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
