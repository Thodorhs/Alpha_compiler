#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <queue> 
#include <sstream>


extern int yylineno;
extern int yyleng;
extern char* yytext;
extern FILE* yyin;
extern FILE* yyout;
extern void* data;
extern int do_nothing;
extern size_t line;
extern size_t no;
extern int comment;
extern int linetmp;
extern int wrong_comment;
extern int wrong_string;
extern char* tempstring;
extern int instring;



enum alpha_token{
    UNKNOWN1=-1,
    KEYWORD1=2,
    OPERATOR1,
    INTCONST1,
    REALCONST1,
    STRING1,
    CURLY_BRACKET1,
    SQUARE_BRACKET1,
    PARENTHESIS1,
    SEMICOLON1,
    COLON1,
    COMMA1,
    DOUBLE_COLON1,
    DOT1,
    DOUBLE_DOT1,
    IDENT1,
    COMMENT1,
    NESTED_COMMENT1,
    NOT_CLOSED_STRING1,
    NOT_CLOSED_COMMENT1
};
typedef enum alpha_token TYPE;

struct alpha_token_t{
    size_t line;
    size_t no;
    std::string text;
    enum alpha_token type;
    void* value;
};

int alpha_yylex (void* ylval,int res);
void print_token(alpha_token_t* token);
char* replace_escapes(const char* str);
void print_tokens();