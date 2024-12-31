#include "codegen.h"
#include "ast.h"
#include "ts.h"

int LEN  = 0;
//libere les variables en sortant du contexte (PILE - NB_VARIABLES)
int NB_VARIABLES = 0;
int ADR_PILE = 0;
int CONTEXTE_CHGT = 0;

void codegenInit(ast * p){
  genPrintVal("LOAD #%-7d ; ", "init l'adresse de GLOBAL\n", 9);
  genPrintVal("STORE %-7d ; ", "PILE <- ACC\n", GLOBAL);
  genPrintVal("LOAD #%-7d ; ", "init l'adresse de la pile\n", 9 /*+ p->val->prog.nb_global*/);
  genPrintVal("STORE %-7d ; ", "PILE <- ACC\n", PILE);
  ADR_PILE = 0;//relative, sert juste a savoir si fct deja declaree
}


void codegenInitCONTEXTE(char* ctxt){
  setCONTEXTE(ctxt);
  NB_VARIABLES = 0;
  char comment[64];
  sprintf(comment, "##DEBUT %s## // ACC <- PILE\n", ctxt);
  genPrintVal("LOAD %-8d ; ", comment, PILE);
  genPrintVal("STORE %-7d ; ", "LOCAL <- ACC\n", LOCAL);
}

void codegenSortieCONTEXTE(char* ctxt){
  char comment[64];
  sprintf(comment, "##FIN %s## // ACC <- ADRESSE PILE\n", CONTEXTE);
  genPrintVal("LOAD %-8d ; ", comment, PILE);
  genPrintVal("SUB #%-8d ; ","ACC <- ACC - NB Variables declarees& param\n", NB_VARIABLES);
  genPrintVal("STORE %-7d ; ", "Recule la PILE\n", PILE );
  setCONTEXTE(ctxt);
  NB_VARIABLES = 0;
}

void codegenCHGT_PILE(){
  genPrintVal("LOAD %-8d ; ", "ACC <- ADR PILE\n", PILE);
  genPrintVal("SUB #%-8d ; ","ACC <- ACC + ADRESSE PILE\n", NB_VARIABLES);
  genPrintVal("STORE %-7d ; ", "(FIN CHGT PILE) // Place LOCAL au debut des DECL & param\n", LOCAL );
}



void codegen(ast* p){
  if (p == NULL) return;
  if (CONTEXTE_CHGT){
    codegenCHGT_PILE();
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
    case AST_ADR:
      codegenADR(p);
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
      int i = (p->val->si.sinon != NULL);
      codegen(p->val->si.exp); // EXP DANS ACC
      genPrintVal("JUMZ %-8d ; ","(DEBUT SI) // JUMP a SINON si exp = 0\n",LEN +1+i + p->val->si.alors->codelen);
      codegen(p->val->si.alors);
      if (i)
        genPrintVal("JUMP %-8d ; ","(FIN SI) // JUMP APRES INST SINON\n",LEN + p->val->si.sinon->codelen);
      codegen(p->val->si.sinon);
    break;
    case AST_TQ:
      codegenTQ(p);
    break;
    case AST_FCT:
      codegenFCT(p);
    break;
    case AST_APPEL_FCT:
      codegenAPPEL_FCT(p);
    break;
    case AST_LIST:
      codegenLIST(p);
    break;
    case AST_IDL:
      codegenIDL(p);
    break;
    case AST_DECL_IDL:
      codegenDECL_IDL(p);
    break;
    case AST_AFF_IDL:
      codegenAFF_IDL(p);
    break;
    case AST_PROGRAMME:
      setCONTEXTE("GLOBAL");
      codegen(p->val->prog.pre_main);
      codegen(p->val->prog.main); 
    break;
    case AST_PRE_MAIN:
      codegen(p->val->pre_main.pre_main);
      if (p->val->pre_main.main != NULL)
        codegen(p->val->pre_main.main);
    break;
    case AST_MAIN:
      codegenInitCONTEXTE("MAIN");// 3 lignes
      genPrintVal("LOAD #%-7d ; ", "LIGNE FIN MAIN\n", LEN -3 + p->codelen);
      genPrintVal("STORE %-7d ; ", "LIGNE FIN MAIN\n", RETOUR_FCT);
      codegen(p->val->main.val);
        genPrintVal("LOAD %-8d ; ", "ACC <- ADR PILE\n", PILE);
        genPrintVal("ADD #%-8d ; ","ACC <- ACC + ADRESSE PILE\n", NB_VARIABLES);
        genPrintVal("STORE %-7d ; ", "(FIN CHGT PILE) // Place LOCAL au debut des DECL & param\n", PILE );
      //codegenCHGT_PILE();
      codegen(p->val->main.suiv); 
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
  if(s == NULL)
    s = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_ADR);
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

  sprintf(comment, "(FIN ID) // ACC <- %s\n",s->id);
  genPrintVal("LOAD @%-7d ; ", comment, TEMP );
}

