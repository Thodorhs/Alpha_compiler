%{
    #include <stdio.h>
    #include<fstream>
    #include "al.hpp"
    #include "../intermediate/intermediate.hpp"
    #include "../target/target.hpp"
    extern int yylex();
    int yyerror(const char *yaccProvidedMessage);
    const char* filename;
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
    enum type_t{numConst,stringConst,boolConst,nil};
    string *id;
    int n;
    expr* head;
    expr* curr;
    int loopcounter;
    stack <int> stack_lc;
    struct stmt_t;
    typedef struct stmt_t stmt_t;
    stack<vector<unsigned>*> returnLists;
    stack <int> func_start_jump_label;
%}

%union{
    struct expr* exp;
    struct SymbolTableEntry* sym;
    void* data;
    unsigned off;
    struct stmt_t {
        int breaklist, contlist;
    }s;
    struct f{unsigned test,enter;}_for;
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
%type <s> loopstmt
%type <s> stmt
%type <s> stmt_list
%type <exp>  expr
%type <exp>  term
%type <exp>  assignexpr
%type <exp>  primary
%type <exp>  lvalue
%type <exp>  member
%type <exp>  call
%type <exp>  callsuffix
%type <exp>  normcall
%type <exp>  methodcall
%type <exp>  elist
%type <exp>  elists
%type <exp>  objectdef
%type <exp>  indexed
%type <exp>  indexedelem
%type <exp>  indexedelems
%type <s>  block
%type <sym>  funcdef
%type <exp>  const
%type idlist
%type <s> ifstmt
%type <off> ifprefix
%type <off> elseprefix
%type <s> break
%type <s>continue
%type whilestmt
%type <off>  whilestart
%type <off>  whilecond
%type forstmt
%type <off> N
%type <off> M
%type <_for> forprefix
%type <exp>  returnstmt
%type <data> funcname
%type <sym> funcprefix
%type <off> funcbody
%type loopstart
%type loopend

%right ASSIGN
%left OR
%left AND
%nonassoc EQUAL NOT_EQUAL
%nonassoc GREATER GREATER_EQUAL LESS LESS_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MODULO
%right NOT PLUS_PLUS MINUS_MINUS 
%nonassoc UMINUS
%left DOT DOUBLE_DOT
%left LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET
%left LEFT_PARENTHESIS RIGHT_PARENTHESIS
%left RIGHT_CURLY_BRACKET LEFT_CURLY_BRACKET

%%
program: stmt_list
    ;
stmt:   expr SEMICOLON {    resettemp();
                            $$.breaklist=0;
                            $$.contlist=0;
                            if($1->type==boolexpr_e){
                                backpatch($1->true_l,nextquadlabel());
                                backpatch($1->false_l,nextquadlabel()+2);
                                emit(assign,$1,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$1,newexpr(constbool_e,false),NULL,0,yylineno);
                            }}
        |ifstmt {resettemp();$$=$1;}
        |whilestmt {resettemp();$$.breaklist=0;$$.contlist=0;}
        |forstmt {resettemp();$$.breaklist=0;$$.contlist=0;}
        |returnstmt {resettemp();$$.breaklist=0;$$.contlist=0;
                        returnLists.top()->push_back(nextquadlabel());
                        emit(jump,NULL,NULL,NULL,0,yylineno);
                    }
        |break {$$=$1;}
        |continue {$$=$1;}
        |block {resettemp();$$=$1;}
        |funcdef {resettemp();$$.breaklist=0;$$.contlist=0;}
        |SEMICOLON {resettemp();$$.breaklist=0;$$.contlist=0;}
        ;

stmt_list: stmt_list stmt { resettemp();
                                $$.breaklist=mergelist($1.breaklist,$2.breaklist);
                                $$.contlist=mergelist($1.contlist,$2.contlist);
                          }
    |{ resettemp();$$.breaklist=0;$$.contlist=0;}
    ;
expr:   assignexpr {$$ = $1;}
    |expr MINUS expr {
                        if(check_arith_expr($1,$3)){
                            $$ = tempcheck(arithexpr_e,$1, $3,table,scope,yylineno);
                            $$->type=const_check($1,$3);
                            emit(sub, $$, $1, $3, 0, yylineno);
                        }else{
                            comperror("Cannot subtract non-numeric values",filename,yylineno);
                        }
                    }
    |expr PLUS expr {
                        if(check_arith_expr($1,$3)){
                            $$ = tempcheck(arithexpr_e,$1, $3,table,scope,yylineno);
                            $$->type=const_check($1,$3);
                            emit(add, $$, $1, $3, 0, yylineno);
                        }else{
                            comperror("Cannot add non-numeric values",filename,yylineno);
                        }
                    }
    |expr MULTIPLY expr {
                        if(check_arith_expr($1,$3)){
                            $$ = tempcheck(arithexpr_e,$1, $3,table,scope,yylineno);
                            $$->type=const_check($1,$3);
                            emit(mul, $$, $1, $3, 0, yylineno);
                        }else{
                            comperror("Cannot multiply non-numeric values",filename,yylineno);
                        }
                    }
    |expr DIVIDE expr {
                        if(check_arith_expr($1,$3)){
                            $$ = tempcheck(arithexpr_e,$1, $3,table,scope,yylineno);
                            $$->type=const_check($1,$3);
                            emit(Div, $$, $1 ,$3 , 0, yylineno);
                        }else{
                            comperror("Cannot divide non-numeric values",filename,yylineno);
                        }
                    }
    |expr MODULO expr {
                        if(check_arith_expr($1,$3)){
                            $$ = tempcheck(arithexpr_e,$1, $3,table,scope,yylineno);
                            $$->type=const_check($1,$3);
                            emit(mod, $$, $1, $3, 0, yylineno);
                        }else{
                            comperror("Cannot modulo non-numeric values",filename,yylineno);
                        }
                    }
    |expr EQUAL expr {
                        if(!type_check($1,$3)){
                            comperror("Relop operations between different const types!",filename,yylineno);
                        }
                        if($1->type==boolexpr_e){
                                backpatch($1->true_l,nextquadlabel());
                                backpatch($1->false_l,nextquadlabel()+2);
                                emit(assign,$1,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$1,newexpr(constbool_e,false),NULL,0,yylineno);
                        }
                        if($3->type==boolexpr_e){
                                backpatch($3->true_l,nextquadlabel());
                                backpatch($3->false_l,nextquadlabel()+2);
                                emit(assign,$3,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$3,newexpr(constbool_e,false),NULL,0,yylineno);
                            }
                        $$ = tempcheck(boolexpr_e,$1, $3,table,scope,yylineno);
                        emitR(if_eq,$$,$1,$3,yylineno);
                        
                       }
    |expr NOT_EQUAL expr {
                            if(!type_check($1,$3)){
                                comperror("Relop operations between different const types!",filename,yylineno);
                            }
                            if($1->type==boolexpr_e){
                                backpatch($1->true_l,nextquadlabel());
                                backpatch($1->false_l,nextquadlabel()+2);
                                emit(assign,$1,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$1,newexpr(constbool_e,false),NULL,0,yylineno);
                        }
                        if($3->type==boolexpr_e){
                                backpatch($3->true_l,nextquadlabel());
                                backpatch($3->false_l,nextquadlabel()+2);
                                emit(assign,$3,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$3,newexpr(constbool_e,false),NULL,0,yylineno);
                            }
                            $$ = tempcheck(boolexpr_e,$1, $3,table,scope,yylineno);
                            emitR(if_noteq,$$,$1,$3,yylineno);
                       }
    |expr LESS expr {
                        if(!type_check($1,$3)){
                            comperror("Relop operations between different const types!",filename,yylineno);
                        }
                        $$ = tempcheck(boolexpr_e,$1, $3,table,scope,yylineno);
                        emitR(if_less,$$,$1,$3,yylineno);
                       }
    |expr GREATER expr {
                        if(!type_check($1,$3)){
                            comperror("Relop operations between different const types!",filename,yylineno);
                        }
                        $$ = tempcheck(boolexpr_e,$1, $3,table,scope,yylineno);
                        emitR(if_greater,$$,$1,$3,yylineno);
                       }
    |expr LESS_EQUAL expr {
                            if(!type_check($1,$3)){
                                comperror("Relop operations between different const types!",filename,yylineno);
                            }
                            $$ = tempcheck(boolexpr_e,$1, $3,table,scope,yylineno);
                            emitR(if_lesseq,$$,$1,$3,yylineno);
                       }
    |expr GREATER_EQUAL expr {
                        if(!type_check($1,$3)){
                            comperror("Relop operations between different const types!",filename,yylineno);
                        }
                        $$ = tempcheck(boolexpr_e,$1, $3,table,scope,yylineno);
                        emitR(if_greatereq,$$,$1,$3,yylineno);
                       }
    |expr AND {if($1->type!=boolexpr_e){to_bool($1,yylineno);}} M expr {
                        if($5->type!=boolexpr_e){
                            to_bool($5,yylineno);
                        }
                        $$ = tempcheck(boolexpr_e,$1, $5,table,scope,yylineno);
                        backpatch($1->true_l,$4);
                        $$->true_l=$5->true_l;
                        $$->false_l=merge($1->false_l,$5->false_l);
                    }
    |expr OR {if($1->type!=boolexpr_e){to_bool($1,yylineno);}} M expr {
                        if($5->type!=boolexpr_e){
                            to_bool($5,yylineno);
                        }
                        $$ = tempcheck(boolexpr_e,$1, $5,table,scope,yylineno);
                        backpatch($1->false_l,$4);
                        $$->true_l=merge($1->true_l,$5->true_l);
                        $$->false_l=$5->false_l;
                       }
    | term {$$ = $1;}
    ;
term: LEFT_PARENTHESIS expr RIGHT_PARENTHESIS{$$=$2;}
    | NOT expr{
                    
                   
                    if($2->type!=boolexpr_e){                        
                        $2->true_l.push_back(nextquadlabel());
                        $2->false_l.push_back(nextquadlabel()+1);
                        emit(if_eq,$2,newexpr(constbool_e,true),NULL,0,line);
                        emit(jump,NULL,NULL,NULL,0,line);
                    }
                    $$ = tempcheck(boolexpr_e,$2,table,scope,yylineno);
                    vector <unsigned> vec=$2->true_l;
                    $$->true_l=$2->false_l;
                    $$->false_l=vec;
                }
    | MINUS expr %prec UMINUS {
                                    check_arith($2,filename,yylineno);
                                    $$ = tempcheck(arithexpr_e,$2,table,scope,yylineno);
                                    emit(uminus,$$,$2,NULL,0,yylineno);
                                }
    | PLUS_PLUS lvalue {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to preincrement a library function");}}else{yyerror("Trying to increment a non-existent variable");}
            check_arith($2,filename,yylineno);
            if($2->type==tableitem_e){
                $$=emit_iftableitem($2,table,scope,yylineno);
                emit(add,$$,$$,newexpr(constnum_e,(double)1),0,yylineno);
                emit(tablesetelem,$2,$2->index,$$,0,yylineno);
            }else{
                emit(add,$2,$2,newexpr(constnum_e,(double)1),0,yylineno);
                $$ = tempcheck(arithexpr_e,$2,table,scope,yylineno);
                emit(assign,$$,$2,NULL,0,yylineno);
            }
    }
    | lvalue PLUS_PLUS {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to increment a library function");}}else{yyerror("Trying to increment a non-existent variable");}
            check_arith($1,filename,yylineno);
            $$ = tempcheck(var_e,$1,table,scope,yylineno);
            if($1->type==tableitem_e){
                expr* val=emit_iftableitem($1,table,scope,yylineno);
                emit(assign,$$,val,NULL,0,yylineno);
                emit(add,val,val,newexpr(constnum_e,(double)1),0,yylineno);
                emit(tablesetelem,$1,$1->index,val,0,yylineno);
            }else{
                emit(assign,$$,$1,NULL,0,yylineno);
                emit(add,$1,$1,newexpr(constnum_e,(double)1),0,yylineno);
            }
    }
    | MINUS_MINUS lvalue {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to predecrement a library function");}}else{yyerror("Trying to decrement a non-existent variable");}
            check_arith($2,filename,yylineno);
            if($2->type==tableitem_e){
                $$=emit_iftableitem($2,table,scope,yylineno);
                emit(sub,$$,$$,newexpr(constnum_e,(double)1),0,yylineno);
                emit(tablesetelem,$2,$2->index,$$,0,yylineno);
            }else{
                emit(sub,$2,$2,newexpr(constnum_e,(double)1),0,yylineno);
                $$ = tempcheck(arithexpr_e,$2,table,scope,yylineno);;
                emit(assign,$$,$2,NULL,0,yylineno);
            }
        }
    | lvalue MINUS_MINUS {if(lval){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Trying to predecrement a library function");}}else{yyerror("Trying to decrement a non-existent variable");}
            check_arith($1,filename,yylineno);
            $$ = tempcheck(arithexpr_e,$1,table,scope,yylineno);
            if($1->type==tableitem_e){
                expr* val=emit_iftableitem($1,table,scope,yylineno);
                emit(assign,$$,val,NULL,0,yylineno);
                emit(sub,val,val,newexpr(constnum_e,(double)1),0,yylineno);
                emit(tablesetelem,$1,$1->index,val,0,yylineno);
            }else{
                emit(assign,$$,$1,NULL,0,yylineno);
                emit(sub,$1,$1,newexpr(constnum_e,(double)1),0,yylineno);
            }
        }
    | primary {$$ = $1;}
    ;
assignexpr: lvalue{if(lval&&memberflag==0){if(lval->type==LIBFUNC||lval->type==USERFUNC){yyerror("Cannot assign to function as an lvalue");}}} ASSIGN expr {    if($4->type==boolexpr_e){
                                                                                                                                                                    backpatch($4->true_l,nextquadlabel());
                                                                                                                                                                    backpatch($4->false_l,nextquadlabel()+2);
                                                                                                                                                                    emit(assign,$4,newexpr(constbool_e,true),NULL,0,yylineno);
                                                                                                                                                                    emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                                                                                                                                                    emit(assign,$4,newexpr(constbool_e,false),NULL,0,yylineno);
                                                                                                                                                                }
                                                                                                                                                                if($1->type==tableitem_e){
                                                                                                                                                                    emit(tablesetelem,$1,$1->index,$4,0,yylineno);
                                                                                                                                                                    $$=emit_iftableitem($1,table,scope,line);
                                                                                                                                                                    $$->type=assignexpr_e;
                                                                                                                                                                }else{
                                                                                                                                                                    emit(assign,$1,$4,NULL,0,yylineno);
                                                                                                                                                                    $$=tempcheck(assignexpr_e,$1,$4,table,scope,line);
                                                                                                                                                                    emit(assign,$$,$1,NULL,0,yylineno);
                                                                                                                                                                }
                                                                                                                                                            }
          ;
primary: lvalue{$$=emit_iftableitem($1,table,scope,line);/*$$=$1*/}
       | call{$$=$1;$$->type=var_e;}
       | objectdef
       | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS {
                                                        $$=newexpr(programfunc_e,$2);
                                                    }
       | const {$$ = $1;}
       ;
lvalue: IDENT{memberflag=0;
            lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
            if(lval==NULL){
                for(int i=scope-1;i>-1;i--){
                    lval=table->lookup(((string*)(yylval.data))->c_str(),i);
                    if(lval!=NULL){
                        break;
                    }
                }
                if(lval!=NULL){
                    if(func.openfunction!=func.closefunction&&func.lastfunctioncall>lval->value.varVal->scope){
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
                    lval->space=currscopespace();
                    lval->offset=currscopeoffset();
                    incurrscopeoffset();
                }
            }
            $$=newexpr(lval);
      }
      |LOCAL IDENT{memberflag=0;
                lval=table->lookup(((string*)(yylval.data))->c_str());
                if(lval!=NULL&&lval->type==LIBFUNC&&scope!=0){
                    yyerror("Trying to shadow a library function");
                }else if(((lval=table->lookup(((string*)(yylval.data))->c_str(),scope)))==NULL){
                    if(scope>0){
                        table->insert(true, LOCAL1,((string*)(yylval.data))->c_str(),scope, yylineno);
                        lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                        lval->space=currscopespace();
                        lval->offset=currscopeoffset();
                        incurrscopeoffset();
                    }else{
                        table->insert(true, GLOBAL, ((string*)(yylval.data))->c_str(),scope, yylineno);
                        lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                        lval->space=currscopespace();
                        lval->offset=currscopeoffset();
                        incurrscopeoffset();
                    }
                }

                $$=newexpr(lval);
      }
      |DOUBLE_COLON IDENT{memberflag=0;
            lval=table->lookup(((string*)(yylval.data))->c_str(),0);
            if(lval==NULL){
                yyerror("Trying to access undefined global variable");
            }
            $$=newexpr(lval);
      }
      |member {
                $$=$1;
            }
      ;
member: lvalue DOT IDENT{   memberflag=1;

                            $$=member_item($1,((string*)($3))->c_str(),table,scope,line);
                        }
      | lvalue LEFT_SQUARE_BRACKET expr RIGHT_SQUARE_BRACKET {
                                                                $1=emit_iftableitem($1,table,scope,yylineno);
                                                                $$=newexpr(tableitem_e);
                                                                $$->sym=$1->sym;
                                                                $$->index=$3;
                                                            }
      | call DOT IDENT {memberflag=1;}
      | call LEFT_SQUARE_BRACKET expr RIGHT_SQUARE_BRACKET
      ;
call: call LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {$$=make_call($1,$3,table,scope,yylineno);}
    | lvalue callsuffix {
                      $1=emit_iftableitem($1,table,scope,yylineno);
                            if($2->boolConst){
                                expr* t = $1;
                                $1=emit_iftableitem(member_item(t,$2->charConst,table,scope,line),table,scope,yylineno);
                                if(curr){
                                    curr->next=t;
                                }
                            }
                            $$=make_call($1,$2->next,table,scope,yylineno);
                        }
    | LEFT_PARENTHESIS funcdef RIGHT_PARENTHESIS LEFT_PARENTHESIS elist RIGHT_PARENTHESIS{
                                                                                            expr* func = newexpr(programfunc_e);
                                                                                            func->sym=$2;
                                                                                            $$=make_call(func,$5,table,scope,yylineno);
                                                                                            }
    ;
callsuffix: normcall {$$=$1;}
    |methodcall {$$=$1;}
    ;
normcall: LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
                                                        $$=newexpr(nil_e);
                                                        $$->next=$2;
                                                        $$->boolConst=false;
                                                        $$->charConst="";
                                                    }
    ;
methodcall: DOUBLE_DOT IDENT {id=((string*)(yylval.data));} LEFT_PARENTHESIS elist RIGHT_PARENTHESIS {
                                                                        $$=newexpr(tableitem_e);
                                                                        $$->sym=curr->sym;
                                                                        $$->next=$5;
                                                                        $$->charConst=*id;
                                                                        $$->boolConst=true;
                                                                    }
    ;
elist: expr elists {if ($2==NULL){head=$1;/*head->next=NULL;*/$$=head;curr=head;}else{$$=head;curr=$1;$2->next=$1;n=0;}
                    if($1->type==boolexpr_e){
                                backpatch($1->true_l,nextquadlabel());
                                backpatch($1->false_l,nextquadlabel()+2);
                                emit(assign,$1,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$1,newexpr(constbool_e,false),NULL,0,yylineno);
                        }
                    }
     | {$$=NULL;}
     ;
elists: COMMA expr elists {if(n==0){head=$2;curr=$2;$2->next=NULL;$$=$2;}else{ $3->next=$2;curr=$2;curr->next=NULL;$$=curr;}n++;
                            if($2->type==boolexpr_e){
                                backpatch($2->true_l,nextquadlabel());
                                backpatch($2->false_l,nextquadlabel()+2);
                                emit(assign,$2,newexpr(constbool_e,true),NULL,0,yylineno);
                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                emit(assign,$2,newexpr(constbool_e,false),NULL,0,yylineno);
                        }
                            }
      |{$$=NULL;}
      ;
objectdef: LEFT_SQUARE_BRACKET elist RIGHT_SQUARE_BRACKET {
                                                            expr* t= newexpr(newtable_e,newtemp(table,scope));
                                                            emit(tablecreate,t,NULL,NULL,0,yylineno);
                                                            int j=0;
                                                            expr* tm=$2;
                                                            for(j=0;tm;tm=tm->next){j++;}
                                                            emit_list(t,$2,j-1,yylineno);
                                                            $$=t;
                                                        }
	     |LEFT_SQUARE_BRACKET indexed RIGHT_SQUARE_BRACKET {
                                                            expr* t= newexpr(newtable_e,newtemp(table,scope));
                                                            emit(tablecreate,t,NULL,NULL,0,yylineno);
                                                            while($2!=NULL){
                                                                emit(tablesetelem,t,$2->index,$2,0,yylineno);
                                                                $2=$2->next;
                                                            }
                                                            $$=t;
                                                        }
		 ;
indexed: indexedelem indexedelems {$$=$1;$$->next=$2;}
       ;
indexedelems: COMMA indexedelem indexedelems {$$=$2;$$->next=$3;}
			| {$$=NULL;}
			;
indexedelem: LEFT_CURLY_BRACKET expr COLON expr RIGHT_CURLY_BRACKET {$4->index=$2;$$=$4;
                                                                    if($2->type==boolexpr_e){
                                                                            backpatch($2->true_l,nextquadlabel());
                                                                            backpatch($2->false_l,nextquadlabel()+2);
                                                                            emit(assign,$2,newexpr(constbool_e,true),NULL,0,yylineno);
                                                                            emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                                                            emit(assign,$2,newexpr(constbool_e,false),NULL,0,yylineno);
                                                                    }
                                                                    if($4->type==boolexpr_e){
                                                                            backpatch($4->true_l,nextquadlabel());
                                                                            backpatch($4->false_l,nextquadlabel()+2);
                                                                            emit(assign,$4,newexpr(constbool_e,true),NULL,0,yylineno);
                                                                            emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                                                            emit(assign,$4,newexpr(constbool_e,false),NULL,0,yylineno);
                                                                    }
                                                                    }
           ;
block: LEFT_CURLY_BRACKET{scope++;} stmt_list RIGHT_CURLY_BRACKET{table->hide(scope);scope--;$$=$3;}
     ;
funcname: IDENT {$$=yylval.data;}
    |   {string* str=new string("_f_"); *str+=to_string(anonymousFunctionCount++);$$=(void*)str;}
    ;
funcprefix:FUNCTION funcname{
                                temp=table->lookup(((string*)($2))->c_str());
                                if(temp!=NULL&&temp->type==LIBFUNC){
                                    yyerror("Trying to shadow a library function");
                                }else if((table->lookup(((string*)($2))->c_str(),scope))!=NULL){
                                    if((table->lookup(((string*)($2))->c_str(),scope))->type==FORMAL){
                                        yyerror("function name previously defined as formal parameter");
                                    }else{
                                        yyerror("Redefinition of variable or function name");
                                    }
                                }else{
                                    table->insert(true, USERFUNC, ((string*)($2))->c_str(),scope, yylineno);
                                }
                                enterscopespace();
                                save_offsets();
                                temp=table->lookup(((string*)($2))->c_str(),scope);
                                temp->iaddress=nextquadlabel();
                                $$=temp;
                                func_start_jump_label.push(nextquadlabel());
                                emit(jump,NULL,NULL,NULL,0,yylineno);
                                emit(funcstart,NULL,newexpr($$,'\0'),NULL,0,yylineno);
                            }
;
funcargs:LEFT_PARENTHESIS{scope++;insideFunction=true;} idlist RIGHT_PARENTHESIS{
                                                                                scope--;
                                                                                enterscopespace();
                                                                                func.openfunction++;
                                                                                func.lastfunctioncall=(unsigned int) scope+1;
                                                                                stack_lc.push(loopcounter);
                                                                                loopcounter=0;
                                                                                vector <unsigned> *list=new vector<unsigned>();
                                                                                returnLists.push(list);
                                                                                }
;
funcbody: block{
                    insideFunction = false;
                    func.closefunction++;
                    func.lastfunctioncall=(unsigned int) scope;
                    $$=currscopeoffset();
                    exitscopespace();
                }
                ;
funcdef: funcprefix funcargs funcbody {
                                        exitscopespace();
                                        $1->totalLocals=$3;
                                        reset_offsets();
                                        $$=$1;
                                        patch_func_start_jump(func_start_jump_label.top());
                                        func_start_jump_label.pop();
                                        emit(funcend,NULL,newexpr($1),NULL,0,yylineno);
                                        loopcounter=stack_lc.top();
                                        stack_lc.pop();
                                        vector <unsigned> *list=returnLists.top();
                                        backpatch(*list,nextquadlabel());
                                        returnLists.pop();
                                      }
       ;
const: INTCONST {
                    $$=newexpr(constnum_e,(double)*(int*)yylval.data);
                }
	| REALCONST {
                   $$=newexpr(constnum_e,*((double*)yylval.data));
                }
	| STRING {
                    $$=newexpr(conststring_e,*((string*)yylval.data));
             }
	| NIL   {
                     $$=newexpr(nil_e);
            }
	| TRUE  {
                    $$=newexpr(constbool_e,true);
            }
	| FALSE {
                    $$=newexpr(constbool_e,false);
            }
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
                    lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                    lval->space=currscopespace();
                    lval->offset=currscopeoffset();
                    incurrscopeoffset();
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
                    lval=table->lookup(((string*)(yylval.data))->c_str(),scope);
                    lval->space=currscopespace();
                    lval->offset=currscopeoffset();
                    incurrscopeoffset();
                }
			}
        idlists
       |
       ;
