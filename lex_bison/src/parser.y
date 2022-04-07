%{
    #include <stdio.h>
    #include "al.hpp"
    #include "hashtable.hpp"
    extern int yylex();
    int yyerror(const char *yaccProvidedMessage);
    int scope = 0;
    bool insideFunction = false;
    int anonymousFunctionCount = 0;
    HashTable *table;
    SymbolTableEntry *temp;
    SymbolTableEntry *lval;
    struct functioncall{
        int openfunction=0;
        int closefunction=0;
        unsigned int lastfunctioncall=0;
    }func;
    int  memberflag=0;
%}
%union{
    void* data;
}

%start program
%token <data> IF 
%token <data> ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND OR NOT LOCAL TRUE FALSE NIL
%token <data> IDENT
%token <data> ASSIGN EQUAL MINUS PLUS MULTIPLY DIVIDE MODULO NOT_EQUAL PLUS_PLUS MINUS_MINUS LESS GREATER LESS_EQUAL GREATER_EQUAL
%token <data> INTCONST REALCONST
%token <data> LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET LEFT_PARENTHESIS RIGHT_PARENTHESIS
%token <data> SEMICOLON COLON COMMA DOUBLE_COLON DOT DOUBLE_DOT
%token <data> STRING 
%token <data> UNKNOWN
%token <data> UMINUS
%type  program
%type  stmt
%type  expr
%type  term
%type  assignexpr
%type  primary
%type  lvalue
%type  member
%type  call
%type  callsuffix
%type  normcall
%type  methodcall
%type  elist
%type  objectdef
%type  indexed
%type  indexedelem
%type  block
%type  funcdef
%type  const
%type  idlist
%type  ifstmt
%type  whilestmt
%type  forstmt
%type  returnstmt
%type  stmt_list

%left LEFT_PARENTHESIS RIGHT_PARENTHESIS
%left LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET
%left DOT DOUBLE_DOT
%right NOT PLUS_PLUS MINUS_MINUS 
%nonassoc UMINUS
%left MULTIPLY DIVIDE MODULO
%left PLUS MINUS
%nonassoc GREATER GREATER_EQUAL LESS LESS_EQUAL
%nonassoc EQUAL NOT_EQUAL
%left AND
%left OR
%right ASSIGN

%%
program: stmt_list
    ;
stmt:   expr SEMICOLON
        |ifstmt
        |whilestmt
        |forstmt
        |returnstmt
        |BREAK{if(scope==0){ yyerror("Cannot BREAK while outside of branch block");}} SEMICOLON
        |CONTINUE{if(scope==0){ yyerror("Cannot CONTINUE while outside of branch block");}} SEMICOLON
        |block
        |funcdef
        |SEMICOLON
        ;
stmt_list: stmt_list stmt
    |
    ;
expr:   assignexpr
    |expr EQUAL expr
    |expr MINUS expr
    |expr PLUS expr
    |expr MULTIPLY expr
    |expr DIVIDE expr
    |expr MODULO expr
    |expr NOT_EQUAL expr
    |expr LESS expr
    |expr GREATER expr
    |expr LESS_EQUAL expr
    |expr GREATER_EQUAL expr
    |expr AND expr
    |expr OR expr
    | term
    ;
