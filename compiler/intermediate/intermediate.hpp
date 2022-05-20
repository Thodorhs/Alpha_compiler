#ifndef __INTERMEDIATE__H__
#define __INTERMEDIATE__H__
#include "../yacc/hashtable.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <stack> 

using namespace std;

typedef enum iopcode{
    assign,         add,            sub,
    mul,            Div,           mod,
    uminus,         And,            Or,
    Not,            if_eq,          if_noteq,
    if_lesseq,      if_greatereq,   if_less,
    if_greater,     call,           param,
    ret,            getretval,      funcstart,
    funcend,        tablecreate,    jump,
    tablegetelem,   tablesetelem
} iopcode;

typedef enum expr_t{
    var_e=0,
    tableitem_e,

    programfunc_e,
    libraryfunc_e,

    arithexpr_e,
    boolexpr_e,
    assignexpr_e,
    newtable_e,

    constnum_e,
    constbool_e,
    conststring_e,

    nil_e
} expr_t;

typedef struct expr{
    expr_t type;
    SymbolTableEntry* sym;
    struct expr * index;
    double numConst;
    string charConst;
    bool boolConst;
    struct expr * next;
    vector <unsigned>true_l;
    vector <unsigned>false_l;
} expr;//to be vector

typedef struct quad{
    iopcode op;
    expr * result;
    expr * arg1;
    expr * arg2;
    unsigned label;
    unsigned line;
} quad;
int newlist(int i);
int mergelist (int l1, int l2);
void patchlist (int list, int label);
const char* name(SymbolTableEntry *e);
expr* newexpr(expr_t type);
expr* newexpr(expr_t type, double numConst);
expr* newexpr(expr_t type, string charConst);
expr* newexpr(expr_t type, bool boolConst);
expr* newexpr(expr_t type, SymbolTableEntry* sym);
expr* newexpr(SymbolTableEntry* sym);
expr* member_item(expr* lv,string name,HashTable* table,unsigned scope,unsigned line);
void emit(iopcode op, expr* result, expr* arg1, expr* arg2, unsigned label, unsigned line);
expr* emit_iftableitem(expr* e,HashTable* table, unsigned scope, unsigned line);
bool check_arith_expr(expr *a, expr *b);
expr_t const_check(expr *a, expr *b);
expr* make_call(expr* lv, expr* reversed_elist, HashTable* table, unsigned scope, unsigned line);
const char* newtempname();
void resettemp();
void resettemp(int i);
void printQuads();
SymbolTableEntry* newtemp(HashTable* table, unsigned int scope);
expr* tempcheck(expr_t type, expr* a, expr* b, HashTable* table,unsigned scope , unsigned line);
expr* tempcheck(expr_t type, expr* a, HashTable* table,unsigned scope , unsigned line);
void emit_list(expr* t ,expr* list,int i,unsigned yylineno);
bool type_check(expr* a, expr* b);
void check_arith (expr* e,const char* file,unsigned line);
void comperror(const char*msg,const char* file,unsigned line);
void setTempcount(int x);
scopespace_t currscopespace(void);
unsigned currscopeoffset(void);
void incurrscopeoffset(void);
void enterscopespace(void);
void exitscopespace(void);
unsigned nextquadlabel(void);
void patchlabel(unsigned quadNo, unsigned label);
void save_offsets();
void reset_offsets();
void backpatch(vector<expr*>&list, unsigned label);
void to_bool(expr *a,unsigned line);
void emitR(iopcode type, expr *res, expr *a, expr *b,unsigned yylineno);
vector <unsigned> merge(vector <unsigned> l1,vector <unsigned>l2);
void backpatch(vector<unsigned>&list, unsigned label);
#endif