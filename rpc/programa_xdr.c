/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "programa.h"

bool_t
xdr_estructura (XDR *xdrs, estructura *objp)
{
	register int32_t *buf;

	 if (!xdr_int (xdrs, &objp->tiempo))
		 return FALSE;
	 if (!xdr_int (xdrs, &objp->inventario))
		 return FALSE;
	return TRUE;
}