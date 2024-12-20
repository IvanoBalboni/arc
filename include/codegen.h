#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "ts.h"


extern FILE *    exefile;
extern table_symb * TABLE_SYMBOLES;

enum{
    TEMP = 1, RET = 2, PILE = 3, PILE_APPEL = 4
};

#define DEBUT_GLOBAL 9

typedef enum CONTEXT{GLOBAL_VAR = 128, LOCAL_VAR = 129, FUNCTION = 130}CONTEXT;
//
#define EMPILER() PILE++    \

#define DEPILER() PILE--    \


int chercher(table_symb ts, CONTEXT);

//utilise la TS pour initialiser PILE apres les var globales & fonctions
void codegenInit();//jump @main
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
void EMPILER(char* comment);
void DEPILER(char* comment);
void genPrintVal(char* line, char* comment, int val);
#endif