ifprefix: IF LEFT_PARENTHESIS expr RIGHT_PARENTHESIS{
                                                        if($3->type==boolexpr_e){
                                                                backpatch($3->true_l,nextquadlabel());
                                                                backpatch($3->false_l,nextquadlabel()+2);
                                                                emit(assign,$3,newexpr(constbool_e,true),NULL,0,yylineno);
                                                                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                                                                emit(assign,$3,newexpr(constbool_e,false),NULL,0,yylineno);
                                                        }
                                                        emit(if_eq,$3,newexpr(constbool_e,true),newexpr(constnum_e,(double)(nextquadlabel()+1+2)),0,yylineno);
                                                        $$=nextquadlabel();
                                                        emit(jump,NULL,NULL,NULL,0,yylineno);
                                                    }
    ;
elseprefix: ELSE {$$=nextquadlabel();emit(jump,NULL,NULL,NULL,0,yylineno);}
ifstmt: ifprefix stmt {patchlabel($1,nextquadlabel()+1);$$=$2;}
      |ifprefix stmt elseprefix stmt {
                                        patchlabel($1,($3+1)+1);
                                        patchlabel($3,nextquadlabel()+1);
                                        $$.breaklist=mergelist($2.breaklist,$4.breaklist);
                                        $$.contlist=mergelist($2.contlist,$4.contlist);
                                    }
      ;
