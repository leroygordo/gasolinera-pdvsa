/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "pdvsa.h"
#include <stdio.h>
#include <stdlib.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif

static void
pdvsa_prog_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
	union {
		char *preguntar_1_arg;
		desafio responder_1_arg;
		ticket validar_1_arg;
		char *pedir_gasolina_1_arg;
	} argument;
	char *result;
	xdrproc_t _xdr_argument, _xdr_result;
	char *(*local)(char *, struct svc_req *);

	switch (rqstp->rq_proc) {
	case NULLPROC:
		(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
		return;

	case preguntar:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_wrapstring;
		local = (char *(*)(char *, struct svc_req *)) preguntar_1_svc;
		break;

	case responder:
		_xdr_argument = (xdrproc_t) xdr_desafio;
		_xdr_result = (xdrproc_t) xdr_ticket;
		local = (char *(*)(char *, struct svc_req *)) responder_1_svc;
		break;

	case validar:
		_xdr_argument = (xdrproc_t) xdr_ticket;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) validar_1_svc;
		break;

	case pedir_gasolina:
		_xdr_argument = (xdrproc_t) xdr_wrapstring;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) pedir_gasolina_1_svc;
		break;

	case pedir_tiempo:
		_xdr_argument = (xdrproc_t) xdr_void;
		_xdr_result = (xdrproc_t) xdr_int;
		local = (char *(*)(char *, struct svc_req *)) pedir_tiempo_1_svc;
		break;

	default:
		svcerr_noproc (transp);
		return;
	}
	memset ((char *)&argument, 0, sizeof (argument));
	if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		svcerr_decode (transp);
		return;
	}
	result = (*local)((char *)&argument, rqstp);
	if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
		svcerr_systemerr (transp);
	}
	if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
		fprintf (stderr, "%s", "unable to free arguments");
		exit (1);
	}
	return;
}

int
main (int argc, char **argv)
{
	register SVCXPRT *transp;

	pmap_unset (PDVSA_PROG, PDVSA_VER);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create udp service.");
		exit(1);
	}
	if (!svc_register(transp, PDVSA_PROG, PDVSA_VER, pdvsa_prog_1, IPPROTO_UDP)) {
		fprintf (stderr, "%s", "unable to register (PDVSA_PROG, PDVSA_VER, udp).");
		exit(1);
	}

	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		fprintf (stderr, "%s", "cannot create tcp service.");
		exit(1);
	}
	if (!svc_register(transp, PDVSA_PROG, PDVSA_VER, pdvsa_prog_1, IPPROTO_TCP)) {
		fprintf (stderr, "%s", "unable to register (PDVSA_PROG, PDVSA_VER, tcp).");
		exit(1);
	}

	svc_run ();
	fprintf (stderr, "%s", "svc_run returned");
	exit (1);
	/* NOTREACHED */
}
