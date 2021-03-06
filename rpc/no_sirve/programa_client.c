/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "programa.h"
#include "estructuras.h"

int inventario, capacidad,tiempoSuministro;
int t_funcionamiento = 480;
char *log_file_name;
FILE *log_file;

static void print_use(){
  printf("uso: bomba OPCIONES ...\n");
  printf("     OPCIONES:\n");
  printf("       -n  <nombre_bomba>\n");
  printf("       -cp <capacidad_maxima>\n");
  printf("       -i  <inventario>\n");
  printf("       -c  <consumo>\n");
  printf("       -fc  <fichero_centros>\n");
}

static void read_arg(char **argv, int *capacidad, char **fichero_centros, char **nombre_bomba, int *inventario, int *consumo) {
  int j;
  for(j = 0 ; j < 5 ; j++) {
    if(!strcmp(argv[2 * j + 1],"-cp")) {
      *capacidad = atoi(argv[2*j+2]);
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-fc")) {
      *fichero_centros = argv[2 * j + 2];
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-n")) {
      *nombre_bomba = argv[2 * j+2];
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-i")) {
      *inventario = atoi(argv[2 * j+2]);
      continue;
    }
    else if(!strcmp(argv[2 * j + 1],"-c")) {
      *consumo = atoi(argv[2 * j + 2]);
      continue;
    }
    else {
      print_use();
      exit(EXIT_FAILURE);
    }
  }
}