break: BREAK SEMICOLON {
                        resettemp();
                        if(loopcounter==0){
                            comperror("Cannot use break statement outside of branch",filename,yylineno);
                        }
                        emit(jump,NULL,NULL,NULL,0,yylineno);
                        $$.breaklist=0;
                        $$.contlist=0;
                        $$.breaklist=newlist(nextquadlabel()-1);
                       }
    ;
continue: CONTINUE SEMICOLON {
                                resettemp();
                                if(loopcounter==0){
                                    comperror("Cannot use continue statement outside of branch",filename,yylineno);
                                }
                                emit(jump,NULL,NULL,NULL,0,yylineno);
                                $$.breaklist=0;
                                $$.contlist=0;
                                $$.contlist=newlist(nextquadlabel()-1);
                                }
    ;

loopstart:{++loopcounter;}
;
loopend:{--loopcounter;}
;
loopstmt: loopstart stmt loopend {
                                    $$=$2;
                                }
whilestart: WHILE{
    $$=nextquadlabel()+1;
    }
    ;
whilecond: LEFT_PARENTHESIS expr RIGHT_PARENTHESIS{
        if($2->type==boolexpr_e){
                backpatch($2->true_l,nextquadlabel());
                backpatch($2->false_l,nextquadlabel()+2);
                emit(assign,$2,newexpr(constbool_e,true),NULL,0,yylineno);
                emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                emit(assign,$2,newexpr(constbool_e,false),NULL,0,yylineno);
        }
        emit(if_eq,$2,newexpr(constbool_e,true),newexpr(constnum_e,(double)(nextquadlabel()+1+2)),0,yylineno);
        $$=nextquadlabel();
        emit(jump,NULL,NULL,NULL,0,yylineno);
        }
        ;
