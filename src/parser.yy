%language "C++"
%defines
%define parse.trace
%locations

%{
#include "drums.h"
#include "environment.h"
#include "module.h"
#include "beat.h"
#include "pattern.h"

#include <string>

extern FILE *yyin;
Environment* env;

%}

%union {
	Beat* b;
	Module* m;
	Pattern* p;
	char* c;
	int i;
};

%token <c> STRING 
%token <i> NUMBER
%token ';' ':' '.' '{' '}'

%type <m> module_line module_lines
%type <b> beat pattern
%type <p> pattern_line pattern_lines
%type <i> tempo_line
%destructor { free($$); } <c>
%destructor { delete $$; } <m> <b> <p>

%{
extern int yylex(yy::parser::semantic_type *yylval, yy::parser::location_type* yylloc);
%}

%start file
%%

file		: define file				{}
		| define				{}
		;

define		: instrument				{}
       		| pattern_def				{}
		| module				{}
		;

instrument	: STRING NUMBER STRING ';'		{ if($2>255) env->add_instrument(std::string($1), new Instrument(255, std::string($3)));
	   							else env->add_instrument(std::string($1), new Instrument($2, std::string($3)));
								free($1); free($3);	}
	   	| error ';' 				{ error(@1, "Bad instrument definition");		}
	   	;

pattern_def	: STRING NUMBER '{' pattern_lines '}'	{ env->add_module(std::string($1), new Module($2, $4));	free($1);	}
      		;

pattern_lines	: pattern_line pattern_lines		{ $1->add($2); delete $2; $$=$1;	}
	   	| pattern_line				{ $$ = $1;				}
		;

pattern_line	: STRING pattern ';'			{ $$ = new Pattern(std::string($1), $2); free($1);	}
		| error ';'				{ error(@1, "Bad pattern line"); $$ = new Pattern();	}
	  	;

pattern		: beat pattern				{ $1->add($2); delete $2; $$=$1;	}
	 	| beat					{ $$ = $1;			}
		;

beat		: NUMBER				{ if($1>9) $$ = new Beat(9);
      								else $$ = new Beat($1);	}
      		| '.'					{ $$ = new Beat(0);	}
		;

module		: STRING '{' module_lines '}'		{ env->add_module(std::string($1), $3); free($1);	}
	    	| error	'}'				{ error(@1, "Bad Module");	}
		;

module_lines	: module_lines module_line		{ $1->add($2); delete $2; $$=$1;}
	     	| module_line				{ $$ = $1;			}
		;

module_line	: STRING NUMBER	';'			{ $$ = new Module(std::string($1), $2); free($1);	}
	    	| STRING ';'				{ $$ = new Module(std::string($1), 1); free($1);	}
		| error ';'				{ error(@1, "Bad module line"); $$ = new Module();	}
		| tempo_line				{ $$ = new Module($1);					}
		;

tempo_line	: NUMBER ';' 				{ $$ = $1;	}
		;

%%

void parse(const char* file_name, Environment* _env) {
	env=_env;
	FILE *file;
	try {
		file=fopen(file_name,"r");
	} catch (std::exception& e) {
		throw File_Not_Found();
	}
	if(!file) throw File_Not_Found();
	yyin=file;
	yy::parser parser;
	parser.parse();
}

namespace yy {
	void parser::error(const location_type& loc, const std::string& msg){
		 std::cerr << "Error at " << loc << ": " << msg << std::endl;
	}
}
