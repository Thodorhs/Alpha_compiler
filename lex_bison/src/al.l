
%{
#include "al.hpp"
#include "parser.hpp"
int linestart;
char *tempstring;
int instring=0;
std::string to_string(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

//convert char* to upper case
char* to_upper(char* str)
{
    for(int i = 0; str[i] != '\0'; i++)
    {
        str[i] = toupper(str[i]);
    }
    return str;
}
std::string temp=std::string();

%}

%option outfile= "./src/la.cpp"
%option noyywrap
%option yylineno
%option nounput

LETTER  [a-zA-Z][a-zA-Z0-9_]*

%%

"if"|"else"|"while"|"for"|"function"|"return"|"break"|"continue"|"and"|"not"|"or"|"local"|"true"|"false"|"nil"    {
        if(strcmp(yytext,"if")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return IF;
        }else if(strcmp(yytext,"else")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return ELSE;
        }else if(strcmp(yytext,"while")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return WHILE;
        }else if(strcmp(yytext,"for")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return FOR;
        }else if(strcmp(yytext,"function")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return FUNCTION;
        }else if(strcmp(yytext,"return")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return RETURN;
        }else if(strcmp(yytext,"break")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return BREAK;
        }else if(strcmp(yytext,"continue")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return CONTINUE;
        }else if(strcmp(yytext,"and")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return AND;
        }else if(strcmp(yytext,"not")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return NOT;
        }else if(strcmp(yytext,"or")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return OR;
        }else if(strcmp(yytext,"local")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return LOCAL;
        }else if(strcmp(yytext,"true")==0){
            alpha_yylex(data,(TYPE)KEYWORD1);
            return TRUE;
        }else if(strcmp(yytext,"false")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return FALSE;
        }else if(strcmp(yytext,"nil")==0){
            yylval.data=new std::string(to_upper(yytext));
            data=yylval.data;
            alpha_yylex(data,(TYPE)KEYWORD1);
            return NIL;
        }
    }

{LETTER}+ {yylval.data=(void*)new std::string(yytext);data=yylval.data;alpha_yylex(data,(TYPE)IDENT1);return IDENT;}

"=" {yylval.data=new std::string("ASSIGNMENT");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return ASSIGN;}

"==" {yylval.data=new std::string("EQUAL");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return EQUAL;}

"-" {yylval.data=new std::string("MINUS");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return MINUS;}

"+" {yylval.data=new std::string("PLUS");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return PLUS;}

"*" {yylval.data=new std::string("MULTIPLY");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return MULTIPLY;}

"/" {yylval.data=new std::string("DIVIDE");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return DIVIDE;}

"%" {yylval.data=new std::string("MODULO");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return MODULO;}

"!=" {yylval.data=new std::string("NOT_EQUAL");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return NOT_EQUAL;}

"++" {yylval.data=new std::string("PLUS_PLUS");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return PLUS_PLUS;}

"--" {yylval.data=new std::string("MINUS_MINUS");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return MINUS_MINUS;}

"<" {yylval.data=new std::string("LESS");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return LESS;}

">" {yylval.data=new std::string("GREATER");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return GREATER;}

"<=" {yylval.data=new std::string("LESS_EQUAL");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return LESS_EQUAL;}

">=" {yylval.data=new std::string("GREATER_EQUAL");data=yylval.data;alpha_yylex(data,(TYPE)OPERATOR1);return GREATER_EQUAL;}

[0-9]+ {int* x=new int;*x=atoi(yytext);yylval.data=x;data=yylval.data;alpha_yylex(data,(TYPE)INTCONST1);return INTCONST;}

[0-9]+[.][0-9]+  {double* x=new double;*x=atof(yytext);yylval.data=x;data=yylval.data;alpha_yylex(data,(TYPE)REALCONST1); return REALCONST;}

"{"|"}" {if(strcmp(yytext,"{")==0){yylval.data=new std::string("LEFT_CURLY_BRACKET");data=new std::string("LEFT");alpha_yylex(data,(TYPE)CURLY_BRACKET1); return LEFT_CURLY_BRACKET;}else{yylval.data=new std::string("RIGHT_CURLY_BRACKET");data=new std::string("RIGHT");alpha_yylex(data,(TYPE)CURLY_BRACKET1);return RIGHT_CURLY_BRACKET;}}

"["|"]" {if(strcmp(yytext,"[")==0){yylval.data=new std::string("LEFT_SQUARE_BRACKET");data=new std::string("LEFT"); alpha_yylex(data,(TYPE)SQUARE_BRACKET1); return LEFT_SQUARE_BRACKET;}else{yylval.data=new std::string("RIGHT_SQUARE_BRACKET");data=new std::string("RIGHT");alpha_yylex(data,(TYPE)SQUARE_BRACKET1);return RIGHT_SQUARE_BRACKET;}}

"("|")" {if(strcmp(yytext,"(")==0){yylval.data=new std::string("LEFT_PARENTHESIS");data= new std::string("LEFT");alpha_yylex(data,(TYPE)PARENTHESIS1); return LEFT_PARENTHESIS;}else{yylval.data=new std::string("RIGHT_PARENTHESIS");data= new std::string("RIGHT");alpha_yylex(data,(TYPE)PARENTHESIS1);return RIGHT_PARENTHESIS;}}

";" {yylval.data=new std::string(); data= new std::string();alpha_yylex(data,SEMICOLON1);return SEMICOLON;}

":" {yylval.data=new std::string();data= new std::string();alpha_yylex(data,COLON1); return COLON;}

"," {yylval.data=new std::string();data= new std::string();alpha_yylex(data,COMMA1);return COMMA;}

"::"    {yylval.data=new std::string();data= new std::string();alpha_yylex(data,DOUBLE_COLON1); return DOUBLE_COLON;}

"." {yylval.data=new std::string();data= new std::string();alpha_yylex(data,DOT1); return DOT;}

".."    {yylval.data=new std::string();data= new std::string(); alpha_yylex(data,DOUBLE_DOT1); return DOUBLE_DOT;}

["]([^"\\\n]|\\.|\\\n)*["]   {
            wrong_string=0;
            instring=1;
            yylval.data=new std::string(yytext);
            data=yylval.data;
            tempstring=replace_escapes(yytext);
            alpha_yylex(data,STRING1);
            instring=0;
            return STRING;
            }

