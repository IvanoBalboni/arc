#ifndef TS_H
#define TS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

typedef enum{
  TS_ID  = 64, TS_DECLA  = 65, TS_AFFECT  = 66 ,
  TS_LID = 67, TS_LID_DECLA = 68, TS_LID_AFFECT = 69 ,
  TS_FCT = 70, TS_FCT_DECLA = 71,
  TS_FIN = 72
}Type;


typedef struct symbole{
  char     id[32];
  int      adr;
  Type     type;
} symbole;


typedef struct contexte{
  symbole*            symb;
  struct contexte *   suiv;
}contexte;

typedef struct table_symb{
  char                 contexte[32];
  contexte *           table_ctxt;
  struct table_symb *  suiv;
}table_symb;


symbole * CreerSymb(char* id, int adr, Type type);
//FIFO
contexte * contexteAddt(symbole * s);
//FIFO
table_symb * TableSymbInit();

void AjouterCTXT(table_symb * ts, char* ctxt);

void AjouterSymb(table_symb * ts, symbole * symb);

symbole * RechercherSymb(table_symb * ts, char * id, Type type);

void setCONTEXTE(char* ctxt);

void printTS(table_symb * ts);
void printCONTEXTE(contexte * tc);
void ErrorTs(const char * errmsg);

#endif
