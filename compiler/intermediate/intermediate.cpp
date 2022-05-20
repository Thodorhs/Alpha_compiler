#include "intermediate.hpp"
#include <assert.h>
#include <bits/stdc++.h>
int tempcount=0;
unsigned currquad=0;
stack <unsigned> func_offs;
stack <unsigned> form_offs;
vector <quad*> quads;
unsigned programVarOffset=0;
unsigned functionLocalOffset=0;
unsigned formalArgOffset=0;
unsigned scopeSpaceCounter=1;

void save_offsets(){
    func_offs.push(functionLocalOffset);
    form_offs.push(formalArgOffset);
    functionLocalOffset=0;
    formalArgOffset=0;
}

void reset_offsets(){
    functionLocalOffset=func_offs.top();
    formalArgOffset=form_offs.top();
    func_offs.pop();
    form_offs.pop();
}
scopespace_t currscopespace(void) {
    if(scopeSpaceCounter == 1) return programvar;
    else {
        if(scopeSpaceCounter % 2 == 0) return formalarg;
        else return functionlocal;
    }
}

unsigned currscopeoffset(void) {
    switch(currscopespace()) {
        case programvar : return programVarOffset;
        case functionlocal : return functionLocalOffset;
        case formalarg : return formalArgOffset;
        default: assert(0);
    }
}

void incurrscopeoffset(void) {
    switch(currscopespace()) {
        case programvar : ++programVarOffset; break;
        case functionlocal : ++functionLocalOffset; break;
        case formalarg : ++formalArgOffset; break;
        default: assert(0);
    }
}
bool type_check(expr* a, expr* b) {
    if(a->type==constnum_e||a->type==constbool_e||a->type==conststring_e){
        if(a->type == b->type) return true;
        else return false;
    }
    
    return true;
}
bool check_arith_expr(expr *a, expr *b){
    expr_t k=a->type;
    expr_t l=b->type;
    if(k == programfunc_e||k == libraryfunc_e||k == boolexpr_e||k == newtable_e||k == constbool_e||k == conststring_e ||k == nil_e){
        return false;
    }
    if(l == programfunc_e||l == libraryfunc_e||l == boolexpr_e||l == newtable_e||l == constbool_e||l == conststring_e ||l == nil_e){
        return false;
    }
    return true;
}

expr_t const_check(expr *a, expr *b){
    if (a->type == constnum_e && b->type == constnum_e) {
        return constnum_e;
    }
    return arithexpr_e;
}

void enterscopespace(void){++scopeSpaceCounter;}
void exitscopespace(void){assert(scopeSpaceCounter>1); --scopeSpaceCounter;}
void to_bool(expr *a,unsigned line){
    a->true_l=vector<unsigned>();
    a->false_l=vector<unsigned>();
    a->true_l.push_back(currquad);
    a->false_l.push_back(currquad+1);
    emit(if_eq,a,newexpr(constbool_e,true),NULL,0,line);
    emit(jump,NULL,NULL,NULL,0,line);
}

unsigned nextquadlabel(void) {
    return currquad;
}
void patchlabel(unsigned quadNo, unsigned label) {
    assert(quadNo<currquad && !quads[quadNo]->label);
    quads[quadNo]->label = label;
}
void backpatch(vector<unsigned>&list, unsigned label){
    for(unsigned i=0;i<list.size();i++){
        patchlabel(list[i],label+1);
    }
}
vector <unsigned> merge(vector <unsigned> l1,vector <unsigned>l2){
    if(l1.size()==0) return l2;
    else if(l2.size()==0) return l1;
    else{
        vector <unsigned> l3;
        for(unsigned i=0;i<l1.size();i++){
            l3.push_back(l1[i]);
        }
        for(unsigned i=0;i<l2.size();i++){
            l3.push_back(l2[i]);
        }
        return l3;
    }
}
void emitR(iopcode type, expr *res, expr *a, expr *b,unsigned yylineno) {
    res->true_l=vector<unsigned>();
    res->true_l.push_back(currquad);
    res->false_l=vector<unsigned>();
    res->false_l.push_back(currquad+1);
    emit(type,a,b,NULL,0,yylineno);
    emit(jump,NULL,NULL,NULL,0,yylineno);
}

expr* newexpr(expr_t type){
    expr* e = new expr;
    e->type = type;
    return e;
}

expr* newexpr(expr_t type, double numConst){
    expr* e = new expr;
    e->type = type;
    e->numConst = numConst;
    return e;
}

expr* newexpr(expr_t type, string charConst){
    expr* e = new expr;
    e->type = type;
    e->charConst = charConst;
    return e;
}

expr* newexpr(expr_t type, bool boolConst){
    expr* e = new expr;
    e->type = type;
    e->boolConst = boolConst;
    return e;
}

expr* newexpr(expr_t type, SymbolTableEntry* sym){
    expr* e = new expr;
    e->type = type;
    e->sym = sym;
    return e;
}