int valid_arg(int capacidad, char *fichero_centros, char *nombre_bomba, int inventario, int consumo) {
  int valid = TRUE;
  if(capacidad < 38000 || capacidad > 380000) {
    printf("La capacidad maxima debe ser un numero comprendido entre 38000 y 380000.\n");
    valid = FALSE;
  }
  if(inventario < 0) {
    printf("El inventario debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(inventario > capacidad) {
    printf("El inventario actual no debe exceder a la capacidad maxima.\n");
    valid = FALSE;
  }
  if(consumo < 0) {
    printf("El valor de consumo debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(consumo > 1000) {
    printf("El valor de consumo no debe exceder a 1000.\n");
    valid = FALSE;
  }
  return valid;
}

void *inventario_consumo(void * tid) {
int consumo = (int) tid;
while (TRUE) {
//printf("%d lts. ",inventario);
usleep(100000);
if(inventario == capacidad)
  fprintf(log_file,"Tanque full: %d minutos.\n",480 - t_funcionamiento);
if(inventario - consumo > 0)
  inventario-=consumo;
 else if(inventario - consumo <= 0) {
inventario = 0;
fprintf(log_file,"Tanque vacio: %d minutos.\n",480 - t_funcionamiento);
}
if(!t_funcionamiento)
  pthread_exit(EXIT_SUCCESS);
}

}

void *tiempo(void * tid) {
  while (TRUE) {
    //printf("%d min.\n",t_funcionamiento);
    usleep(100000);    
    t_funcionamiento--;
    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }
}
void
centroprog_1(char *host)
{
  CLIENT *clnt;
  char * *result_1;
  char * preguntar_1_arg;
  ticket  *result_2;
  desafio responder_1_arg;
  int  *result_3;
  ticket pedir_gasolina_1_arg;
  int  *result_4;
  ticket pedir_tiempo_1_arg;

#ifndef	DEBUG
  clnt = clnt_create (host, CENTROPROG, CENTRO_VER, "udp");
  if (clnt == NULL) {
    clnt_pcreateerror (host);
    exit (1);
  }
#endif	/* DEBUG */

  result_1 = preguntar_1(&preguntar_1_arg, clnt);
  if (result_1 == (char **) NULL) {
    clnt_perror (clnt, "call failed");
  }
  result_2 = responder_1(&responder_1_arg, clnt);
  if (result_2 == (ticket *) NULL) {
    clnt_perror (clnt, "call failed");
  }
  result_3 = pedir_gasolina_1(&pedir_gasolina_1_arg, clnt);
  if (result_3 == (int *) NULL) {
    clnt_perror (clnt, "call failed");
  }
  result_4 = pedir_tiempo_1(&pedir_tiempo_1_arg, clnt);
  if (result_4 == (int *) NULL) {
    clnt_perror (clnt, "call failed");
  }
#ifndef	DEBUG
  clnt_destroy (clnt);
#endif	 /* DEBUG */

}

int
main (int argc, char *argv[]) {

  CLIENT *clnt;
  char * *result_1;
  char * preguntar_1_arg;
  ticket  *result_2;
  desafio responder_1_arg;
  int  *result_3;
  ticket pedir_gasolina_1_arg;
  int  *result_4;
  ticket pedir_tiempo_1_arg;
  //ticket ticket;
  
  if(argc != 11) {
    print_use();
    exit(EXIT_FAILURE);
  }
 
  char *nombre_bomba, *fichero_centros;
  int consumo;
  
  // Lectura de argumentos
  read_arg(argv,&capacidad,&fichero_centros,&nombre_bomba,&inventario,&consumo);

  // Validacion de argumentos
  if (!valid_arg(capacidad,fichero_centros,nombre_bomba,inventario,consumo))
    exit(EXIT_FAILURE);

    //CLIENT *clnt;
    clnt = clnt_create ("localhost", CENTROPROG, CENTRO_VER, "udp");
    if (clnt == NULL) {
      clnt_pcreateerror ("localhost");
      exit (EXIT_FAILURE);
    }

  //printf(nombre_bomba);
  //char **p = preguntar_1(&nombre_bomba,clnt);
  int *t = pedir_tiempo_1(NULL,clnt);

  /*centro *directorio_centros = NULL;
 
  // Creacion del directorio con los centros de distribucion que se encuentran en 'fichero_centros' 
  if (!(directorio_centros = crear_directorio(fichero_centros,nombre_bomba))) {
    printf("Error: no se pudo crear el directorio de centros de distribucion.\n");
    exit(EXIT_FAILURE); 
  }
 
  log_file_name = (char *) malloc(strlen(nombre_bomba)+8);
  sprintf(log_file_name,"log_%s.txt",nombre_bomba);

  log_file = fopen(log_file_name,"w");
  if(!log_file) {
    printf("Error: no se pudo crear el archivo log\n.");
    exit(EXIT_FAILURE);
  }
 
  fprintf(log_file,"Inventario inicial: %d litros.\n",inventario);

  pthread_t thread_inv, thread_func;
  pthread_attr_t attr1, attr2;

  pthread_attr_init(&attr1);
  pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_inv, &attr1, inventario_consumo, (void *) consumo)) {
    printf("Error: no se pudo crear el hilo para controlar el inventario.");
    exit(EXIT_FAILURE);
  }

  pthread_attr_init(&attr2);
  pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_func, &attr2, tiempo,NULL)) {
    printf("Error: no se pudo crear el hilo para controlar el funcionamiento.");
    exit(EXIT_FAILURE);
  }
  centro c;
  c = *directorio_centros;
  while (t_funcionamiento > 0 ) {
  
    if (capacidad - inventario >= 38000) {
      // Si no tiene ticket
      
      // No se pudo hacerla conexion
   
      // Pudo conectarse
      	clnt = clnt_create (c.hostname, CENTROPROG, CENTRO_VER, "udp");
	if (clnt == NULL) {
	  clnt_pcreateerror (c.hostname);
	  exit (1);
	}
	crear_pase (nombre_bomba,pedir_gasolina_1_arg);
      result_1 = pedir_gasolina_1(&pedir_gasolina_1_arg, clnt);
      if (result_1 == (int *) NULL) {
	clnt_perror (clnt, "call failed");
      }
 
      if(*result_1 == 0) {
	if(c.next == NULL)
	  c = *directorio_centros;
	else
	  c = *c.next;

	fprintf(log_file,"Peticion: %d minutos, %s, Fallido.\n",480 - t_funcionamiento,c.nombre_centro);
      }
      else if (*result_1 == 1) {
	tiempoSuministro = c.send_t;
	fprintf(log_file,"Peticion: %d minutos, %s, OK.\n",480 - t_funcionamiento,c.nombre_centro);
	usleep(tiempoSuministro * 100000);
	if (inventario + 38000 >= capacidad) {
	  inventario = capacidad;
	}
	else {
	  inventario+=38000;
	}
	fprintf(log_file,"Llegada de la gandola: %d minutos, %d litros.\n",480 - t_funcionamiento,inventario); 
	c = *directorio_centros;
      }
    }
  }
  destruir_directorio(&directorio_centros);  
  pthread_attr_destroy(&attr1);
  pthread_attr_destroy(&attr2);
  void * status;
  pthread_join(thread_inv,&status);
  pthread_join(thread_func,&status);
  fclose(log_file);*/
   
  exit(EXIT_SUCCESS);
}
