#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include "programa.h"
#include "programa_svc.c"
#include "desafio.c"

int tiempo, inventario, capacidad, suministro;
char *nombre_centro;
int t_funcionamiento = 480;

int nro_ticket = 0;

char *log_file_name;
FILE *log_file;

pthread_mutex_t mtx;
pthread_t thread_inv, thread_func, thread_exit;
pthread_attr_t attr1, attr2, attr3;

void crear_ticket(ticket *t,char *bomba); 


// preguntar: funcion que recibe el nombre de la bomba quien llama al procedimiento
//            retorna este argumento encriptado con el algoritmo mD5
char **
preguntar_1_svc(char **argp, struct svc_req *rqstp)
{
  static char *result;
  result = (char *) malloc(32);
  MDString(*argp,result);
  return &(result);
}


// responder: funcion que recibe la pregunta y respuesta para generar un ticket en caso de 
//            haber pasado el desafio. La respuesta al desafio es el nombre de la bomba 
//            encriptado y concantenado con el nombre del centro encriptado
ticket *
responder_1_svc(desafio *argp, struct svc_req *rqstp)
{
  static ticket *result;
  result = (ticket *) malloc(sizeof(ticket));
  char *nombre_centro_enc = (char *) malloc(32);
  MDString(nombre_centro,nombre_centro_enc);
  
  char *true_rpta = (char *) malloc(64);
  sprintf(true_rpta,"%s%s",argp->pregunta,nombre_centro_enc);

  if(!strcmp(true_rpta,argp->respuesta))
    crear_ticket(result,argp->nombre_bomba);
  else
    return NULL;

  return result;
}

// pedir_gasolina: retorna el estado de solicitud de gasolina.
int *
pedir_gasolina_1_svc(ticket *argp, struct svc_req *rqstp)
{
  pthread_mutex_lock(&mtx);
  static int  result;

/*  if (!ticket_valido(argp)) {
    result = 2;
    return &result;
  }*/
  
  if (inventario < 38000)
    result = 0;
  else {
    fprintf(log_file, "Suministro:  %d minutos, %s, OK, %d.\n", 480 - t_funcionamiento, argp->nombre_bomba, inventario);
    inventario = inventario - 38000;
    result = 1;
  }
  pthread_mutex_unlock(&mtx);
  return &result;
}

// pedir_tiempo: retorna el tiempo de llegada de una gandola del centro
int *
pedir_tiempo_1_svc(ticket *argp, struct svc_req *rqstp)
{
  pthread_mutex_lock(&mtx);
  static int  result;
  result = tiempo;
  pthread_mutex_unlock(&mtx);
  return &result;
}

// crear_ticket: crea un ticket para determinada bomba
void crear_ticket(ticket *t,char *bomba) {
  t->nro_ticket = nro_ticket;
  nro_ticket++;
  t->nombre_centro = (char *) malloc(sizeof(strlen(nombre_centro)));
  strcpy(t->nombre_centro,nombre_centro);
  t->hora = t_funcionamiento;
  t->nombre_bomba=(char*) malloc(sizeof(strlen(bomba)));
  strcpy(t->nombre_bomba,bomba);
}

// ticket_valido: verifica si un ticket cumple con las condiciones de validacion
int ticket_valido(ticket *t) {
  return (t->nro_ticket <= nro_ticket) && 
         !(strcmp(t->nombre_centro,nombre_centro)) &&
         (5 <= 480 - t->hora && 480 - t->hora <= 60);
}

void *inventario_suministro(void * tid) {
  int suministro = (int) tid;
  while (TRUE) {
    //printf("%d lts.\n",inventario);
    usleep(100000);
    if(inventario == 0)
      fprintf(log_file,"Tanque vacio: %d minutos.\n",480 - t_funcionamiento);
    
    if(inventario + suministro < capacidad)
      inventario+=suministro;
    else if(inventario + suministro >= capacidad) {
      inventario = capacidad;
      fprintf(log_file,"Tanque full: %d minutos.\n",480 - t_funcionamiento);
    }

    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }
}

void *tiempo_funcionamiento(void * tid) {
  while (TRUE) {
    //printf("%d min. \n",t_funcionamiento);
    usleep(100000);
    t_funcionamiento--;
    if(!t_funcionamiento)
      pthread_exit(EXIT_SUCCESS);
  }
}

void *tiempo_exit(void *tid) {
  while(TRUE)
    if(t_funcionamiento == 0){
      kill(getpid(),SIGUSR1);
      pthread_exit(EXIT_SUCCESS);
    }
}

void finish() {
 pthread_attr_destroy(&attr1);
 pthread_attr_destroy(&attr2);
 pthread_attr_destroy(&attr3);
 void * status;
 pthread_join(thread_inv,&status);
 pthread_join(thread_func,&status);
 pthread_join(thread_exit,&status);
 fclose(log_file);
 exit(EXIT_SUCCESS);
}