expr* newexpr(SymbolTableEntry* sym){
    expr* e = new expr;
    switch (sym->type)
    {
        case LOCAL1:
            e->type = var_e;
            break;
        case GLOBAL:
            e->type = var_e;
            break;
        case FORMAL:
            e->type = var_e;
            break;
        case LIBFUNC:
            e->type = libraryfunc_e;
            break;
        case USERFUNC:
            e->type = programfunc_e;
            break;
        default:
            assert(0);
            break;
    }
    e->sym = sym;
    return e;
}

expr* member_item(expr* lv,string name,HashTable* table, unsigned scope, unsigned line){
    lv=emit_iftableitem(lv,table,scope,line);
    expr* ti=newexpr(tableitem_e);
    ti->sym=lv->sym;
    ti->index=newexpr(tableitem_e,name); //????tableitem_e
    return ti;
}


void emit(iopcode op, expr* result, expr* arg1, expr* arg2, unsigned label, unsigned line){
    quad* q = new quad;
    q->op = op;
    q->result = result;
    q->arg1 = arg1;
    q->arg2 = arg2;
    q->label = label;
    q->line = line;
    quads.push_back(q);
    currquad++;
}

expr* emit_iftableitem(expr* e,HashTable* table, unsigned scope, unsigned line){
    if(e->type!=tableitem_e){
        return e;
    }
    expr* result=newexpr(var_e,newtemp(table,scope));
    emit(tablegetelem,result,e,e->index,0,line);
    return result;
}

expr* make_call(expr* lv, expr* reversed_elist,HashTable* table, unsigned scope, unsigned line){
    expr* func=emit_iftableitem(lv,table,scope,line);

    while(reversed_elist){
        if(reversed_elist->sym!=NULL&&(reversed_elist->sym->value.varVal!=NULL||reversed_elist->sym->value.funcVal!=NULL)){
            emit(param,reversed_elist,NULL,NULL,0,line);
        }else if(reversed_elist->type==constnum_e||reversed_elist->type==constbool_e||reversed_elist->type==conststring_e||reversed_elist->type==nil_e){
            emit(param,reversed_elist,NULL,NULL,0,line);
        }
        reversed_elist=reversed_elist->next;
    }
    emit(call,func,NULL,NULL,0,line);
    expr* result=newexpr(var_e,newtemp(table,scope));
    emit(getretval,result,NULL,NULL,0,line);
    return result;
}

const char* newtempname(){
    string* tmp=new string();
    *tmp = "_t" + to_string(tempcount++);
    return tmp->c_str();
}

void resettemp(){
    tempcount = 0;
}

SymbolTableEntry* newtemp(HashTable *table,unsigned int scope){
    //later for hidden temp variables
    const char * name = newtempname();
    
    SymbolTableEntry *e=table->lookup(name,scope);
    if(e==NULL){
        table->insert(true,GLOBAL,name,scope,0);
        e=table->lookup(name,scope);
        e->space=currscopespace();
        e->offset=currscopeoffset();
        incurrscopeoffset();
    }
    
    
    return e;
}

expr* tempcheck(expr_t type, expr* a, HashTable* table,unsigned scope , unsigned line){
    string *tmp;
    if(a){
        if(a->type!=constnum_e && a->type!=constbool_e && a->type!=conststring_e && a->type<=12){
           
               
            if(a->sym!=NULL){
               
                    tmp=new string(a->sym->value.varVal->name);
                    if((*tmp)[0]=='_'){
                        return a;
                    }
                
            }
        }
    }

    return  newexpr(type, newtemp(table,scope));
}

expr* tempcheck(expr_t type, expr* a, expr* b, HashTable* table,unsigned scope , unsigned line){
    string *tmp;
    if(a){
        if(a->type!=constnum_e && a->type!=constbool_e && a->type!=conststring_e && a->type<=12){
            if(a->sym!=NULL){
                   
                        tmp=new string(a->sym->value.varVal->name);
                        if((*tmp)[0]=='_'){
                            return a;
                        }
                    

            }
        }
    }
    if(b&&!a){
        if(b->type!=constnum_e&&b->type!=constbool_e&&b->type!=conststring_e && b->type<=12){
            if(b->sym!=NULL){
                
                    tmp=new string(b->sym->value.varVal->name);
                    if((*tmp)[0]=='_'){
                        return b;
                    }
                
            }
        }
    }
    return  newexpr(type, newtemp(table,scope));
}
void emit_list(expr* t ,expr* list,int i,unsigned yylineno){
    if(list==NULL){
        return;
    }
    emit_list(t,list->next,i-1,yylineno);
    emit(tablesetelem,t,newexpr(constnum_e,(double)i),list,0,yylineno);
}
void check_arith (expr* e,const char* file,unsigned line) {
    if ( e->type == constbool_e ||e->type == conststring_e ||e->type == nil_e ||e->type == newtable_e ||e->type == programfunc_e ||e->type == libraryfunc_e ||e->type == boolexpr_e )
        comperror("Illegal expr used with unary operator!",file,line);
}

