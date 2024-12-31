#include "semantic.h"
#include "ast.h"
#include "ts.h"

int GLOBAL_ADR = 0;
int LOCAL_ADR = 0;

void semantic(ast * p){
  if (p == NULL) return;
  symbole * symb;
  int decl_len, instlen;
  switch(p->type){
    case AST_LIRE:
      p->codelen = 1;
    break;
    case AST_ECRIRE:
      semantic(p->val->ecrire);
      p->codelen = 1 + p->val->ecrire->codelen;
    break;
    case AST_RETOURNE:
      semantic(p->val->retourne);
      p->codelen = 2 + p->val->retourne->codelen;
    break;
    case AST_NB:
    p->codelen = 1;
    break;
    case AST_ID:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_AFFECT);
      if (symb == NULL)
        symb = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_ADR);
      if (symb == NULL)
        ErrorSemantic("ID Variable pas declaree");
      p->codelen = 4;
    break;
    case AST_ADR:
      symb = RechercherADR(TABLE_SYMBOLES, p->val->id);
      if (symb == NULL)
        ErrorSemantic("ID Adresse pas declaree");
      p->codelen = 2;
    break;
    case AST_OP:
      semantic(p->val->op.val[0]);
      semantic(p->val->op.val[1]);
      p->codelen = lenOP(p->val->op.type, p->val->op.val[0]->codelen, p->val->op.val[1]->codelen);
    break;
    case AST_INST:
      semantic(p->val->inst.val);
      p->codelen = p->val->inst.val->codelen;
      if (p->val->inst.suiv != NULL){
        semantic(p->val->inst.suiv);
        p->codelen += p->val->inst.suiv->codelen;
      }
      //printf("len inst %d\n", p->codelen);
    break;
    case AST_DECL:
      semanticDECL(p);
    break;
    case AST_AFF:
      semanticAFF(p);
    break;
    case AST_SI:
      semanticSI(p);
    break;
    case AST_TQ:
      semantic(p->val->tq.exp);
      semantic(p->val->tq.faire);
      p->codelen = 2 + p->val->tq.exp->codelen + p->val->tq.faire->codelen;
    break;
    case AST_FCT:
      semanticFCT(p);
    break;
    case AST_APPEL_FCT:
      semanticAPPL(p);
    break;
    case AST_LIST:
      semantic(p->val->liste.val);
      p->codelen = 3+ p->val->liste.val->codelen;//LEN EMPILER + LEN EXP
      //printf("liste %d\n",p->codelen);
      if (p->val->liste.suiv != NULL){
        semantic(p->val->liste.suiv);
        p->codelen += p->val->liste.suiv->codelen;
      }
    break;
    case AST_IDL:
      symb = RechercherADR(TABLE_SYMBOLES, p->val->id);
      if (symb == NULL)
        ErrorSemantic("IDL: Adresse pas declaree");
      semantic(p->val->IDL.exp);
      p->codelen = 6 + p->val->IDL.exp->codelen;
    break;
    case AST_DECL_IDL:
      semanticDECLIDL(p);
    break;
    case AST_AFF_IDL:
      symb = RechercherADR(TABLE_SYMBOLES, p->val->id);
      if (symb == NULL)
        ErrorSemantic("AFF IDL: Adresse pas declaree");
      semantic(p->val->affect_idl.exp);
      semantic(p->val->affect_idl.pos);
      p->codelen = 7 + p->val->affect_idl.exp->codelen + p->val->affect_idl.pos->codelen;
    break;
    case AST_PROGRAMME:
      semantic(p->val->prog.pre_main);
      p->val->prog.nb_global = GLOBAL_ADR;
      semantic(p->val->prog.main);
      if(p->val->prog.pre_main == NULL){
        p->val->prog.pre_main = CreerNoeudINSTRUCT(NULL, NULL);
        p->val->prog.pre_main->codelen = 0;
      }
      p->codelen = 4 + p->val->prog.pre_main->codelen + p->val->prog.main->codelen;
      printf("len prog: %d // preprog: %d main: %d\n",p->codelen, p->val->prog.pre_main->codelen, p->val->prog.main->codelen);
    break;
    case AST_PRE_MAIN:
      semantic(p->val->pre_main.pre_main);
      p->codelen = p->val->pre_main.pre_main->codelen;
      if (p->val->pre_main.main != NULL){
        semantic(p->val->pre_main.main);
        p->codelen += p->val->pre_main.main->codelen;
      }
    break;
    case AST_MAIN:
      setCONTEXTE("MAIN");
      semantic(p->val->main.val);
      semantic(p->val->main.suiv);
      decl_len = (p->val->main.val != NULL) ? p->val->main.val->codelen : 0;
      instlen = (p->val->main.suiv != NULL) ? p->val->main.suiv->codelen : 0;
      p->codelen = decl_len + instlen; 
      printf("len main: %d // decl: %d inst: %d \n", p->codelen, decl_len, instlen);
    break;
  }
}