static void print_use(){
  printf("uso: centro OPCIONES ...\n");
  printf("     OPCIONES:\n");
  printf("       -n  <nombre_centro>\n");
  printf("       -cp <capacidad_maxima>\n");
  printf("       -i  <inventario>\n");
  printf("       -t  <tiempo>\n");
  printf("       -s  <suministro>\n");
}

static void read_arg(char **argv,int argc, char **nombre_centro,int *capacidad,int *inventario,int *suministro,int *tiempo) {
  // getopt no parsea argumentos cuyo flag es mas de un caracter
  int cp = FALSE;
  {
   int j;
   for(j = 0 ; j < 6 ; j++)
     if(!strcmp(argv[2*j + 1],"-cp")) {
       cp = TRUE;
       *capacidad = atoi(argv[2*j+2]);
       break;
     }

  }

  if(!cp) {
    print_use();
    exit(EXIT_FAILURE);
  }
  else
    while(TRUE){
      int options = getopt(argc,argv,"n:i:t:s:p:c:");
      if (options == -1)
        break;
      switch(options) {
        case 'n':
          *nombre_centro = optarg;
          break;
        case 'i':
          *inventario = atoi(optarg);
          break;
        case 't':
          *tiempo = atoi(optarg);
          break;
        case 's':
          *suministro = atoi(optarg);
          break;
        case 'c':
          break;
        default:
          print_use();
          exit(EXIT_FAILURE);
      }
    }
}

int valid_arg(char *nombre_centro,int capacidad,int inventario,int suministro,int tiempo) {
  int valid = TRUE;
  if(strlen(nombre_centro) > 40) {
    printf("Error: Nombre del centro debe ser a lo sumo de 40 caracteres.\n");
    valid = FALSE;
  }
  if(capacidad < 38000 || capacidad > 3800000) {
    printf("La capacidad maxima debe ser un numero comprendido entre 38000 y 3800000.\n");
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
  if(suministro < 0) {
    printf("El valor de suministro debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(suministro > 10000) {
    printf("El valor de suministro no debe exceder a 10000.\n");
    valid = FALSE;
  }
  if(tiempo < 0) {
    printf("El valor del tiempo debe ser un numero positivo.\n");
    valid = FALSE;
  }
  if(tiempo > 180) {
    printf("El valor del tiempo no debe exceder a 180.\n");
    valid = FALSE;
  }
  return valid;
}


// auxiliar_main: main auxiliar para preparar el ambiente de simulacion del sistema
void auxiliar_main(int argc, char **argv) {
  if(argc != 11){
    print_use();
    exit(EXIT_FAILURE);
  }

  read_arg(argv,argc,&nombre_centro,&capacidad,&inventario,&suministro,&tiempo);

  if (!valid_arg(nombre_centro,capacidad,inventario,suministro,tiempo))
   exit(EXIT_FAILURE);

  log_file_name = (char *) malloc(strlen(nombre_centro)+8);
  sprintf(log_file_name,"log_%s.txt",nombre_centro);

  log_file = fopen(log_file_name,"w");
  if(!log_file) {
    printf("Error: no se pudo crear el archivo log el archivo\n.");
    exit(EXIT_FAILURE);
  }

  fprintf(log_file,"Inventario inicial: %d litros.\n",inventario);
  
  pthread_attr_init(&attr1);
  pthread_attr_setdetachstate(&attr1,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_inv, &attr1, inventario_suministro, (void *) suministro)) {
    printf("Error: no se pudo crear el hilo para controlar el inventario.");
    exit(EXIT_FAILURE);
  }

  pthread_attr_init(&attr2);
  pthread_attr_setdetachstate(&attr2,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_func, &attr2, tiempo_funcionamiento,NULL)) {
    printf("Error: no se pudo crear el hilo para controlar el funcionamiento.");
    exit(EXIT_FAILURE);
  }

  signal(SIGUSR1,finish);
 
  pthread_attr_init(&attr3);
  pthread_attr_setdetachstate(&attr3,PTHREAD_CREATE_JOINABLE);
  if (pthread_create(&thread_exit, &attr3, tiempo_exit,NULL)) {
    printf("Error: no se pudo crear el hilo para salida.");
    exit(EXIT_FAILURE);
  }
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (PROGRAMA_PROG, PROGRAMA_VER);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PROGRAMA_PROG, PROGRAMA_VER, programa_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PROGRAMA_PROG, PROGRAMA_VER, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PROGRAMA_PROG, PROGRAMA_VER, programa_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PROGRAMA_PROG, PROGRAMA_VER, tcp).");
		exit(1);
	}

        auxiliar_main(argc,argv);
	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
