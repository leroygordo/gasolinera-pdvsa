#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "desafio.c"

int main (int argc, char** argv) {

unsigned char res[32];
char *st = "azul";

MDString(st,res);

printf(res);
printf("\n");

}