void codegenADR(ast* p){
  symbole * s = RechercherADR(TABLE_SYMBOLES, p->val->id);
  char comment[64];
  if(CHERCHE_SYMB_GLOBAL){
    sprintf(comment, "(DEBUT ADR) // ACC <- adresse GLOBAL\n");
    genPrintVal("LOAD %-8d ; ", comment, GLOBAL );
  }
  else{
    sprintf(comment, "(DEBUT ADR) // ACC <- adresse LOCAL\n");
    genPrintVal("LOAD %-8d ; ", comment, LOCAL );
  }
  
  sprintf(comment, "(FIN ADR) // ACC + pos. de %s\n", p->val->id);
  genPrintVal("ADD #%-8d ; ", comment, s->adr);
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
      genPrintVal("JUML %-8d ; ","JUMP 3 lignes en dessous si < est vrai \n", LEN +3);
      genPrintVal("LOAD #%-7d ; ", "ACC <- 0 car < est faux \n", 0);
      genPrintVal("JUMP %-8d ; ","(FIN OP <) // JUMP 2 lignes en dessous\n", LEN +2);
      genPrintVal("LOAD #%-7d ; ", "(FIN OP <) // ACC <- 1 car < est vrai\n", 1);
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

  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->decl.id, TS_DECLA);
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

  if (p->val->decl.exp != NULL)
  {
    codegen(p->val->affect.exp->val->affect.exp);//calcul de l'expression a affecter (le resultat sera dans ACC)
  }

  sprintf(comment, "(FIN DECLA) // %s <- ACC\n",s->id);
  genPrintVal("STORE @%-6d ; ", comment, ADR_AFFECT );
  NB_VARIABLES++;
  ADR_PILE++;

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

void codegenFCT(ast* p){
  if (RechercherSymb(TABLE_SYMBOLES, p->val->algo.id, TS_FCT_PROTO) != NULL)
    ErrorTs("Semantique: FCT pas definie");
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->algo.id, TS_FCT_DECLA);
  char temp[32];
  strcpy(temp, CONTEXTE);//CONTEXTE actuel
  char comment[64];
  sprintf(comment, "##DECLA FCT: %s##\n", p->val->algo.id );

  if(s->adr == ADR_PILE){
    genPrintVal("LOAD #%-7d ; ", comment, LEN +6);
    STOCKER("LIGNE FCT\n");
    EMPILER(TEMP, "ADR FCT <- LIGNE FCT\n");// 3 lignes
    ADR_PILE++;
  }
  else{
    genPrintVal("LOAD %-8d ; ", comment, GLOBAL);
    genPrintVal("ADD #%-8d ; ", "ACC + ADR RELATIVE\n", s->adr);
    STOCKER("ADR FCT\n");
    genPrintVal("LOAD #%-7d ; ", "LIGNE FCT\n", LEN +3);
    genPrintVal("STORE @%-6d ; ", "ADR FCT <- LIGNE APPEL FCT\n", TEMP);
  }

  genPrintVal("JUMP %-8d ; ","##FIN DECLA FCT## // JUMP Apres instructions FONCTION\n", LEN - 5 + p->codelen);
  genPrintVal("LOAD #%-7d ; ", "LIGNE RETOUR\n", LEN -29 + 7 + p->codelen);
  genPrintVal("STORE %-7d ; ", "RETOUR_FCT <- LIGNE RETOUR\n", RETOUR_FCT);
  genPrintVal("LOAD #%-7d ; ", "VALEUR RETOUR PAR DEFAUT\n", -999);
  genPrintVal("STORE %-7d ; ", "VALEUR RETOUR <- DEFAUT\n", VALEUR_RETOUR);

  codegenInitCONTEXTE(p->val->algo.id);// DEBUT FCT, 2 lignes
  NB_VARIABLES = p->val->algo.param->val->liste.taille;

  codegen(p->val->algo.decl_liste);
  codegenCHGT_PILE();//3 lignes

  codegen(p->val->algo.inst);
  //DEBUT REOUR 12 lignes

  codegenSortieCONTEXTE(temp);//3 lignes
  DEPILER("ACC <- LOCAL avant appel FCT\n");//2 lignes
  genPrintVal("STORE %-7d ; ", "LOCAL <- ACC\n", LOCAL);
  DEPILER("ACC <- RETOUR_FCT appel FCT\n");//2 lignes
  genPrintVal("STORE %-7d ; ", "REOUR_FCT <- ACC\n", RETOUR_FCT);
  DEPILER("ACC <- LIGNE A JUMP APRES RETOUR FCT\n");//2 lignes
  STOCKER("TEMP <- LIGNE A JUMP APRES RETOUR FCT\n");

  genPrintVal("LOAD %-8d ; ", "VALEUR RETOUR PAR DEFAUT\n", VALEUR_RETOUR);

  genPrintVal("JUMP @%-7d ; ","(RETOUR FCT) // JUMP A l'appel de FCT\n", 0);
}

