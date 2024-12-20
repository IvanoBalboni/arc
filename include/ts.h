#ifndef TS_H
#define TS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

typedef enum{
  TS_ID  = 64, TS_L_ID = 65, TS_DECLA = 66, TS_AFFECT = 67,
  TS_FCT = 68
}Type;

typedef enum{//TODO: devient string pour gerer contexte fct?
  GLOBAL = 0, LOCAL = 1
}contexte;

typedef struct symbole{
  char     id[32];
  int      adr;
  Type     type;
  contexte ctxt;
} symbole;


typedef struct table_symb{
  symbole*            symb;
  struct table_symb * suiv;
}table_symb;

//LIFO
table_symb * TableSymbInit();

void AjouterSymb(table_symb * ts, symbole * symb);
symbole * CreerSymb(char* id, int adr, Type type, contexte ctxt);
//peut etre besoin de demander le ctxt + tard
symbole * RechercherSymb(table_symb * ts, char * id, Type type, contexte ctxt);

void printTS(table_symb * ts);
void ErrorTs(const char * errmsg);

#endif
