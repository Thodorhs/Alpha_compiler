#define  MAGIC_NUMBER 0xC0DE  
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211
#define const_getnumber(index) const_numbers[index]
#define const_getstring(index) const_strings[index]
#define libfuncs_getused(index) libfuncs[index]
#define AVM_STACKENV_SIZE 4
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <vector>
#include "../target/target.hpp"
#include <iostream>
using namespace std;

typedef enum avm_memcell_t
{
    number_m = 0,
    string_m = 1,
    bool_m = 2,
    table_m = 3,
    userfunc_m = 4,
    libfunc_m = 5,
    nil_m = 6,
    undef_m = 7
} avm_memcell_t;
typedef struct avm_memcell
{
    avm_memcell_t type;
    union
    {
        double numVal;
        string *strVal;
        bool boolVal;
        struct avm_table *tableVal;
        unsigned funcVal;
        string *libfuncVal;
    } data;

} avm_memcell;
//
typedef struct avm_table_bucket{
    avm_memcell key;
    avm_memcell value;
}avm_table_bucket;

typedef struct avm_table{
    unsigned refCounter;
    vector <avm_table_bucket> *strIndexed[AVM_TABLE_HASHSIZE];
    vector <avm_table_bucket> *numIndexed[AVM_TABLE_HASHSIZE];
    vector <avm_table_bucket> *userfuncIndexed[AVM_TABLE_HASHSIZE];
    vector <avm_table_bucket> *libFuncIndexed[AVM_TABLE_HASHSIZE];
    vector <avm_table_bucket> *tableIndexed[AVM_TABLE_HASHSIZE];
    vector <avm_table_bucket> *boolIndexed[2];
    unsigned total;
}avm_table;


void avm_memcellclear(avm_memcell * m);
avm_table *avm_tablenew(void);
void avm_tabledestroy(avm_table *t);
avm_memcell *avm_tablegetelem(avm_table *table,avm_memcell *key);
void avm_tablesetelem(avm_table *table,avm_memcell *key, avm_memcell *value);
void avm_tableincrefcounter(avm_table *t);
void avm_tabledecrefcounter(avm_table *t);
void avm_tablebucketsinit(vector <avm_table_bucket> **list);
void avm_tablebucketsdestroy(vector <avm_table_bucket> **list);
avm_memcell* avm_translate_operand(vmarg* arg,avm_memcell* reg);
void execute_cycle(void);
void execute_assign(instruction *instr);
void execute_add(instruction *instr);
void execute_sub(instruction *instr);
void execute_mul(instruction *instr);
void execute_div(instruction *instr);
void execute_mod(instruction *instr);
void execute_jump(instruction *instr);
void execute_jeq(instruction *instr);
void execute_jne(instruction *instr);
void execute_jle(instruction *instr);
void execute_jge(instruction *instr);
void execute_jlt(instruction *instr);
void execute_jgt(instruction *instr);
void execute_call(instruction *instr);
void execute_pusharg(instruction *instr);
void execute_funcenter(instruction *instr);
void execute_funcexit(instruction *instr);
void execute_newtable(instruction *instr);
void execute_tablegetelem(instruction *instr);
void execute_tablesetelem(instruction *instr);
void execute_assign(instruction *instr);
void execute_nop(instruction *instr);


void avm_warning(const char *format, ...);
void avm_assign(avm_memcell *lv, avm_memcell *rv);
void avm_error(char* format,...);
void avm_calllibfunc(string funcname);
void avm_callsaveenviroment(void);
string avm_tostring(avm_memcell* m);

void avm_dec_top(void);
void avm_push_envvalue(unsigned val);