/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#ifndef _PROGRAMA_H_RPCGEN
#define _PROGRAMA_H_RPCGEN

#include <rpc/rpc.h>


#ifdef __cplusplus
extern "C" {
#endif

struct ticket {
	int nro_ticket;
	int ip_centro;
	char *fecha;
	char *hora;
};
typedef struct ticket ticket;

struct desafio {
	char *pregunta;
	char *respuesta;
};
typedef struct desafio desafio;

struct pase {
	char *nombre_bomba;
	ticket ticket_;
};
typedef struct pase pase;

#define CENTROPROG 0x31111111
#define CENTRO_VER 1

#if defined(__STDC__) || defined(__cplusplus)
#define preguntar 1
extern  char ** preguntar_1(char **, CLIENT *);
extern  char ** preguntar_1_svc(char **, struct svc_req *);
#define responder 2
extern  ticket * responder_1(desafio *, CLIENT *);
extern  ticket * responder_1_svc(desafio *, struct svc_req *);
#define pedir_gasolina 3
extern  int * pedir_gasolina_1(pase *, CLIENT *);
extern  int * pedir_gasolina_1_svc(pase *, struct svc_req *);
#define pedir_tiempo 4
extern  int * pedir_tiempo_1(pase *, CLIENT *);
extern  int * pedir_tiempo_1_svc(pase *, struct svc_req *);
extern int pdvsa_prog_1_freeresult (SVCXPRT *, xdrproc_t, caddr_t);
	
#else /* K&R C */
#define preguntar 1
extern  char ** preguntar_1();
extern  char ** preguntar_1_svc();
#define responder 2
extern  ticket * responder_1();
extern  ticket * responder_1_svc();
#define validar 3
extern  int * validar_1();
extern  int * validar_1_svc();
#define pedir_gasolina 4
extern  int * pedir_gasolina_1();
extern  int * pedir_gasolina_1_svc();
#define pedir_tiempo 5
extern  int * pedir_tiempo_1();
extern  int * pedir_tiempo_1_svc();
extern int pdvsa_prog_1_freeresult ();
#endif /* K&R C */

/* the xdr functions */

#if defined(__STDC__) || defined(__cplusplus)
extern  bool_t xdr_ticket (XDR *, ticket*);
extern  bool_t xdr_desafio (XDR *, desafio*);
extern  bool_t xdr_pase (XDR *, pase*);

#else /* K&R C */
extern bool_t xdr_ticket ();
extern bool_t xdr_desafio ();
extern bool_t xdr_pase ();

#endif /* K&R C */

#ifdef __cplusplus
}
#endif

#endif /* !_PROGRAMA_H_RPCGEN */
