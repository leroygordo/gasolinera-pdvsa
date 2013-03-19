#include "programa.h"

#ifndef _ESTRUCTURAS_H
#define ESTRUCTURAS

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

centro *crear_directorio(char *fichero_centros);

int agregar_directorio(centro** directorio, char *nombre_centro, char *hostname, int puerto);

void destruir_directorio(centro **directorio);

void crear_pase (pase pase, char *nombre, ticket ticket);
#endif
