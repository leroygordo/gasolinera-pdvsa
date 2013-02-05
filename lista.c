/* Archivo: lista.c
 *
 * Implementacion de las funciones que operan sobre el tipo LIST
 * 
 * Autores: Hancel Gonzalez
 *          Octavio Manzano
 */

#include "lista.h"
#include <stdlib.h>
#include <stdio.h>

/* Funcion: crear
 * 
 * inicializa la lista. 
 * Nota: previamente fue hecha la reserva con malloc.
 *
 * lista: apuntador a una lista
 *
 */
void crear(LIST *lista) {
  
  lista->l = NULL;
  lista->tam = 0;

}

/* Funcion: agregar
 * 
 * agrega un elemento (arreglo de enteros) a la lista.
 *
 * lista: apuntador a una lista
 * array: arreglo de enteros de tamaño TAM_TABLERO, 
 *        almacena numeros  entre 0 y 7, usado para 
 *        ciertos fines.
 * t:     almacena un tiempo.
 *
 */
void agregar(LIST *lista,int array[TAM_TABLERO],long t) {
  
  BOX * nuevo;
  
  if(!(nuevo = (BOX *) malloc(sizeof(BOX)))) {
    perror("Error en reserva de memoria\n");
    exit(1);
  }
  
  int i;
  for(i = 0 ; i < TAM_TABLERO ; i++) nuevo->solucion[i] = array[i];

  nuevo->times = 1;
  nuevo->time = t;
  nuevo->next = lista->l;
  lista->l = nuevo;

  lista->tam++;

}

/* Funcion: existe
 *
 * verifica si existe, o no, un elemento en la lista.
 *
 * lista: apuntador a una lista
 * array: elemento que sera buscado en la lista.
 * 
 * retorna: apuntador al tipo BOX en caso de que el elemento
 *          se encuentre en la lista. En caso  contrario, se 
 *          retorna NULL.
 *
 */
BOX *existe(LIST *lista, int array[TAM_TABLERO]) {
  
  if(lista->tam == 0) return NULL;

  BOX *aux = lista->l;

  while(aux != NULL) {
    int i;
    int e = 0;
    for(i = 0 ; i < TAM_TABLERO ; i++) 
      if(array[i] == aux->solucion[i]) e++;
    
    if(e == TAM_TABLERO) return aux;

    aux = aux->next;
  }
  
  return NULL;
}

/* Funcion: vaciar
 * 
 * vacia la lista. Se libera la memoria reservada 
 * para sus elementos
 * 
 * lista: apuntador a la lista a vaciar.
 *
 */
void vaciar(LIST *lista) {
  BOX *aux;

  while(lista->l != NULL) {
    aux = lista->l;
    lista->l = aux->next;
    aux->next = NULL;
    free(aux);
    lista->tam--;
  }
}