int newlist(int i){
    quads[i]->label=0;
    return i;
}
int mergelist (int l1, int l2) {
    if (l1<=0)
        return l2;
    else if (l2<=0)
        return l1;
    else {
        int i = l1;
        while (quads[i]->label)
            i = quads[i]->label;
        quads[i]->label = l2;
        return l1;
    }
}
void patchlist (int list, int label) {
    while (list) {
    int next = quads[list]->label;
    quads[list]->label = label;
    list = next;
    }
}
void comperror(const char*msg,const char* file,unsigned line){
    fprintf(stderr,"Compilation Error - %s at line %d : ",file,line);
    fprintf(stderr,"%s\n",msg);
    exit(-1);
}
static void printArg(expr* exp){
    if(!exp)   return;
    if(exp->type==var_e){
        if(exp->sym->value.varVal==NULL&&exp->sym->value.funcVal==NULL) return;
        if(exp->sym->type==GLOBAL||exp->sym->type==LOCAL1){
            cout<<exp->sym->value.varVal->name<<" ";
        }
        else{
            cout<<exp->sym->value.funcVal->name<<" ";
        }
    }else if(exp->type==constnum_e){
        if(exp->sym!=NULL){
            if(exp->sym->type==GLOBAL||exp->sym->type==LOCAL1){
                cout<<exp->sym->value.varVal->name<<" ";
            }
            else{
                cout<<exp->sym->value.funcVal->name<<" ";
            }
        }else{
            cout<<exp->numConst<<" ";
        }
    }else if (exp->type==conststring_e){
        cout<<exp->charConst<<" ";
    }else if (exp->type==constbool_e){
        if(exp->boolConst==true){
            cout<<"true ";
        }else{
            cout<<"false ";
        }
    }else if (exp->type==nil_e){
        cout<<"nil ";
    }else if(exp->type==tableitem_e){
        if(exp->sym==NULL){
             cout<<exp->charConst<<" ";
        }else{
            if(exp->sym->type==GLOBAL||exp->sym->type==LOCAL1){
                cout<<exp->sym->value.varVal->name<<" ";
            }
            else{
                cout<<exp->sym->value.funcVal->name<<" ";
            }
        }
    }else{
        if(exp->sym->type==GLOBAL||exp->sym->type==LOCAL1){
            cout<<exp->sym->value.varVal->name<<" ";
        }
        else{
            cout<<exp->sym->value.funcVal->name<<" ";
        }
    }
}
static void getOp(enum iopcode op){
    switch(op){
            case assign:
                cout<<"ASSIGN ";
                break;
            case add:
                cout<<"ADD ";
                break;
            case sub:
                cout<<"SUB ";
                break;
            case mul:
                cout<<"MUL ";
                break;
            case Div:
                cout<<"DIV ";
                break;
            case mod:
                cout<<"MOD ";
                break;
            case uminus:
                cout<<"UMINUS ";
                break;
            case And:
                cout<<"AND ";
                break;
            case Or:
                cout<<"OR ";
                break;
            case Not:
                cout<<"NOT ";
                break;
            case if_eq:
                cout<<"IF_EQ ";
                break;
            case if_noteq:
                cout<<"IF_NOTEQ ";
                break;
            case if_lesseq:
                cout<<"IF_LESSEQ ";
                break;
            case if_greatereq:
                cout<<"IF_GREATEREQ ";
                break;
            case if_less:
                cout<<"IF_LESS ";
                break;
            case if_greater:
                cout<<"IF_GREATER ";
                break;
            case call:
                cout<<"CALL ";
                break;
            case param:
                cout<<"PARAM ";
                break;
            case ret:
                cout<<"RETURN ";
                break;
            case getretval:
                cout<<"GETRETVAL ";
                break;
            case funcstart:
                cout<<"FUNCSTART ";
                break;
            case funcend:
                cout<<"FUNCEND ";
                break;
            case tablecreate:
                cout<<"TABLECREATE ";
                break;
            case jump:
                cout<<"JUMP ";
                break;
            case tablegetelem:
                cout<<"TABLEGETELEM ";
                break;
            case tablesetelem:
                cout<<"TABLESETELEM ";
                break;
            default:
                break;
        }
}
void printQuads(){
    quad *q;
    long unsigned int l=quads.size();
    for(long unsigned int i=0;i<l;i++){
        q=quads[i];
        cout<<i+1<<": ";
        getOp(q->op);
        printArg(q->result);
        printArg(q->arg1);
        printArg(q->arg2);
        if((q->arg2==NULL||q->result==NULL)&&q->label>0){
            cout<<q->label<<" ";
        }
        cout<<"[line "<< q->line<<"]"<<endl;

    }
}
