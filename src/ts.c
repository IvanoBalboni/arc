#include "ts.h"
#include "ast.h"

int ADR_GLOBAL = 0;
int ADR_LOCAL  = 0;

table_symb * TableSymbInit(){
  table_symb * ts;
  if ((ts = malloc(sizeof(table_symb))) == NULL)
    ErrorTs("echec allocation mémoire");
/*
  symbole * s = malloc(sizeof(symbole));
  s->type  = TS_END;*/
  ts->symb = NULL;
  ts->suiv = NULL;
  return ts;
}

void AjouterSymb(table_symb * ts, symbole * symb){
  table_symb * suiv = ts;
  while (suiv->suiv != NULL) {
    suiv = suiv->suiv;
  }
  suiv->suiv = TableSymbInit();
  suiv->symb = symb;
}

symbole * CreerSymb(char* id, int adr, Type type, contexte ctxt){
  symbole * s = malloc(sizeof(symbole));
  strcpy(s->id, id);
  s->adr = adr;
  s->type = type;
  s->ctxt = ctxt;
  return s;
}

symbole * RechercherSymb(table_symb * ts, char * id, Type type, contexte ctxt){
  if (ts == NULL)
    return NULL;
  if (ts->suiv == NULL)
    return NULL;
  if (type != ts->symb->type)
    RechercherSymb(ts->suiv, id, type, ctxt);
  if (ctxt == LOCAL){
    //id == ts->symb.id
    if (strcmp(id,ts->symb->id) == 0){
      if (ts->symb->ctxt == LOCAL)
        return ts->symb;
      if (RechercherSymb(ts->suiv, id, type, ctxt) == NULL)
        return ts->symb;
    }
  }
  //id == ts->symb.id
  if (strcmp(id,ts->symb->id) == 0)
    return ts->symb;
  return RechercherSymb(ts->suiv, id, type, ctxt);
}

void printTS(table_symb * ts){
  if(ts->suiv == NULL)
    return;
  printf(TXT_BOLD "ID:  " TXT_NULL "%s ", ts->symb->id);
  printf(TXT_BOLD "ADR:  " TXT_NULL "%d ", ts->symb->adr);
  printf(TXT_BOLD "TYPE:  " TXT_NULL "<%d> ", ts->symb->type);
  printf(TXT_BOLD "CONTEXT:  " TXT_NULL "<%d>\n", ts->symb->ctxt);
  printTS(ts->suiv);
}

void ErrorTs(const char * errmsg){
  fprintf(stderr,"[TS error] %s\n",errmsg);
  exit(1);
}