void semanticDECL(ast * p){
  symbole * symb = RechercherSymb(TABLE_SYMBOLES, p->val->decl.id, TS_DECLA);
  if ( symb == NULL){
    if(strcmp(CONTEXTE, "GLOBAL") == 0){
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->decl.id, LOCAL_ADR, TS_DECLA));
      GLOBAL_ADR++;
    }else{
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->decl.id, LOCAL_ADR, TS_DECLA));
      LOCAL_ADR++;
    }
  }
  else
    ErrorSemantic("Variable deja declaree");
  p->codelen = 4;
  if(p->val->decl.exp != NULL){
    semantic(p->val->decl.exp);
    p->codelen += p->val->decl.exp->codelen;
  }

}

void semanticDECLIDL(ast * p){
  symbole * symb = RechercherSymb(TABLE_SYMBOLES, p->val->decl_liste.id, TS_IDL_DECLA);
  if ( symb == NULL){
    if(strcmp(CONTEXTE, "GLOBAL") == 0){
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->decl_liste.id, LOCAL_ADR, TS_IDL_DECLA));
      GLOBAL_ADR+= p->val->decl_liste.taille;
    }else{
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->decl_liste.id, LOCAL_ADR, TS_IDL_DECLA));
      LOCAL_ADR+= p->val->decl_liste.taille;
    }
  }
  else
    ErrorSemantic("DECL IDL: Variable deja declaree");
  if(p->val->decl_liste.liste != NULL){
    if (p->val->decl_liste.taille != p->val->decl_liste.liste->val->liste.taille)
      ErrorSemantic("DECL IDL: Liste de taille differente d'IDL.");
    semantic(p->val->decl_liste.liste);
    p->codelen = p->val->decl_liste.liste->codelen;
  }
  else
  p->codelen = 3;
}

void semanticAFF(ast * p){
  symbole * symb = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_DECLA);
  //printf("%s\n",CONTEXTE);
  if (symb == NULL)
    ErrorSemantic("AFF Variable pas declaree");
  else{
    AjouterSymb(TABLE_SYMBOLES, CreerSymb(symb->id, symb->adr, TS_AFFECT));
  }
  semantic(p->val->affect.exp);
  p->codelen = 4 + p->val->decl.exp->codelen;

}

void semanticSI(ast * p){
  int explen, alorslen, sinonlen;
  semantic(p->val->si.exp);
  semantic(p->val->si.alors);
  semantic(p->val->si.sinon);
  explen = (p->val->si.exp != NULL) ? p->val->si.exp->codelen : 0;
  alorslen = (p->val->si.alors != NULL) ? p->val->si.alors->codelen : 0;
  sinonlen = (p->val->si.sinon != NULL) ? p->val->si.sinon->codelen +1: 0;
  p->codelen = 1 + explen + alorslen + sinonlen;//TODO: bonne len
}

