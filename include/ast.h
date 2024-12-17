#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  TXT_RED    "\x1b[31m"
#define  TXT_GREEN  "\x1b[32m"
#define  TXT_BLUE   "\x1b[34m"
#define  TXT_BOLD   "\x1b[1m"
#define  TXT_NULL   "\x1b[0m"

#define INIT_NOEUD(p)   if ((p = malloc(sizeof(ast))) == NULL)	\
    ErrorAst("echec allocation mémoire");			\
  else {							\
    p->type = 0;						\
    p->type_str[0] = '\0';					\
    p->valeur = NULL;						\
  }								\

enum {AST_NB = 256, AST_OP = 257} ;

typedef struct ast* noeudOP[2];

typedef struct{
  //TODO: changer pour gerer correctement les adresses
  int        nb;
  char       id[32];
}variable;

typedef union val{
  int         nb;
  noeudOP     op;
  variable    var;
}val;



typedef struct ast{
  int  type;
  char type_str[32];
  val  *valeur;
} ast;


ast * CreerFeuilleNB(int nb);
ast * CreerFeuilleDECLA(char* id);
ast * CreerFeuilleAFFECT(char* id, int nb);
ast * CreerNoeudOP(char op, ast* n1, ast* n2);

void FreeAst(ast * p);

void PrintAst(ast * p, char* indent);
void ErrorAst(const char * errmsg);


#endif
