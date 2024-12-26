#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "ts.h"


extern FILE *       exefile;
extern table_symb * TABLE_SYMBOLES;
extern char         CONTEXTE[32];
extern char         CHERCHE_SYMB_GLOBAL;


enum{
    TEMP = 1, APPEL_FCT = 2, PILE = 3, LOCAL = 4, GLOBAL = 5, ADR_AFFECT = 6
};


//
#define EMPILER(adr, comment) genPrintVal("LOAD %-8d ; ", comment, adr);    \
  genPrintVal("STORE @%-6d ; ", "EMPILER\n", PILE);    \
  genPrintVal("INC %-9d ; ", "FIN EMPILER\n", PILE);  \

#define DEPILER(comment) genPrintVal("DEC %-9d ; ", "DEPILER\n", PILE);  \
  genPrintVal("LOAD @%-7d ; ", comment, PILE);    \

#define STOCKER(comment) genPrintVal("STORE %-7d ; ", comment, TEMP);


//utilise la TS pour initialiser PILE apres les var globales & fonctions
void codegenInit();//jump @main

void codegenInitMain();

void codegenInitCONTEXTE();

void codegenCHGT_PILE(int sortie_contexte);

//ecrit dans le fichier exefile les instructions machine RAM
void codegen(ast* p);

void codegenNB(ast* p);
void codegenID(ast* p);
void codegenOP(ast* p);
void codegenDECLA(ast* p);
void codegenAFFECT(ast* p);
void codegenINST(ast* p);
void codegenLIRE(ast* p);
void codegenRETOURNE(ast* p);
void codegenFIN();

//TODO empiler
void genPrintVal(char* line, char* comment, int val);
#endif
