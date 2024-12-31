%{
  #include <stdio.h>
  #include <ctype.h>
  #include <string.h>

  #include "ast.h"
  #include "ts.h"
  #include "codegen.h"
  #include "semantic.h"

  extern int yylex();
  static void print_file_error(char * s, char *errmsg);

  struct ast * ARBRE_PRE_MAIN = NULL;
  struct ast * ARBRE_ABSTRAIT = NULL;
  struct ast * ARBRE_DECLARATION = NULL;

  table_symb * TABLE_SYMBOLES = NULL;

  void yyerror(const char * s);

  char srcname[64];
  char exename[64] = "a.out";
  FILE * exefile;
%}

%union{
  int         nb;
  char        id[32];
  struct ast* tree;
 };

%define parse.error detailed
%locations

%type <tree> PROGRAMME PRE_MAIN FONCTION INSTRUCTIONS EXP DECLARATIONS
%type <tree> INST DECLA L_ID LISTE AFFECTATION L_PARAM

%token MAIN ALGO LIRE ECRIRE RETOURNE VAR DEBUT FIN SEP AFFECT ',' '@'
%token TQ FAIRE FTQ
%token SI SINON FSI
%token <nb> NB
%token <id> ID
%start PROGRAMME

%left  OU
%left  ET
%left  EGAL DIFF
%left  INF '<' SUP '>'
%left  '+' '-'
%left  '*' '/' '%'
%token '(' ')' '[' ']'

%%

PROGRAMME:
PRE_MAIN
MAIN
DECLARATIONS
DEBUT
INSTRUCTIONS
FIN                             { $$ = CreerNoeudPROGRAMME($1, CreerNoeudMAIN($3, $5) );
                                  ARBRE_ABSTRAIT = $$;
                                }
;

PRE_MAIN: %empty                { $$ = NULL;}
| VAR DECLA SEP PRE_MAIN        { $$ = CreerNoeudPRE_MAIN($2, $4); }
| FONCTION PRE_MAIN             { $$ = CreerNoeudPRE_MAIN($1, $2); }
;

FONCTION:
ALGO ID '(' L_PARAM ')'
DECLARATIONS
DEBUT
INSTRUCTIONS
FIN                             { $$ = CreerFONCTION($2, $4, $6, $8);}
| ALGO ID '(' L_PARAM ')' SEP   { $$ = CreerFONCTION($2, $4, NULL, NULL);}
;

L_PARAM: %empty                 { $$ = NULL; }
|L_ID                           { $$ = $1;   }
;

L_ID: ID                        { $$ = CreerNoeudLISTE( CreerFeuilleID(yyval.id) , NULL); }
| '@' ID                        { $$ = CreerNoeudLISTE( CreerFeuilleADR($2) , NULL); }
| ID ',' L_ID                   { $$ = CreerNoeudLISTE( CreerFeuilleID($1) , $3);}
| '@' ID ',' L_ID               { $$ = CreerNoeudLISTE( CreerFeuilleADR($2) , $4); }
;

LISTE: EXP                      { $$ = CreerNoeudLISTE($1, NULL); }
| EXP ',' LISTE                 { $$ = CreerNoeudLISTE($1, $3);   }
;

DECLARATIONS: %empty            { $$ = NULL;}
| VAR DECLA SEP DECLARATIONS    { $$ = CreerNoeudINSTRUCT($2, $4);}
//TODO: VAR ID DECLA_LISTE SEP DECLARATIONS
;

DECLA: ID AFFECT EXP            { $$ = CreerFeuilleDECLA(yyval.id, $3); }
| ID                            { $$ = CreerFeuilleDECLA(yyval.id, NULL);}
| ID '[' NB ']'                 { $$ = CreerFeuilleDECLA_IDL(yyval.id, $3, NULL);}
| ID '[' NB ']' AFFECT '[' LISTE ']'{ $$ = CreerFeuilleDECLA_IDL(yyval.id, $3, $7);}
;

INSTRUCTIONS: INST              { $$ = CreerNoeudINSTRUCT($1, NULL); }
| INST INSTRUCTIONS             { $$ = CreerNoeudINSTRUCT($1, $2);   }
;