"//".*" "*  {std::string *temp=new std::string("LINE_COMMENT");yylval.data=temp;data=yylval.data;strcpy(yytext,"");comment=0;linetmp=yylineno;alpha_yylex(data,(TYPE)COMMENT1);}


"/*"    {
            int c,start=1,end=0;
            linestart=yylineno;
            std::string *temp=new std::string();
            wrong_comment=1;
            int flag=1;
            while((c=yyinput())&&flag)
            {
                if(c=='/'){
                    c=yyinput();
                    if(c=='*'){
                        start++;
                    }
                }else if(c=='*'){
                    c=yyinput();
                    if(c=='/')
                    {   end++;
                        if(end==start)
                        {
                            if(start==1){
                                *temp=*temp+to_string(linestart)+"-"+to_string(yylineno);
                                strcpy(yytext,temp->c_str());
                                temp=new std::string("BLOCK_COMMENT");
                                yylval.data=temp;
                                wrong_comment=0;
                                comment=0;
                                linetmp=linestart;
                                data=yylval.data;
                                alpha_yylex(data,(TYPE)COMMENT1);
                                
                            }else if(start>1){
                                *temp=*temp+to_string(linestart)+" - "+to_string(yylineno);
                                strcpy(yytext,temp->c_str());
                                temp=new std::string("BLOCK_COMMENT");
                                yylval.data=temp;
                                wrong_comment=0;
                                linetmp=linestart;
                                data=yylval.data;
                                alpha_yylex(data,(TYPE)NESTED_COMMENT1);
                            }
                            flag=0;
                        }
                    }
                }
            }
        }

"\n"|" "|'\n'|"\t"|"\r"|"$0" {}

<<EOF>> {if(wrong_string==1){ yylval.data=new std::string(to_string(yylineno));data=yylval.data;alpha_yylex(data,(TYPE)NOT_CLOSED_STRING1);}else if(wrong_comment==1){ yylval.data=new std::string(to_string(linestart));data=yylval.data;alpha_yylex(data,(TYPE)NOT_CLOSED_COMMENT1);}return EOF;}

. {alpha_yylex(new std::string("UNKNOWN"),UNKNOWN1);return UNKNOWN;}

%%
