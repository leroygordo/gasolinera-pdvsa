/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "pdvsa.h"

char **
preguntar_1_svc(char **argp, struct svc_req *rqstp)
{
	static char * result;

	/*
	 * insert server code here
	 */

	return &result;
}

ticket *
responder_1_svc(desafio *argp, struct svc_req *rqstp)
{
	static ticket  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
pedir_gasolina_1_svc(pase *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}

int *
pedir_tiempo_1_svc(pase *argp, struct svc_req *rqstp)
{
	static int  result;

	/*
	 * insert server code here
	 */

	return &result;
}
