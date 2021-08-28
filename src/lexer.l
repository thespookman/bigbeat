%option noyywrap nounput case-insensitive
%{
#include <cstdlib>
#include <vector>

#include "drums.h"
#include "parser.tab.hh"

#define YY_DECL int yylex(yy::parser::semantic_type *yylval, yy::parser::location_type *yylloc)

std::vector<FILE*> files;
std::vector<int> lines;

int yycolumn=1;

#define YY_USER_ACTION yylloc->initialize(NULL, yylineno, yycolumn); yylloc->columns(yyleng); yycolumn += yyleng;

typedef yy::parser::token token;

void open_file (const char* file_name);
bool end_file();

%}

%x FILENAME

%%

%{
yylloc->step();
%}

"//".*				|
[ \t]				{ yylloc->step(); 	}

"."				|
"{"				|
"}"				|
";"				{ return yytext[0]; 	}

\n				{ ++yylineno; yycolumn=0; BEGIN(0); 	}

[a-z_][a-z0-9_]*		{ yylval->c = strdup(yytext); return token::STRING;	}
("./"|"../")[^;\n]*		{ yylval->c = strdup(yytext); return token::STRING;	}
[0-9]+				{ yylval->i = atoi(yytext); return token::NUMBER;	}

include[ \t]+			{ BEGIN(FILENAME);		}
<FILENAME>.*			{ open_file(yytext); BEGIN(0);	}
<FILENAME>\n			{ BEGIN(0); ++yylineno; yycolumn=0;		}


.				{ yylloc->step();	}

<<EOF>>				{ if(end_file()) return 0; else BEGIN(0); }

%%

void open_file (const char* file_name) {
	FILE *file;
	try {
		file = fopen(file_name, "r");
	} catch (std::exception& e) {
		std::cerr << file_name << " not found. Ignoring." << std::endl;
		return;
	}
	if(!file) {
		std::cerr << file_name << " not found. Ignoring." << std::endl;
		return;
	}
	files.push_back(yyin);
	lines.push_back(yylineno);
	yylineno=0;
	yycolumn=0;
	yyin=file;
}

bool end_file () {
	if(files.size()==0) return true;
	fclose(yyin);
	yyin=files.back();
	files.pop_back();
	yylineno=lines.back();
	lines.pop_back();
	return false;
}
