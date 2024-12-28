#include "codegen.h"
#include "ast.h"
#include "ts.h"

int LEN  = 0;
//libere les variables en sortant du contexte (PILE - NB_VARIABLES)
//
int NB_VARIABLES = 0;

int CONTEXTE_CHGT = 1;

void codegenInit(){
  //l'adresse de la pile sera connue une fois sorti de GLOBAL et modifie a ce moment >> besoin de fopen "w+"
  genPrintVal("LOAD #%-7d ; ", "init l'adresse de la pile\n", -20);
  genPrintVal("STORE %-7d ; ", "PILE <- ACC\n", PILE);
  //l'adresse de main() sera connue une fois au debut de MAIN et modifie a ce moment
  genPrintVal("JUMP %-8d ; ", "JUMP a la 1ere instruction de MAIN()\n", -20);

  //TODO: PILE LEN VARIBLES etc. tec.
}

void codegenInitMAIN(){
  CONTEXTE_CHGT = 0;
  char temp[128];

  if (fseek(exefile, 6, SEEK_SET) != 0)
    printf("CONDEGEN INIT MAIN NON \n#\n#\n");
  fprintf(exefile, "%-6d", NB_VARIABLES + 8);
  for (size_t i = 0; i < 2; i++){//passe la ligne courante et la suivante
    fscanf(exefile,"%[^\n]",temp);
    fscanf(exefile,"%c",temp);
    temp[0] = '\0';
  }
  
  if (fseek(exefile, 5, SEEK_CUR) != 0)
    printf("CONDEGEN INIT MAIN NON \n#\n#\n");
  fprintf(exefile, "%-8d", LEN);

  fseek(exefile, 0, SEEK_END);
  genPrintVal("LOAD %-8d ; ", "(DEBUT MAIN) // ACC <- PILE\n", PILE);
  genPrintVal("STORE %-7d ; ", "LOCAL <- ACC\n", LOCAL);

}

void codegenInitCONTEXTE(){

}

void codegenCHGT_PILE(int sortie_contexte){
  if (sortie_contexte){

  }else{
    genPrintVal("LOAD #%-7d ; ", "ACC <- NB Variabls declarees\n", NB_VARIABLES);
    genPrintVal("ADD %-9d ; "," ACC <- ACC + ADRESSE PILE\n", PILE);
  genPrintVal("STORE %-7d ; ", "Avance la PILE du nombre de DECL\n", PILE );
  }
}

void codegen(ast* p){
  if (p == NULL) return;
  if (CONTEXTE_CHGT){
    if (strcmp(CONTEXTE, "MAIN") == 0)
      codegenInitMAIN();
    else
      codegenInitCONTEXTE();
  }

    switch(p->type){
    case AST_LIRE:
      codegenLIRE(p);
    break;
    case AST_ECRIRE:
      codegenECRIRE(p);
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
    case AST_SI:
    break;
    case AST_TQ:
      codegenTQ(p);
    break;
    case AST_FCT:
    break;
    case AST_APPEL_FCT:
    break;
    case AST_LIST:
    break;
    case AST_IDL:
    break;
    case AST_DECL_IDL:
    break;
    case AST_AFF_IDL:
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
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_AFFECT);
  if(s == NULL){
    fprintf(stderr,"codegen ID, symbole non affecte.");
    exit(1);
  }

  char comment[64];
  if(CHERCHE_SYMB_GLOBAL){
    sprintf(comment, "(DEBUT ID) // ACC <- adresse GLOBAL\n");
    genPrintVal("LOAD %-8d ; ", comment, GLOBAL );
  }
  else{
    sprintf(comment, "(DEBUT ID) // ACC <- adresse LOCAL\n");
    genPrintVal("LOAD %-8d ; ", comment, LOCAL );
  }
  sprintf(comment, "ACC + position relative de  %s\n",s->id);
  genPrintVal("ADD #%-8d ; ", comment, s->adr );

  STOCKER("TEMP <- ACC (ID)\n");

  sprintf(comment, "ACC <- %s\n",s->id);
  genPrintVal("LOAD @%-7d ; ", comment, TEMP );
}

