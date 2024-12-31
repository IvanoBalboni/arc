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
    p->val = NULL;						\
  }								\

typedef enum {
  AST_LIRE = 252, AST_ECRIRE = 253, AST_RETOURNE = 254,
  AST_NB   = 255, AST_ID   = 256, AST_OP  = 257,
  AST_INST = 258, AST_DECL = 259, AST_AFF = 260,
  AST_SI   = 261, AST_TQ   = 262,
  AST_FCT  = 263, AST_APPEL_FCT = 264,
  AST_LIST = 265,
  AST_IDL  = 266, AST_DECL_IDL = 267, AST_AFF_IDL = 268,
  AST_PROGRAMME = 269, AST_PRE_MAIN = 270, AST_MAIN = 271,
  AST_ADR = 272
} ast_type;
//un ID peut avoir le meme nom qu'une liste / fonction.
//les 3 peuvent cohabiter et fonctionnent toutes correctement.

typedef enum {
  OP_PLUS = 512, OP_MOINS = 513, OP_MULT = 514, OP_DIV  = 515, OP_MOD = 516,
  OP_INF  = 517, OP_SINF  = 518, OP_SUP  = 519, OP_SSUP = 520,
  OP_EGAL = 521, OP_DIFF  = 522, OP_OU   = 523, OP_ET   = 524
}typeOP;


typedef struct{
  typeOP      type;
  struct ast* val[2];
} noeudOP;

typedef char variable[32];

typedef struct{
  variable       id;
  struct ast*    exp;
}affectation;

typedef struct{
  int            taille;
  struct ast*    val;
  struct ast*    suiv;
}Liste;

typedef struct{
  variable       id;
  int            taille;
  struct ast*    liste;
}decla_liste;

typedef struct{
  variable       id;
  struct ast*    pos;
  struct ast*    exp;
}affect_IDL;

typedef struct{
  struct ast*    val;
  struct ast*    suiv;
}instructions;

typedef struct{//stocke le memes donnees que instructions mais avec des noms differents
  struct ast*    exp;
  struct ast*    faire;
}tant_que;

typedef struct{
  struct ast*    exp;
  struct ast*    alors;
  struct ast*    sinon;
}condition;

typedef struct{
  variable       id;
  struct ast*    param;
  struct ast*    decl_liste;
  struct ast*    inst;
}decl_fonction;

typedef struct{
  int            nb_global;
  struct ast*    pre_main;
  struct ast*    main;
}programme;

typedef union valeur{
  int               nb;
  variable          id;
  affectation       decl;   //peut affecter dans la decl
  affectation       affect;
  affectation       IDL;  //id[exp] renvoie l'elem
  decla_liste       decl_liste;
  affect_IDL        affect_idl;  // peut faire id[a*5+2] <- exp
  Liste             liste;   // liste d'exp
  decl_fonction     algo;
  affectation       appel;  //id fonction, liste
  noeudOP           op;
  instructions      inst;
  condition         si;
  tant_que          tq;
  struct ast*       ecrire;
  struct ast*       retourne;
  programme         prog;
  programme         pre_main;
  instructions      main;
}valeur;



typedef struct ast{
  ast_type  type;
  char      type_str[32];
  int       codelen;
  valeur*   val;
} ast;


ast * CreerFeuilleNB(int nb);
ast * CreerFeuilleID(char* id);
ast * CreerFeuilleADR(char* id);

ast * CreerFeuilleLIRE();
ast * CreerFeuilleECRIRE(ast * exp);
ast * CreerFeuilleRETOURNE(ast* exp);

ast * CreerFeuilleDECLA(char* id, ast* exp);
ast * CreerFeuilleAFFECT(char* id, ast* exp);

ast * CreerFeuilleDECLA_IDL(char* id, int taille, ast* liste);
ast * CreerNoeudLISTE(ast * exp, ast * suiv);

ast * CreerFeuilleLIRE_IDL(char* id, ast* exp);
ast * CreerFeuilleAFFECT_IDL(char* id, ast* pos, ast* exp);

ast * CreerNoeudOP(typeOP op, ast* n1, ast* n2);
ast * CreerNoeudINSTRUCT(ast* inst, ast* suiv);

ast * CreerNoeudSI(ast* exp, ast* alors,  ast* sinon);
ast * CreerNoeudTQ(ast* exp, ast* faire);

ast * CreerFONCTION(char* id, ast* param, ast* decla, ast* inst);
ast * CreerAPPEL_FONCTION(char* id, ast* param);

ast * CreerNoeudPROGRAMME(ast* pre_main, ast* main);
ast * CreerNoeudPRE_MAIN(ast* decla, ast* inst);
ast * CreerNoeudMAIN(ast* decla, ast* inst);

void FreeAst(ast * p);

void PrintAst(ast * p, char* indent);
void ErrorAst(const char * errmsg);


#endif
