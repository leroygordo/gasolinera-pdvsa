/*
 * Modified by: Hancel Gonzalez
 *              Susana Charara
 */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "global.h"
#include "md5.h"

static void MDString PROTO_LIST ((char *)(unsigned char[16]));
static void MDPrint PROTO_LIST ((unsigned char [16])(unsigned char[16]));

#define MD5_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final

/* Digests a string and store the result.
 */
static void MDString (string,result)
char *string, result[16];
{
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int len = strlen (string);

  MDInit (&context);
  MDUpdate (&context, string, len);
  MDFinal (digest, &context);
  
  MDPrint (digest,result);
}

/* Store a message digest in hexadecimal.
 */
static void MDPrint (digest,result)
unsigned char digest[16], result[32];
{
  bzero(result,16);
  unsigned int i;
  unsigned char tmp[2];
  for (i = 0; i < 16; i++) {
    sprintf (&result[i*2],"%02x", digest[i]);
  }
}