void codegenAPPEL_FCT(ast* p){
  symbole * s = RechercherSymb(TABLE_SYMBOLES, p->val->appel.id, TS_FCT_DECLA);
  genPrintVal("LOAD #%-7d ; ", "(DEBUT APPEL) // ACC <- ADR RETOUR APRES APPEL\n", LEN + p->codelen);
  EMPILER(0, "\n");//3 lignes
  EMPILER(RETOUR_FCT, "ACC <- LIGNE FIN INST DE FCT\n");
  EMPILER(LOCAL, "ACC <- ADR DEBUT LOCAL\n");//3 lignes
  codegen(p->val->appel.exp);
  genPrintVal("LOAD %-8d ; ", "ACC <- ADR DEBUT GLOBAL\n", GLOBAL);
  genPrintVal("ADD #%-8d ; ", "ACC + ADR RELATIVE FCT\n", s->adr);
  genPrintVal("LOAD @%-7d ; ", "ACC <- ADR LIGNE FCT\n", 0);

  genPrintVal("JUMP @%-7d ; ", "(FIN APPEL FCT) // JUMP au debut de la FCT\n",0);
}

void codegenLIST(ast* p){
  codegen(p->val->liste.val);
  EMPILER(0,"ACC = EXP de liste a empiler\n");
  codegen(p->val->liste.suiv);
}

void codegenIDL(ast* p){
  symbole * s = RechercherADR(TABLE_SYMBOLES, p->val->id);
  char comment[64];
  codegen(p->val->IDL.exp);
  STOCKER("(DEBUT ID[POS] <-) // TEMP <- POS\n");
  if(CHERCHE_SYMB_GLOBAL){
    sprintf(comment, "(DEBUT IDL) // ACC + adresse GLOBAL\n");
    genPrintVal("LOAD %-8d ; ", comment, GLOBAL );
  }
  else{
    sprintf(comment, "(DEBUT IDL) // ACC + adresse LOCAL\n");
    genPrintVal("LOAD %-8d ; ", comment, LOCAL );
  }
  
  sprintf(comment, "ACC + position relative de %s\n", s->id);
  genPrintVal("ADD #%-8d ; ", comment, s->adr);

  sprintf(comment, "ACC + position de  %s\n",s->id);
  genPrintVal("LOAD @%-7d ; ", comment, 0);

  genPrintVal("ADD %-9d ; ", "ACC <- ADR ID[POS]\n", TEMP);

  genPrintVal("LOAD @%-7d ; ","(FIN IDL) // ACC ID [POS]\n", 0);
}

void codegenDECL_IDL(ast* p){

  if (p->val->decl_liste.liste != NULL)
    codegen(p->val->decl_liste.liste);
  else{
    genPrintVal("LOAD #%-7d ; ", "(INIT LISTE VIDE) // ACC <- TAILLE LISTE\n", p->val->decl_liste.taille);
    genPrintVal("ADD %-9d ; ", "ACC <- ACC + PILE\n", PILE);
    genPrintVal("STORE %-7d ; ", "PILE <- ACC\n", PILE);
  }
  ADR_PILE += p->val->decl_liste.taille;
}

void codegenAFF_IDL(ast* p){
  symbole * s = RechercherADR(TABLE_SYMBOLES, p->val->affect_idl.id);
  char comment[64];
  codegen(p->val->affect_idl.pos);
  STOCKER("(DEBUT ID[POS] <-) // TEMP <- POS\n");

  if(CHERCHE_SYMB_GLOBAL){
    genPrintVal("LOAD %-8d ; ", "ACC <- ADRESSE GLOBAL\n", GLOBAL );
  }
  else{
    genPrintVal("LOAD %-8d ; ", "ACC <- ADRESSE LOCAL\n", LOCAL );
  }
  sprintf(comment, "ACC + position relative de  %s\n",s->id);
  genPrintVal("ADD #%-8d ; ", comment, s->adr );

  sprintf(comment, "ACC + position de  %s\n",s->id);
  genPrintVal("LOAD @%-7d ; ", comment, 0);
  
  genPrintVal("ADD %-9d ; ", "ACC <- ADR ID[POS]\n", TEMP);
  
  genPrintVal("STORE %-7d ; ", "ADR_AFFECT <- ADR ID[POS]\n", ADR_AFFECT);
  
  codegen(p->val->affect_idl.exp);
  genPrintVal("STORE @%-6d ; ", "(FIN ID[POS] <-) // ID[POS] <- ACC\n", ADR_AFFECT);
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
  codegen(p->val->retourne);
  genPrintVal("STORE %-7d ; ", " (DEBUT RETURN) // RETOUR_FCT <- LIGNE RETOUR\n", VALEUR_RETOUR);
  genPrintVal("JUMP %-8d ; "," JUMP Apres les instructions de FONCTION\n", RETOUR_FCT);
}


void codegenFIN(){
  LEN++;
  fprintf(exefile, "STOP" );
}

void genPrintVal(char* line, char* comment, int val){
  LEN++;
  fprintf(exefile, line, val);
  fprintf(exefile, comment, val);
}
