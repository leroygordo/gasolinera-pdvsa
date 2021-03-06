/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "pdvsa.h"

bool_t
xdr_ticket (XDR *xdrs, ticket *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->nro_ticket))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->ip_centro))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->fecha, 8))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->hora, 6))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_desafio (XDR *xdrs, desafio *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->pregunta, 32))
		 return FALSE;
	 if (!xdr_string (xdrs, &objp->respuesta, 64))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_pase (XDR *xdrs, pase *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, &objp->nombre_bomba, 100))
		 return FALSE;
	 if (!xdr_ticket (xdrs, &objp->ticket_))
		 return FALSE;
	return TRUE;
}
