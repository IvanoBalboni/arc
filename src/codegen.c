#include "codegen.h"
#include "ast.h"
#include "ts.h"

int LEN  = 0;

void codegen_init(){
  //TODO: PILE LEN VARIBLES etc. tec.
}

void codegen(ast* p){
  if (p == NULL) return;
  switch(p->type){
  case AST_LIRE:
  codegenLIRE(p);
  break;
  case AST_RETOURNE:
    codegenRETOURNE(p);
  break;
  case AST_NB:
    codegenNB(p);
  break;
  case AST_ID:
    codegenID(p);
  break;
  case AST_OP:
    codegenOP(p);
  break;
  case AST_AFF:
    codegenAFFECT(p);
  break;
  case AST_DECL:
    codegenDECLA(p);
  break;
  case AST_INST:
    codegenINST(p);
    if(p->val->inst.suiv != NULL)
      codegen(p->val->inst.suiv);
  break;
  default:
    fprintf(stderr,"[Erreur] type <%d>: %s non reconnu\n",p->type,p->type_str);
  break;
  }

}

void codegenNB(ast* p){
  char comment[64];
  sprintf(comment, "ACC <- %d\n", p->val->nb);
  genPrintVal("LOAD #%-7d ; ", comment, p->val->nb);
}

void codegenID(ast* p){
  //TODO: ts match
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_ID, LOCAL );
  char comment[64];
  sprintf(comment, "ACC <- valeur de %s\n", p->val->id);
  genPrintVal("LOAD %-8d ; ", comment, VARIABLES + s->adr);
}