void semanticFCT(ast * p){
  char ajouttaille = 0;
  int plen, decl_len, instlen;
  ast * param = NULL;
  symbole * symb = RechercherSymb(TABLE_SYMBOLES, p->val->algo.id, TS_FCT_DECLA);
  if( symb != NULL)
    ErrorSemantic("FONCTION DEJA IMPLEMENTEE");
  symb = RechercherSymb(TABLE_SYMBOLES, p->val->algo.id, TS_FCT_PROTO);
  plen = p->val->algo.param->val->liste.taille;

  if (symb == NULL){
    AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->algo.id, GLOBAL_ADR, TS_FCT_PROTO));
    GLOBAL_ADR++;
    ajouttaille = 1;
    param = p->val->algo.param;
  }
  setCONTEXTE(p->val->algo.id);
  AjouterCTXT(TABLE_SYMBOLES, CONTEXTE);

  if (ajouttaille)
    AjouterSymb(TABLE_SYMBOLES, CreerSymb("param", plen, TS_FCT_PROTO));

  if (plen != RechercherSymb(TABLE_SYMBOLES, "param", TS_FCT_PROTO)->adr)
    ErrorSemantic("Decl FCT: Pas le meme nb. de parametres que le prototype.");
  
  while (param != NULL)
  {
    variable id;
    strcpy(id, param->val->liste.val->val->id);
    ast_type type = param->val->liste.val->type;
    if(type == AST_ID){
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(id , LOCAL_ADR, TS_DECLA));
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(id , LOCAL_ADR, TS_AFFECT));
    }
    else
      AjouterSymb(TABLE_SYMBOLES, CreerSymb(id , LOCAL_ADR, TS_ADR));
    LOCAL_ADR++; 
    param = param->val->liste.suiv;
  }
  semantic(p->val->algo.decl_liste);
  semantic(p->val->algo.inst);

  if (p->val->algo.inst != NULL)
    symb->type = TS_FCT_DECLA;
  
  decl_len = (p->val->algo.decl_liste != NULL) ? p->val->algo.decl_liste->codelen : 0;
  instlen = (p->val->algo.inst != NULL) ? p->val->algo.inst->codelen : 0;
  p->codelen = 25 + decl_len + instlen;
  LOCAL_ADR = 0;
  setCONTEXTE("GLOBAL");
}

void semanticAPPL(ast * p){
  char temp[32];
  strcpy(temp, CONTEXTE);
  symbole * symb = RechercherSymb(TABLE_SYMBOLES, p->val->appel.id, TS_FCT_DECLA);
  if( symb == NULL)
    symb = RechercherSymb(TABLE_SYMBOLES, p->val->appel.id, TS_FCT_PROTO);
  if( symb == NULL)
    ErrorSemantic("APPEL: Fonction non existante");

  setCONTEXTE(p->val->appel.id);
  ast * param = p->val->appel.exp;

  semantic(p->val->appel.exp);

  if (param->val->liste.taille != RechercherSymb(TABLE_SYMBOLES, "param", TS_FCT_PROTO)->adr)
    ErrorSemantic("APPEL: Pas le meme nb. de parametres que le prototype.");
  p->codelen = 14 + p->val->appel.exp->codelen;
}

int lenOP(typeOP op, int exp1, int exp2){
  switch (op)
  {
  case OP_PLUS:
  case OP_MOINS:
  case OP_MULT:
  case OP_DIV:
  case OP_MOD:
    return (7 + exp1 + exp2);
  case OP_SSUP:
  case OP_SINF:
  case OP_DIFF:
  case OP_EGAL:
  case OP_ET:
    return (11 + exp1 + exp2);
  case OP_SUP:
  case OP_INF:
  case OP_OU:
    return (12 + exp1 + exp2);
  default:
    break;
  }
  ErrorSemantic("type OP pas reconnu");
  return -50;
}




void ErrorSemantic(const char * errmsg){
  fprintf(stderr,"[SEMANTIC error] %s\n",errmsg);
  exit(1);
}