whilestmt: whilestart whilecond loopstmt {
        emit(jump,NULL,NULL,NULL,$1,yylineno);
        patchlabel($2,nextquadlabel()+1);
        patchlist($3.breaklist,nextquadlabel()+1);
        patchlist($3.contlist,$1);
        }
		;
N: {
    $$=nextquadlabel();
    emit(jump,NULL,NULL,NULL,0,yylineno);
    }
    ;
M: {
    $$=nextquadlabel();
    }
    ;
forprefix: FOR LEFT_PARENTHESIS elist SEMICOLON M expr SEMICOLON {
    if($6->type==boolexpr_e){
            backpatch($6->true_l,nextquadlabel());
            backpatch($6->false_l,nextquadlabel()+2);
            emit(assign,$6,newexpr(constbool_e,true),NULL,0,yylineno);
            emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
            emit(assign,$6,newexpr(constbool_e,false),NULL,0,yylineno);
    }
    $$.test=$5;
    $$.enter=nextquadlabel();
    emit(if_eq,NULL,newexpr(constbool_e,true),$6,0,yylineno);
}
;
forstmt: forprefix N elist RIGHT_PARENTHESIS N loopstmt N{
    patchlabel($1.enter,$5+1+1);
    patchlabel($2,nextquadlabel()+1);
    patchlabel($5,$1.test+1);
    patchlabel($7,$2+1+1);//edw
    patchlist($6.breaklist,nextquadlabel()+1);
    patchlist($6.contlist,$2+1+1);
}
;
returnstmt: RETURN{if(func.openfunction==func.closefunction){ comperror("Cannot RETURN while outside of function",filename,yylineno);}} expr SEMICOLON{
     if($3->type==boolexpr_e){
                        backpatch($3->true_l,nextquadlabel());
                        backpatch($3->false_l,nextquadlabel()+2);
                        emit(assign,$3,newexpr(constbool_e,true),NULL,0,yylineno);
                        emit(jump,NULL,NULL,NULL,nextquadlabel()+3,yylineno);
                        emit(assign,$3,newexpr(constbool_e,false),NULL,0,yylineno);
                    }
                    emit(ret,$3,NULL,NULL,0,yylineno );

}
          | RETURN{if(func.openfunction==func.closefunction){ comperror("Cannot RETURN while outside of function",filename,yylineno);}} SEMICOLON{emit(ret,NULL,NULL,NULL,0,yylineno );}
          ;


%%
int yyerror(const char *yaccProvidedMessage) {
    printf("ERROR::Line %d - %s token: \"%s\" \n",yylineno,yaccProvidedMessage, yytext);
    return 0;
}

int main(int argc,char* argv[]) {
    FILE fp;
    if ( argc == 3 ){
        yyin= fopen( argv[1], "r" );
        yyout=fopen( argv[2], "w" );
        filename=argv[1];
        assert( yyin != NULL );
        assert( yyout != NULL );
    }else if(argc==2){
        yyin= fopen( argv[1], "r" );
        yyout = fopen("../out/quads.txt", "w" );
        filename=argv[1];
        assert( yyin != NULL );
        assert( yyout != NULL );
    }
    table =new HashTable(500);
    yyparse();
    table->printScopes();
    fp=*stdout;
    *stdout=*yyout;
    printQuads();
    fclose(yyin);
    fclose(yyout);
    *stdout=fp;
    generate();
    return 0;
}