INST: EXP SEP                   { $$ = $1; }
| ECRIRE EXP SEP                { $$ = CreerFeuilleECRIRE($2); }
| RETOURNE EXP SEP              { $$ = CreerFeuilleRETOURNE($2); }
| VAR DECLA SEP                 { $$ = $2; }
| AFFECTATION SEP               { $$ = $1; }
| SI EXP FAIRE INSTRUCTIONS FSI { $$ = CreerNoeudSI($2, $4, NULL);}
| TQ EXP FAIRE INSTRUCTIONS FTQ { $$ = CreerNoeudTQ($2, $4);}
| SI EXP FAIRE INSTRUCTIONS SINON INSTRUCTIONS FSI { $$ = CreerNoeudSI($2, $4, $6);}
;


EXP : NB                        { $$ = CreerFeuilleNB(yyval.nb); }
| ID                            { $$ = CreerFeuilleID(yyval.id); }
| '@' ID                        { $$ = CreerFeuilleADR($2); }
| ID '[' EXP ']'                { $$ = CreerFeuilleLIRE_IDL(yyval.id, $3); }
| ID '(' LISTE ')'              { $$ = CreerAPPEL_FONCTION(yyval.id, $3); }
| EXP '+' EXP                   { $$ = CreerNoeudOP(OP_PLUS,  $1, $3); }
| EXP '-' EXP                   { $$ = CreerNoeudOP(OP_MOINS, $1, $3); }
| EXP '*' EXP                   { $$ = CreerNoeudOP(OP_MULT,  $1, $3); }
| EXP '/' EXP                   { $$ = CreerNoeudOP(OP_DIV,   $1, $3); }
| EXP '%' EXP                   { $$ = CreerNoeudOP(OP_MOD,   $1, $3); }
| EXP INF EXP                   { $$ = CreerNoeudOP(OP_INF,   $1, $3); }
| EXP '<' EXP                   { $$ = CreerNoeudOP(OP_SINF,  $1, $3); }
| EXP SUP EXP                   { $$ = CreerNoeudOP(OP_SUP,   $1, $3); }
| EXP '>' EXP                   { $$ = CreerNoeudOP(OP_SSUP,  $1, $3); }
| EXP EGAL EXP                  { $$ = CreerNoeudOP(OP_EGAL,  $1, $3); }
| EXP DIFF EXP                  { $$ = CreerNoeudOP(OP_DIFF,  $1, $3); }
| EXP OU EXP                    { $$ = CreerNoeudOP(OP_OU,    $1, $3); }
| EXP ET EXP                    { $$ = CreerNoeudOP(OP_ET,    $1, $3); }
| '(' EXP ')'                   { $$ = $2; }
| LIRE                          { $$ = CreerFeuilleLIRE(); }
;

AFFECTATION: ID AFFECT EXP     { $$ = CreerFeuilleAFFECT(yyval.id, $3); }
| ID '[' EXP ']' AFFECT EXP    { $$ = CreerFeuilleAFFECT_IDL(yyval.id, $3, $6);}
;

%%

int main( int argc, char * argv[] ) {
  //pour compiler example1 : ./bin/arc test/exemple1.algo a.out
  extern FILE *yyin;

  if (argc > 1){
    strcpy(srcname, argv[1]);
    if ( (yyin = fopen(srcname,"r"))==NULL ){
      char errmsg[256];
      sprintf(errmsg,"fichier \x1b[1m\x1b[33m' %s '\x1b[0m introuvable",srcname);
      print_file_error(argv[0],errmsg);
      exit(1);
    }
  }  else {
    print_file_error(argv[0],"aucun fichier en entree");
    exit(1);
  }
  if (argc == 3){
    strcpy(exename, argv[2]);
  }
  exefile = fopen(exename,"w+");
  yyparse();
  char indent[32] = "";
  PrintAst( ARBRE_ABSTRAIT, indent);

  TABLE_SYMBOLES = TableSymbInit();
  printf("##\n");

  semantic(ARBRE_ABSTRAIT);
  printf("##\n");

  printTS(TABLE_SYMBOLES);
  printf("debut codegen: \n" );

  codegenInit(ARBRE_ABSTRAIT);

  codegen(ARBRE_ABSTRAIT);
  codegenFIN();
  fclose(yyin);
}



static void print_file_error(char * prog, char *errmsg){
  fprintf(stderr,
	  "\x1b[1m%s:\x1b[0m \x1b[31m\x1b[1merreur fatale:\x1b[0m %s\nechec de la compilation\n",
	  prog, errmsg);
}

void yyerror(const char * s)
{
  fprintf(stderr, "\x1b[1m%s:%d:%d:\x1b[0m %s\n", srcname, yylloc.first_line, yylloc.first_column, s);
  exit(0);
}