void codegenOP(ast* p){
  /*
   * Operateurs associatifs à gauche, on génère d'abord la partie droite,
   * on l'empile et on génère la partie gauche puis on execute l'operation
   * et stocke le resultat dans l'acc.
  */
  codegen(p->val->op.val[0]);
  EMPILER(0,"(DEBUT OP) // EMPILE EXP1 de l'op : EXP1 OP EXP2\n");
  //TODO: temp
  //TODO: STORE 1
  codegen(p->val->op.val[1]);
  STOCKER("TEMP <- ACC (EXP2)\n");
  DEPILER("EXP1\n");

  switch (p->val->op.type) {
    case OP_PLUS:
      genPrintVal("ADD %-9d ; ","(FIN OP +) // ACC <- ACC (EXP1) + TEMP (EXP2)\n", TEMP);
    break;
    case OP_MOINS:
      genPrintVal("SUB %-9d ; ","(FIN OP -) // ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
    break;
    case OP_MULT:
      genPrintVal("MUL %-9d ; ","(FIN OP *) // ACC <- ACC (EXP1) * TEMP (EXP2)\n", TEMP);
    break;
    case OP_DIV:
      genPrintVal("DIV %-9d ; ","(FIN OP /) // ACC <- ACC (EXP1) / TEMP (EXP2)\n", TEMP);
    break;
    case OP_MOD:
      genPrintVal("MOD %-9d ; ","(FIN OP %%) // ACC <- ACC (EXP1) %% TEMP (EXP2)\n", TEMP);
    break;
    case OP_INF:
      genPrintVal("SUB %-9d ; ","(DEBUT OP <=) // ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
      genPrintVal("DEC %-9d ; ","ACC <- ACC - 1\n", 0);
      genPrintVal("JUML %-8d ; ","JUMP 3 lignes en dessous si <= est vrai \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car <= est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP <=) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP <=) // ACC <- 1 car <= est vrai\n", 1);
    break;
    case OP_SINF:
      genPrintVal("SUB %-9d ; ","(DEBUT OP <) // ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
      genPrintVal("JUML %-8d ; ","JUMP 3 lignes en dessous si <= est vrai \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car < est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP <=) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP <=) // ACC <- 1 car < est vrai\n", 1);
    break;
    case OP_SUP:
      genPrintVal("SUB %-9d ; ","(DEBUT OP >=) // ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
      genPrintVal("DEC %-9d ; ","ACC <- ACC - 1\n", 0);
      genPrintVal("JUMG %-8d ; ","JUMP 3 lignes en dessous si >= est vrai \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car >= est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP >=) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP >=) // ACC <- 1 car >= est vrai\n", 1);
    break;
    case OP_SSUP:
      genPrintVal("SUB %-9d ; ","(DEBUT OP >) // ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
      genPrintVal("JUMG %-8d ; ","JUMP 3 lignes en dessous si > est vrai \n", LEN +4);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car > est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP >) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP >) // ACC <- 1 car >  est vrai\n", 1);
    break;
    case OP_EGAL:
      genPrintVal("SUB %-9d ; ","(DEBUT OP =) // ACC <- ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
      genPrintVal("JUMZ %-8d ; ","JUMP 3 lignes en dessous si = est vrai \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car = est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP =) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP =) // ACC <- 1 car = est vrai\n", 1);
    break;
    case OP_DIFF:
      genPrintVal("SUB %-9d ; ","(DEBUT OP !=) // ACC <- ACC <- ACC (EXP1) - TEMP (EXP2)\n", TEMP);
      genPrintVal("JUMZ %-8d ; ","JUMP 3 lignes en dessous si != est faux \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 1 car != est vrai \n", 1);
      genPrintVal("JUMP %-8d ; ","(FIN OP !=) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP !=) // ACC <- 0 car != est faux\n", 0);
    break;
    case OP_OU:
      genPrintVal("JUMZ %-8d ; ","(DEBUT OP OU) // JUMP 3 lignes en dessous si EXP1 est faux \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 1 car OU est vrai \n", 1);
      genPrintVal("JUMP %-8d ; ","(FIN OP OU) // JUMP 5 lignes en dessous\n", LEN +5);
      genPrintVal("LOAD %-8d ; ", "ACC <- TEMP (EXP2)\n", TEMP);
      genPrintVal("JUMZ %-8d ; ","JUMP 2 lignes en dessous si EXP2 est faux \n", LEN +2);
      genPrintVal("JUMP %-8d ; ","JUMP 4 lignes au dessus si EXP2 est vrai\n", LEN -3);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP OU) // ACC <- 0 car OU est faux \n", 0);
    break;
    case OP_ET:
      genPrintVal("JUMZ %-8d ; ","(DEBUT OP ET) // JUMP 5 lignes en dessous si EXP1 est faux \n", LEN +5);
      genPrintVal("LOAD %-8d ; ", "ACC <- TEMP (EXP2)\n", TEMP);
      genPrintVal("JUMZ %-8d ; ","JUMP 3 lignes en dessous si EXP2 est faux \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 1 car ET est vrai \n", 1);
      genPrintVal("JUMP %-8d ; ","(FIN OP ET) // JUMP 2 lignes au dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP ET) // ACC <- 0 car ET est faux \n", 0);
    break;
    default:
      fprintf(stderr,"[Erreur] type <%d>: %s non reconnu\n",p->type,p->type_str);
    break;
  }

}

void codegenDECLA(ast* p){

  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_DECLA);
  char comment[64];

  if (strcmp(CONTEXTE, "GLOBAL") == 0){
    sprintf(comment, "(DEBUT DECLA) // ACC <- adresse GLOBAL\n");
    genPrintVal("LOAD %-8d ; ", comment, GLOBAL );
  }
  else{
    sprintf(comment, "(DEBUT DECLA) // ACC <- adresse LOCAL\n");
    genPrintVal("LOAD %-8d ; ", comment, LOCAL );
  }
  sprintf(comment, "ACC + position relative de  %s\n",s->id);
  genPrintVal("ADD #%-8d ; ", comment, s->adr );

  sprintf(comment, "stocke l'adresse de %s\n",s->id);
  genPrintVal("STORE %-7d ; ", comment, ADR_AFFECT );

  codegen(p->val->affect.exp);//calcul de l'expression a affecter (le resultat sera dans ACC)

  sprintf(comment, "(FIN DECLA) // %s <- ACC\n",s->id);
  genPrintVal("STORE @%-6d ; ", comment, ADR_AFFECT );
  NB_VARIABLES++;

}

void codegenAFFECT(ast* p){
  
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_AFFECT);
  char comment[64];

  if (strcmp(CONTEXTE, "GLOBAL") == 0){
    sprintf(comment, "(DEBUT AFFECT) // ACC <- adresse GLOBAL\n");
    genPrintVal("LOAD %-8d ; ", comment, GLOBAL );
  }
  else{
    sprintf(comment, "(DEBUT AFFECT) // ACC <- adresse LOCAL\n");
    genPrintVal("LOAD %-8d ; ", comment, LOCAL );
  }
  sprintf(comment, "ACC - position relative de  %s\n",s->id);
  genPrintVal("ADD #%-8d ; ", comment, s->adr );

  sprintf(comment, "stocke l'adresse de %s\n",s->id);
  genPrintVal("STORE %-7d ; ", comment, ADR_AFFECT );

  codegen(p->val->affect.exp);//calcul de l'expression a affecter (le resultat sera dans ACC)

  sprintf(comment, "(FIN AFFECT) // %s <- ACC\n",s->id);
  genPrintVal("STORE @%-6d ; ", comment, ADR_AFFECT );

}

void codegenINST(ast* p){
  codegen(p->val->inst.val);
}

void codegenTQ(ast* p){
  codegen(p->val->tq.exp);
  genPrintVal("JUMZ %-8d ; ", "(TQ) // SI EXP TQ = 0 -> FTQ\n", LEN + p->val->tq.faire->codelen +2);
  codegen(p->val->tq.faire); 
  genPrintVal("JUMP %-8d ; ", "(TQ) // JUMP DEBUT TQ\n", LEN - p->val->tq.faire->codelen -p->val->tq.exp->codelen -1);
}


void codegenLIRE(ast* p){
  LEN++;
  fprintf(exefile, "READ          ; ACC <- ENTREE[I++]\n");
}


void codegenECRIRE(ast* p){
  codegen(p->val->ecrire);
  LEN++;
  fprintf(exefile, "WRITE         ; SORTIE[I++] <- ACC\n" );
}

void codegenRETOURNE(ast* p){
  printf("CMARCHE {PASJSIOJSD} TODO\n");
  codegen(p->val->retourne);
  LEN++;
  fprintf(exefile, "WRITE         ; ACC <- SORTIE[I++]\n" );
}


void codegenFIN(){
  LEN++;
  fprintf(exefile, "STOP\n" );
}

void genPrintVal(char* line, char* comment, int val){
  LEN++;
  fprintf(exefile, line, val);
  fprintf(exefile, comment, val);
}