void codegenOP(ast* p){
  /*
   * Operateurs associatifs à gauche, on génère d'abord la partie droite,
   * on l'empile et on génère la partie gauche puis on execute l'operation
   * et stocke le resultat dans l'acc.
  */
  codegen(p->val->op.val[1]);
  //TODO: temp
  //TODO: STORE 1 
  genPrintVal("STORE %-7d ; ","EMPILER( ACC )\n", PILE);
    EMPILER();
  codegen(p->val->op.val[0]);

  switch (p->val->op.type) {
    case OP_PLUS:
      DEPILER();
      genPrintVal("ADD %-9d ; ","ACC <- ACC + DEPILER( )\n", PILE);
    break;
    case OP_MOINS:
      DEPILER();
      genPrintVal("SUB %-9d ; ","ACC <- ACC - DEPILER( )\n", PILE);
    break;
    case OP_MULT:
      DEPILER();
      genPrintVal("MUL %-9d ; ","ACC <- ACC * DEPILER( )\n", PILE);
    break;
    case OP_DIV:
      DEPILER();
      genPrintVal("DIV %-9d ; ","ACC <- ACC / DEPILER( )\n", PILE);
    break;
    case OP_MOD:
      DEPILER();
      genPrintVal("MOD %-9d ; ","ACC <- ACC % DEPILER( )\n", PILE);
    break;
    case OP_INF://TODO:
      DEPILER();
      genPrintVal("SUB %-9d ; ","(DEBUT OP <=) // ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("DEC %-9d ; ","ACC <- ACC - 1\n", 0);
      genPrintVal("JUML %-8d ; ","JUMP 3 lignes en dessous si <= est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car <= est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP <=) // JUMP 2 lignes en dessous\n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP <=) // ACC <- 1 car <= est vrai\n", 1);
    break;
    case OP_SINF:
      DEPILER();
      genPrintVal("SUB %-9d ; ","(DEBUT OP <) // ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("JUML %-8d ; ","JUMP 3 lignes en dessous si <= est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car < est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP <=) // JUMP 2 lignes en dessous\n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP <=) // ACC <- 1 car < est vrai\n", 1);
    break;
    case OP_SUP:
      DEPILER();
      genPrintVal("SUB %-9d ; ","(DEBUT OP >=) // ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("DEC %-9d ; ","ACC <- ACC - 1\n", 0);
      genPrintVal("JUMG %-8d ; ","JUMP 3 lignes en dessous si >= est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car >= est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP >=) // JUMP 2 lignes en dessous\n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP >=) // ACC <- 1 car >= est vrai\n", 1);
    break;
    case OP_SSUP:
      DEPILER();
      genPrintVal("SUB %-9d ; ","(DEBUT OP >) // ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("JUMG %-8d ; ","JUMP 3 lignes en dessous si > est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car > est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP >) // JUMP 2 lignes en dessous\n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP >) // ACC <- 1 car >  est vrai\n", 1);
    break;
    case OP_EGAL:
      DEPILER();
      genPrintVal("SUB %-9d ; ","(DEBUT OP =) // ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("JUMZ %-8d ; ","JUMP 3 lignes en dessous si = est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car = est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP =) // JUMP 2 lignes en dessous\n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP =) // ACC <- 1 car = est vrai\n", 1);
    break;
    case OP_DIFF:
      DEPILER();
      genPrintVal("SUB %-9d ; ","(DEBUT OP !=) // ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("JUMZ %-8d ; ","JUMP 3 lignes en dessous si != est faux \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 1 car != est vrai \n", 1);
      genPrintVal("JUMP %-8d ; ","(FIN OP !=) // JUMP 2 lignes en dessous\n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP !=) // ACC <- 0 car != est faux\n", 0);
    break;
    case OP_OU:
      genPrintVal("JUMZ %-8d ; ","(DEBUT OP OU) // JUMP 3 lignes en dessous si GAUCHE OU est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 1 car OU est vrai \n", 1);
      genPrintVal("JUMP %-8d ; ","(FIN OP OU) // JUMP 5 lignes en dessous\n", LEN +6);
      DEPILER();
      genPrintVal("LOAD %-8d ; ", "ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("JUMZ %-8d ; ","JUMP 2 lignes en dessous si OU est faux \n", LEN +3);
      genPrintVal("JUMP %-8d ; ","JUMP 4 lignes au dessus si OU DROITE est vrai\n", LEN -3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP OU) // ACC <- 0 car OU est faux \n", 0);
    break;
    case OP_ET:
      genPrintVal("JUMZ %-8d ; ","(DEBUT OP ET) // JUMP 5 lignes en dessous si GAUCHE ET est faux \n", LEN +6);
      DEPILER();
      genPrintVal("LOAD %-8d ; ", "ACC <- ACC - DEPILER( )\n", PILE);
      genPrintVal("JUMZ %-8d ; ","JUMP 3 lignes en dessous si ET est faux \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 1 car ET est vrai \n", 1);
      genPrintVal("JUMP %-8d ; ","(FIN OP ET) // JUMP 2 lignes au dessous\n", LEN -3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP ET) // ACC <- 0 car OU est faux \n", 0);
    break;
    default:
      fprintf(stderr,"[Erreur] type <%d>: %s non reconnu\n",p->type,p->type_str);
    break;
  }

}

void codegenDECLA(ast* p){
  //TODO: + propre
  //var locale seulement
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->decl.id, TS_ID, LOCAL );
  //codegenAFFECT(p->val->decl.exp);
  char comment[64];
  sprintf(comment, "init %s sur l'adresse %d <- ACC\n", p->val->id, VARIABLES + s->adr);
  genPrintVal("STORE %-7d ; ", comment, VARIABLES + s->adr );
}

void codegenAFFECT(ast* p){
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_ID, LOCAL );
  codegen(p->val->affect.exp);
  char comment[64];
  sprintf(comment, "%s <- ACC\n", p->val->id);
  genPrintVal("STORE %-7d ; ", comment, VARIABLES + s->adr );
}

void codegenINST(ast* p){
  codegen(p->val->inst.val);
}

void codegenLIRE(ast* p){
  LEN++;
  fprintf(exefile, "READ          ; ACC <- ENTREE[I++]\n");
}
void codegenRETOURNE(ast* p){
  codegen(p->val->retourne);
  LEN++;
  fprintf(exefile, "WRITE         ; ACC <- ENTREE[I++]\n" );
}


void codegenFIN(){
  LEN++;
  fprintf(exefile, "STOP\n" );
}

void EMPILER()

void genPrintVal(char* line, char* comment, int val){
  LEN++;
  fprintf(exefile, line, val);
  fprintf(exefile, comment, val);
}
