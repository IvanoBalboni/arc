#include "semantic.h"
#include "ast.h"
#include "ts.h"

int GLOBAL_ADR = 0;
int LOCAL_ADR = 0;

void semantic(ast * p){
  if (p == NULL) return;
  symbole * symb;
  switch(p->type){
    case AST_LIRE:
      p->codelen = 1;
    break;
    case AST_ECRIRE:
      semantic(p->val->ecrire);
      p->codelen = p->val->ecrire->codelen +1;
    break;
    case AST_RETOURNE:
      semantic(p->val->retourne);
      //TODO: a gerter avec les fct
      printf("CODELEN SEM 1");
    break;
    case AST_NB:
    p->codelen = 1;
    break;
    case AST_ID:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->id, TS_AFFECT);
      if (symb == NULL)
        ErrorSemantic("ID Variable pas declaree");
      p->codelen = 4;
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
    break;
    case AST_DECL:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->decl.id, TS_DECLA);
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
    break;
    case AST_AFF:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->affect.id, TS_DECLA);
      if (symb == NULL)
        ErrorSemantic("AFF Variable pas declaree");
      else{
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(symb->id, symb->adr, TS_AFFECT));
      }
      semantic(p->val->affect.exp);
      p->codelen = 4 + p->val->decl.exp->codelen;
    break;
    case AST_SI://TODO: codelen PARTOUT
      semantic(p->val->si.exp);
      semantic(p->val->si.alors);
      semantic(p->val->si.sinon);
    break;
    case AST_TQ:
      semantic(p->val->tq.exp);
      semantic(p->val->tq.faire);
      p->codelen = 2 + p->val->tq.exp->codelen + p->val->tq.faire->codelen;
    break;
    case AST_FCT:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->algo.id, TS_FCT_DECLA);
      GLOBAL_ADR++;
      setCONTEXTE(p->val->algo.id);
      AjouterCTXT(TABLE_SYMBOLES, CONTEXTE);
      ast * param = p->val->algo.param;
      while (param != NULL)
      {
        AjouterSymb(TABLE_SYMBOLES, CreerSymb(p->val->liste.val->val->id, LOCAL_ADR, TS_DECLA));
        LOCAL_ADR++; 
        param = param->val->liste.suiv;
      }
      semantic(p->val->algo.decl_liste);
      setCONTEXTE("GLOBAL");
    break;
    case AST_APPEL_FCT:
      symb = RechercherSymb(TABLE_SYMBOLES, p->val->appel.id, TS_FCT_DECLA);
      printf("A FAIRE CORRECTEMENT OUI\n");
    break;
    case AST_LIST:
    break;
    case AST_IDL:
    break;
    case AST_DECL_IDL:
    break;
    case AST_AFF_IDL:
    break;
  }
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
