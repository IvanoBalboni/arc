#ifndef TS_H
#define TS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct symbole{
  char   id[32];
  int    type; 
  int    size;
  int    adr;
  struct symbole *next;
} symbole;

typedef struct contexte{
  char name[32];
  symbole * liste_symbole;
  struct contexte * next;
} contexte;

typedef struct contexte * table_symb;


#endif