#include "programa.h"

#ifndef _ESTRUCTURAS_H
#define ESTRUCTURAS

struct centro_servidor;
typedef struct centro_servidor centro;

struct centro_servidor {
  char *nombre_centro;
  char *hostname;
  int send_t;
  ticket *ticket_;
  centro *next;
};

centro *crear_directorio(char *fichero_centros, char *nombre_bomba);

int agregar_directorio(centro** directorio, char *nombre_centro, char *hostname, char* nombre_bomba);

void destruir_directorio(centro **directorio);

void crear_pase (char *nombre, ticket ticket);
#endif
