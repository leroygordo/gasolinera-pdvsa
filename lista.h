/* Archivo: lista.h
 *
 * Definicion de la estructura lista "LIST" y las firmas de las funciones 
 * que operan sobre el tipo LIST.
 * 
 * Autores: Hancel Gonzalez
 *          Octavio Manzano
 */

#ifndef _LISTA_H
#define LISTA

#include "preinas.h"

/* Definicion de estructuras */

struct box {
  long time;
  int times;
  int solucion[TAM_TABLERO];
  struct box *next;
};

struct list {
  struct box *l;
  int tam;
};

typedef struct box BOX;
typedef struct list LIST;

/* Firmas de las funciones */

void crear(LIST *lista);
void agregar(LIST *lista,int array[TAM_TABLERO],long t);
BOX *existe(LIST *lista, int array[TAM_TABLERO]);
void vaciar(LIST *lista);

#endif
