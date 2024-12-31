#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "ts.h"


extern FILE *       exefile;
extern table_symb * TABLE_SYMBOLES;
extern char         CONTEXTE[32];
extern char         CHERCHE_SYMB_GLOBAL;


enum{
    TEMP = 1, PILE = 2, GLOBAL = 3, LOCAL = 4, ADR_AFFECT = 5, RETOUR_FCT = 6, VALEUR_RETOUR = 7
};


//
#define EMPILER(adr, comment) genPrintVal("LOAD %-8d ; ", comment, adr);    \
  genPrintVal("STORE @%-6d ; ", "EMPILER\n", PILE);    \
  genPrintVal("INC %-9d ; ", "FIN EMPILER\n", PILE);  \

#define DEPILER(comment) genPrintVal("DEC %-9d ; ", "DEPILER\n", PILE);  \
  genPrintVal("LOAD @%-7d ; ", comment, PILE);    \

#define STOCKER(comment) genPrintVal("STORE %-7d ; ", comment, TEMP);


void codegenInit(ast * p);

void codegenInitCONTEXTE(char* ctxt);
void codegenSortieCONTEXTE(char* ctxt);
void codegenCHGT_PILE();

//ecrit dans le fichier exefile les instructions machine RAM
void codegen(ast* p);

void codegenNB(ast* p);
void codegenID(ast* p);
void codegenADR(ast* p);
void codegenOP(ast* p);
void codegenDECLA(ast* p);
void codegenAFFECT(ast* p);
void codegenINST(ast* p);
//void codegenSI(ast* p);
void codegenTQ(ast* p);
void codegenFCT(ast* p);
void codegenAPPEL_FCT(ast* p);
void codegenLIST(ast* p);
void codegenIDL(ast* p);
void codegenDECL_IDL(ast* p);
void codegenAFF_IDL(ast* p);
void codegenLIRE(ast* p);
void codegenECRIRE(ast* p);
void codegenRETOURNE(ast* p);
void codegenFIN();

void genPrintVal(char* line, char* comment, int val);
#endif
