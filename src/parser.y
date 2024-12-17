%{
  #include <stdio.h>
  #include <ctype.h>
  #include <string.h>

  #include "ast.h"

  extern int yylex();
  static void print_file_error(char * s, char *errmsg);

  struct ast * ARBRE_ABSTRAIT = NULL;

  struct ast * ARBRE_DECLARATION = NULL;

  void yyerror(const char * s);

  char srcname[64];
  char exename[64] = "a.out";
  FILE * exefile;
%}

%union{
  int         nb;
  char        var[32];
  struct ast* tree;
 };

%define parse.error detailed
%locations

%type <tree> PROGRAMME INSTRUCTIONS INST EXP DECLARATIONS

%token MAIN DEBUT FIN SEP AFFECT DECL
%token <nb> NB
%token <var> VAR
%start PROGRAMME

%left '+' '-'
%left '*' '/'

%%

PROGRAMME:
MAIN
DECLARATIONS
DEBUT
INSTRUCTIONS
FIN                             { $$ = $4; ARBRE_ABSTRAIT = $$; ARBRE_DECLARATION = $2;}

DECLARATIONS: DECL VAR SEP      { $$ = CreerFeuilleDECLA(yyval.var); ARBRE_DECLARATION = $$;}
|DECL VAR SEP DECLARATIONS      { $$ = CreerFeuilleDECLA(yyval.var); ARBRE_DECLARATION = $$;}
|                               { $$ = NULL;}


INSTRUCTIONS: INST SEP          { $$ = $1; }
| INST SEP INSTRUCTIONS         { $$ = $1; }

INST: EXP                       { $$ = $1; }
| AFFECTATION                   { $$ = $1; }

EXP : NB                        { $$ = CreerFeuilleNB(yyval.nb); }
| EXP '+' EXP                   { $$ = CreerNoeudOP('+', $1, $3); }
| EXP '-' EXP                   { $$ = CreerNoeudOP('-', $1, $3); }
| EXP '*' EXP                   { $$ = CreerNoeudOP('*', $1, $3); }
| EXP '/' EXP                   { $$ = CreerNoeudOP('/', $1, $3); }
;

AFFECTATION: VAR AFFECT NB      { $$ = CreerFeuilleAFFECT(yyval.var, yyval.nb); }

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
  exefile = fopen(exename,"w");
  yyparse();
  char indent[32] = "";
  PrintAst( ARBRE_ABSTRAIT, indent);
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