term: LEFT_PARENTHESIS expr RIGHT_PARENTHESIS
    | NOT expr
    | MINUS expr %prec UMINUS
    | PLUS_PLUS lvalue {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to preincrement a library function");}}else{yyerror("Trying to increment a non-existent variable");}}
    | lvalue PLUS_PLUS {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to increment a library function");}}else{yyerror("Trying to increment a non-existent variable");}}
    | MINUS_MINUS lvalue {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to predecrement a library function");}}else{yyerror("Trying to decrement a non-existent variable");}}
    | lvalue MINUS_MINUS {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to predecrement a library function");}}else{yyerror("Trying to decrement a non-existent variable");}}
    | primary
    ;
assignexpr: lvalue{if(lval&&memberflag==0){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Cannot assign to function as an lvalue");}}} ASSIGN expr
          ;
primary: lvalue
       | call
       | objectdef
       | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS
       | const
       ;
       
lvalue: IDENT{memberflag=0;
            lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
            //to ckeck if  the use changes
            if(lval==NULL){
                for(int i=scope-1;i>-1;i--){
                    lval=table->lookup(((string*)(yylval.data))->c_str(),i);
                    if(lval!=NULL){
                        break;
                    }
                }
                if(lval!=NULL){                                                                                                // local f(x); g(){f;}  
                    //if local or formal ==illegal else lval= entry
                    //illegal
                    if(func.openfunction!=func.closefunction&&func.lastfunctioncall>lval->value.varVal->scope){ //
                        if(lval->type==LOCAL1||lval->type==FORMAL){
                            yyerror("Cannot use local or formal variable from illegal scope");
                        }
                    }
                }else{
                    if(scope>0)
                        table->insert(true,LOCAL1,((string*)(yylval.data))->c_str(),scope, yylineno);
                    else
                        table->insert(true,GLOBAL,((string*)(yylval.data))->c_str(),scope, yylineno);

                    lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                }
            }
      }
      |LOCAL IDENT{memberflag=0;
                lval=table->lookup(((string*)(yylval.data))->c_str());
                if(lval!=NULL&&lval->type==LIBFUNC&&scope!=0){
                    yyerror("Trying to shadow a library function");
                }else if(((lval=table->lookup(((string*)(yylval.data))->c_str(),scope)))==NULL){
                    if(scope>0){
                        table->insert(true, LOCAL1,((string*)(yylval.data))->c_str(),scope, yylineno);
                        lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                    }else{
                        table->insert(true, GLOBAL, ((string*)(yylval.data))->c_str(),scope, yylineno);
                        lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                    }
                }
      }
      |DOUBLE_COLON IDENT{memberflag=0;
            lval=table->lookup(((string*)(yylval.data))->c_str(),0);
            if(lval==NULL){
                yyerror("Trying to access undefined global variable");
            }
      }
      |member
      ;
member: lvalue DOT IDENT{memberflag=1;}
      | lvalue LEFT_SQUARE_BRACKET expr RIGHT_SQUARE_BRACKET
      | call DOT IDENT {memberflag=1;}
      | call LEFT_SQUARE_BRACKET expr RIGHT_SQUARE_BRACKET
      ;
call: call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    | lvalue callsuffix
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;
callsuffix: normcall
    |methodcall
    ;
normcall: LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;
methodcall: DOUBLE_DOT IDENT LEFT_PARENTHESIS elist RIGHT_PARENTHESIS
    ;
elist: expr elists {}
     | {}
     ;
elists: COMMA expr elists
      |
      ;
objectdef: LEFT_SQUARE_BRACKET elist RIGHT_SQUARE_BRACKET
	     |LEFT_SQUARE_BRACKET indexed RIGHT_SQUARE_BRACKET
		 ;
indexed: indexedelem indexedelems
       ;
indexedelems: COMMA indexedelem indexedelems
			|
			;
indexedelem: LEFT_CURLY_BRACKET expr COLON expr RIGHT_CURLY_BRACKET
           ;
block: LEFT_CURLY_BRACKET{scope++;} stmt_list RIGHT_CURLY_BRACKET{table->hide(scope);scope--;}
     ;
funcdef: FUNCTION IDENT{
                temp=table->lookup(((string*)(yylval.data))->c_str());
                if(temp!=NULL&&temp->type==LIBFUNC){
                    yyerror("Trying to shadow a library function");
                }else if((table->lookup(((string*)(yylval.data))->c_str(),scope))!=NULL){
                    if((table->lookup(((string*)(yylval.data))->c_str(),scope))->type==FORMAL){
                        yyerror("function name previously defined as formal parameter");
                    }else{
                        yyerror("Redefinition of variable or function name");                                                       //function f(x){ x = 1; function x(){ return 1; } } function hi (x,y,z){}
                    }
                }else{
				    table->insert(true, USERFUNC, ((string*)(yylval.data))->c_str(),scope, yylineno);
                }
                } LEFT_PARENTHESIS{scope++;insideFunction=true;} idlist RIGHT_PARENTHESIS{scope--;func.openfunction++;func.lastfunctioncall=(unsigned int) scope+1;} block {insideFunction = false;func.closefunction++;func.lastfunctioncall=(unsigned int) scope;}
       | FUNCTION {string str=string("f_"); str+=to_string(anonymousFunctionCount);table->insert(true, USERFUNC, str.c_str() ,scope, yylineno);anonymousFunctionCount++;} LEFT_PARENTHESIS{scope++;insideFunction=true;} idlist RIGHT_PARENTHESIS{scope--;func.openfunction++;func.lastfunctioncall=(unsigned int) scope+1;} block {insideFunction = false;func.closefunction++;func.lastfunctioncall=(unsigned int) scope;}
       ;
const: INTCONST
	| REALCONST
	| STRING
	| NIL 
	| TRUE 
	| FALSE
    ;
idlist: IDENT{  
                temp=table->lookup(((string*)(yylval.data))->c_str());
                if(temp!=NULL&&temp->type==LIBFUNC){
                    yyerror("Trying to shadow a library function");
                }else if((table->lookup(((string*)(yylval.data))->c_str(),scope))!=NULL){
                    if((table->lookup(((string*)(yylval.data))->c_str(),scope))->type==FORMAL){
                        
                    yyerror("Formal parameter has been already given");
                    }
                }else{
				    table->insert(true, FORMAL, ((string*)(yylval.data))->c_str(),scope, yylineno);
                }
			}
     idlists
      |
      ;
idlists: COMMA IDENT{  
                temp=table->lookup(((string*)(yylval.data))->c_str());
                if(temp!=NULL&&temp->type==LIBFUNC){
                    yyerror("Trying to shadow a library function");
                }else if((table->lookup(((string*)(yylval.data))->c_str(),scope))!=NULL){
                    if((table->lookup(((string*)(yylval.data))->c_str(),scope))->type==FORMAL){
                        yyerror("Formal parameter has been already given");
                    }
                }else{
				    table->insert(true, FORMAL, ((string*)(yylval.data))->c_str(),scope, yylineno);
                }
			} 
        idlists
       |
       ;
ifstmt: IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
      |IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt ELSE stmt
      ;
whilestmt: WHILE LEFT_PARENTHESIS expr RIGHT_PARENTHESIS stmt
		 ;
forstmt: FOR LEFT_PARENTHESIS elist  SEMICOLON expr SEMICOLON elist RIGHT_PARENTHESIS stmt
	   ;
returnstmt: RETURN{if(scope==0){ yyerror("Cannot RETURN while outside of function");}} expr SEMICOLON
          | RETURN{if(scope==0){ yyerror("Cannot RETURN while outside of function");}} SEMICOLON
          ;


%%
int yyerror(const char *yaccProvidedMessage) {
    printf("ERROR::Line %d - %s token: \"%s\" \n",yylineno,yaccProvidedMessage, yytext);
    return 0;
}

int main(int argc,char* argv[]) {
    
    if ( argc == 3 ){
        yyin= fopen( argv[1], "r" );
        yyout= fopen( argv[2], "w" );
        assert( yyin != NULL );
        assert( yyout != NULL );
    }else if(argc==2){
        yyin= fopen( argv[1], "r" );
        yyout = stdout;
        assert( yyin != NULL );
    }
    table =new HashTable(500);
    yyparse();
    table->printScopes();
    fclose(yyin);
    fclose(yyout);
    return 0;
}
