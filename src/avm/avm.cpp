#include "avm.hpp"

avm_memcell *_stack;
avm_memcell ax,bx,cx;
avm_memcell retval;
unsigned top;
unsigned topsp;
unsigned totalActuals=0;

vector <instruction> instructions;
vector<string> const_strings;
vector<double> const_numbers;
vector<string> libfuncs;
vector <struct userfunc> ufuncs;
/*
#define  AVM_ENDING_PC codeSize
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
#define AVM_NUMACTUALS_OFFSET +4
#define AVM_SAVEDPC_OFFSET +3
#define AVM_SAVEDTOP_OFFSET +2
#define AVM_SAVEDTOPSP_OFFSET +1

static void avm_initstack(void){
    _stack = (avm_memcell *)malloc(AVM_STACKSIZE* sizeof(avm_memcell));
    for(unsigned i=0;i<AVM_STACKSIZE;i++){
        AVM_WIPEOUT(_stack[i]);
        _stack[i].type = undef_m;
    }
}
avm_table *avm_tablenew(void){
    avm_table* t=new avm_table;
    t->refCounter=0;
    t->total=0;
    avm_tablebucketsinit(t->strIndexed);
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->userfuncIndexed);
    avm_tablebucketsinit(t->libFuncIndexed);
    avm_tablebucketsinit(t->tableIndexed);
    avm_tablebucketsinit(t->boolIndexed);
    return t;
}
void avm_tabledestroy(avm_table *t){
    avm_tablebucketsdestroy(t->strIndexed);
    avm_tablebucketsdestroy(t->numIndexed);
    avm_tablebucketsdestroy(t->userfuncIndexed);
    avm_tablebucketsdestroy(t->libFuncIndexed);
    avm_tablebucketsdestroy(t->tableIndexed);
    avm_tablebucketsdestroy(t->boolIndexed);
    delete t;
}
#define HASH_MULTIPLIER 65999
static unsigned int hashS(const char *key){
   size_t ui;
   unsigned int uiHash = 0U;
    for (ui = 0U; key[ui] != '\0'; ui++){
        uiHash = uiHash * HASH_MULTIPLIER + key[ui];
    }
    return uiHash%AVM_TABLE_HASHSIZE;   
}
static unsigned int hashN(double key){
    return (unsigned int)key%AVM_TABLE_HASHSIZE;
}
static unsigned int hashP(avm_table* key){
    return (intptr_t)key%AVM_TABLE_HASHSIZE;
}
avm_memcell *avm_tablegetelem(avm_table *table,avm_memcell *key){

    switch(key->type){
        case number_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashN(key->data.numVal)])[i].key.data.numVal==key->data.numVal){
                    return &(*table->numIndexed[hashN(key->data.numVal)])[i].value;
                }
            }
            break;
        case string_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashS(key->data.strVal->c_str())])[i].key.data.strVal==key->data.strVal){
                    return &(*table->numIndexed[hashS(key->data.strVal->c_str())])[i].value;
                }
            }
        case bool_m:
            if(table->boolIndexed[key->data.boolVal]->size()>0)
                return &(*table->boolIndexed[key->data.boolVal])[0].value;
            break;
        case table_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->tableIndexed[hashP(key->data.tableVal)])[i].key.data.tableVal==key->data.tableVal){
                    return &(*table->tableIndexed[hashP(key->data.tableVal)])[i].value;
                }
            }
            break;
        case userfunc_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashS(key->data.strVal->c_str())])[i].key.data.strVal==key->data.strVal&&(*table->numIndexed[hashS(key->data.strVal->c_str())])[i].key.data.funcVal==key->data.funcVal){
                    return &(*table->numIndexed[hashS(key->data.strVal->c_str())])[i].value;
                }
            }
            break;
        case libfunc_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashS(key->data.libfuncVal->c_str())])[i].key.data.libfuncVal==key->data.libfuncVal){
                    return &(*table->numIndexed[hashS(key->data.libfuncVal->c_str())])[i].value;
                }
            }
            break;
    }
        return NULL;


}
void avm_tablesetelem(avm_table *table,avm_memcell *key, avm_memcell *value){
    if(key->type==table_m){
        avm_tableincrefcounter(key->data.tableVal);
    }
    if(value->type==table_m){
        avm_tableincrefcounter(value->data.tableVal);
    }

    switch(key->type){
        case number_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashN(key->data.numVal)])[i].key.data.numVal==key->data.numVal){
                   (*table->numIndexed[hashN(key->data.numVal)])[i].value=*value;
                }
            }
            break;
        case string_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashS(key->data.strVal->c_str())])[i].key.data.strVal==key->data.strVal){
                    (*table->numIndexed[hashS(key->data.strVal->c_str())])[i].value=*value;
                }
            }
        case bool_m:
            if(table->boolIndexed[key->data.boolVal]->size()>0)
                (*table->boolIndexed[key->data.boolVal])[0].value=*value;
            break;
        case table_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->tableIndexed[hashP(key->data.tableVal)])[i].key.data.tableVal==key->data.tableVal){
                    (*table->tableIndexed[hashP(key->data.tableVal)])[i].value=*value;
                }
            }
            break;
        case userfunc_m:
            for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashS(key->data.strVal->c_str())])[i].key.data.strVal==key->data.strVal&&(*table->numIndexed[hashS(key->data.strVal->c_str())])[i].key.data.funcVal==key->data.funcVal){
                  (*table->numIndexed[hashS(key->data.strVal->c_str())])[i].value=*value;
                }
            }
            break;
        case libfunc_m:
             for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
                if((*table->numIndexed[hashS(key->data.libfuncVal->c_str())])[i].key.data.libfuncVal==key->data.libfuncVal){
                    (*table->numIndexed[hashS(key->data.libfuncVal->c_str())])[i].value=*value;
                }
            }
            break;
    }
}
void avm_tableincrefcounter(avm_table *t){
    t->refCounter++;
}
void avm_tabledecrefcounter(avm_table *t){
    assert(t->refCounter>0);
    if((--t->refCounter)==0){
        avm_tabledestroy(t);
    }
}
void avm_tablebucketsinit(vector <avm_table_bucket> **list){
    for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
        list[i]=new vector <avm_table_bucket>;
    }
}
void avm_memcellclear(avm_memcell * m){
    if(m->type==undef_m){
        return;
    }else if (m->type==table_m){
        assert(m->data.tableVal);
        avm_tabledecrefcounter(m->data.tableVal);
    }else if (m->type==string_m){
        assert(m->data.strVal);
        delete m->data.strVal;
    }else{
        return;
    }
}
void avm_tablebucketsdestroy(vector <avm_table_bucket> **list){
    for(unsigned i=0;i<AVM_TABLE_HASHSIZE;i++){
        avm_memcellclear(&((*list[i])[0].key));
        avm_memcellclear(&((*list[i])[0].value));
        delete list[i];
    }

}

avm_memcell* avm_translate_operand(vmarg* arg,avm_memcell* reg){
    switch(arg->type){
        case global_a:
            return &_stack[AVM_STACKSIZE-1- arg->val];
        case formal_a:
            return &_stack[topsp+AVM_STACKSIZE+1+ arg->val];
        case local_a:
            return &_stack[topsp-arg->val];
        case retval_a:
            return &retval;
        case number_a:
            reg->type=number_m;
            reg->data.numVal=const_numbers[arg->val];
            return reg;
        case string_a:
            reg->type=string_m;
            reg->data.strVal=new string(const_strings[arg->val]);
            return reg;
        case bool_a:
            reg->type=bool_m;
            reg->data.boolVal=arg->val;
            return reg;
        case nil_a:
            reg->type=nil_m;
            return reg;
        case userfunc_a:
            reg->type=userfunc_m;
            reg->data.funcVal=arg->val;
            return reg;
        case libfunc_a:
            reg->type=libfunc_m;
            reg->data.libfuncVal=new string(const_strings[arg->val]);
            return reg;
        default:
            assert(0);
    }
}

bool executionFinished=false;
unsigned pc=0;
unsigned currLine=0;
unsigned codeSize=0;
typedef void (*execute_func_t)(instruction*);




void avm_warning(const char* format,...){
    fprintf(stderr,"#%d warning: %s\n",currLine,format);
    return;
}
void execute_assign(instruction *instr){
    avm_memcell *lv =avm_translate_operand(&instr->result,NULL);
    avm_memcell *rv =avm_translate_operand(&instr->arg1,&ax);
    assert(lv&&(&_stack[AVM_STACKSIZE-1]>=lv&&lv>=&_stack[top]||lv==&retval));
    assert(rv&&(&_stack[AVM_STACKSIZE-1]>=rv&&rv>=&_stack[top]||rv==&retval));
    avm_assign(lv,rv);
}
void avm_assign(avm_memcell *lv,avm_memcell *rv){
    if(lv==rv){
        return;
    }

    if(lv->type==table_m && rv->type==table_m && lv->data.tableVal==rv->data.tableVal){
        return;
    }

    if(rv->type==undef_m){
        avm_warning("assigning from undef content!");
    }

    avm_memcellclear(lv);
    memcpy(lv,rv,sizeof(avm_memcell));

    if(lv->type==string_m){
        lv->data.strVal=new string(*(rv->data.strVal));
    }else if(lv->type==table_m){
        avm_tableincrefcounter(lv->data.tableVal);
    }
}
void avm_error(string err){
fprintf(stderr,"#%d error: %s\n",currLine,err.c_str());
}
void avm_calllibfunc(string funcname){

}

string avm_tostring(avm_memcell* m){
string err;
    switch(m->type){
        case libfunc_m:
            err=string(*m->data.libfuncVal);
            break;
        case userfunc_m:
            err=string(*m->data.strVal);
            break;
    }
    return err;
}

void execute_call(instruction *instr){
avm_memcell *func =avm_translate_operand(&instr->result,&ax);
assert(func);
avm_callsaveenviroment();
switch(func->type){
    case userfunc_m:
        pc=func->data.funcVal;
        assert(pc<AVM_ENDING_PC);
        assert(instructions[pc].opcode==funcenter_v);
        break;
    case string_m:
        avm_calllibfunc(*func->data.strVal);
        break;
    case libfunc_m:
        avm_calllibfunc(*func->data.libfuncVal);
        break;
    default:
        string err=string("call cannot bind")+avm_tostring(func)+string("to a function");
        avm_error(err);
        executionFinished=true;
}
}

void avm_dec_top(void){
    if(top>0){
        avm_error(string("stack overflow"));
        executionFinished=true;
    }else{
        --top;
    }
}
void avm_push_envvalue(unsigned val){
    _stack[top].type=number_m;
    _stack[top].data.numVal=val;
    avm_dec_top();
}
void avm_callsaveenviroment(void){
    avm_push_envvalue(totalActuals);
    avm_push_envvalue(pc+1);
    avm_push_envvalue(top+totalActuals+2);
    avm_push_envvalue(topsp);
}
struct userfunc* avm_getfuncinfo(unsigned address){
    for(int i=0;ufuncs.size();i++){
        if(ufuncs[i].taddress==address){
            return &ufuncs[i];
        }
    }
    assert (0);
    return NULL;
}
void execute_funcenter(instruction *instr){
    avm_memcell*func=avm_translate_operand(&instr->result,&ax);
    assert(func);
    assert(pc==func->data.funcVal);
    totalActuals=0;
    struct userfunc* funcInfo=avm_getfuncinfo(pc);
    topsp=top;
    top=top+funcInfo->totalLocals;
}
unsigned avm_get_envvalue(unsigned i){
    assert(_stack[i].type==number_m);
    unsigned val=(unsigned)_stack[i].data.numVal;
    assert(_stack[i].data.numVal==((double)val));
    return val;
}
void execute_funcexit(instruction *instr){
    unsigned oldTop=top;
    top=avm_get_envvalue(topsp+AVM_SAVEDTOP_OFFSET);
    pc=avm_get_envvalue(topsp+AVM_SAVEDPC_OFFSET);
    topsp=avm_get_envvalue(topsp+AVM_SAVEDTOPSP_OFFSET);
    while(++oldTop<=top){
        avm_memcellclear(&_stack[oldTop]);
    }

}

typedef void (*library_func_t)(void);
library_func_t avm_getlibraryfunc(string id);
void avm_callibfunc(string id){
    library_func_t f=avm_getlibraryfunc(id);
    if(!f){
        avm_error(string("cannot find library function ")+id);
        executionFinished=true;
    }else{
        topsp=top;
        totalActuals=0;
        (*f)();
        if(!executionFinished){
            execute_funcexit(NULL);
        }
    }
}
unsigned avm_totalactuals(void){
    return avm_get_envvalue(topsp+AVM_NUMACTUALS_OFFSET);
}
avm_memcell* avm_getactual(unsigned i){
    assert(i<avm_totalactuals());
    return &_stack[topsp+AVM_STACKENV_SIZE+1+i];
}
void libfunc_print(void){
    unsigned n=avm_totalactuals();
    for(unsigned i=0;i<n;i++){
        string s =avm_tostring(avm_getactual(i));
        puts(s.c_str());
    }
}
void avm_registerlibfunc(string id,library_func_t func);
execute_func_t *executeFuncs;

execute_func_t executeFuncs[] = {
    execute_add,
    execute_sub,
    execute_mul,
    execute_div,
    execute_mod,
    NULL,
    execute_newtable,
    execute_tablegetelem,
    execute_tablesetelem,
    execute_assign,
    execute_nop,
    execute_jump,
    execute_jeq,
    execute_jne,
    execute_jgt,
    execute_jge,
    execute_jlt,
    execute_jle,
    execute_pusharg,
    execute_call,
    NULL,
    execute_funcenter,
    NULL,
    execute_funcexit
    };


void  execute_cycle(void){
    if(executionFinished){
        return;
    }
    if(pc==AVM_ENDING_PC){
        executionFinished=true;
        return;
    }
    assert(pc<AVM_ENDING_PC);
    instruction* instr=&instructions[pc];
    assert(instr->opcode>=0&&instr->opcode<=AVM_MAX_INSTRUCTIONS);
    if(instr->srcLine){
        currLine=instr->srcLine;
    }
    unsigned oldPc=pc;
    (*executeFuncs[instr->opcode])(instr);
    if(pc == oldPc){
      ++pc;  
    }
}
*/

//=========================================================================================================================================================
void readBinary(){
    FILE* fd;
    char *str=NULL;
    fd=fopen("out.bin","rb");
    if(fd==NULL){
        printf("error opening file\n");
        exit(1);
    }
    
    unsigned m ,a,size;
    double num;
    fread(&m,sizeof(unsigned),1,fd);
    a=MAGIC_NUMBER;
    if(m!=a){
        fprintf(stderr,"Invalid binary code\n");
        exit(1);
    }
    fread(&size, sizeof(unsigned),1, fd);
    for(unsigned i=0;i<size;i++){
        if(str){
            free(str);
            str=NULL;
        }
        fread(&a, sizeof(unsigned),1, fd);
        str=(char*)malloc(a+1);
        fread(str, sizeof(char),a, fd);
        str[a]='\0';
        const_strings.push_back(*(new string(str)));

    }
    fread(&size, sizeof(unsigned),1, fd);
    for(unsigned i=0;i<size;i++){
        fread(&num, sizeof(double),1, fd);
        const_numbers.push_back(num);
    }
    fread(&size, sizeof(unsigned),1, fd);
    for(unsigned i=0;i<size;i++){
        if(str){
            free(str);
            str=NULL;
        }
        fread(&a, sizeof(unsigned),1, fd);
        str=(char*)malloc(a);
        fread(str, sizeof(char),a, fd);
        str[a]='\0';
        libfuncs.push_back(string(str));
    }
    fread(&size, sizeof(unsigned),1, fd);
    struct userfunc f;
    for(unsigned i=0;i<size;i++){
        fread(&a, sizeof(unsigned),1, fd);
        if(str){
            free(str);
            str=NULL;
        }
        str=(char*)malloc(a+1);
        fread(str, sizeof(char),a, fd);
        str[a]='\0';
        f.id=string(str);
        fread(&f.totalLocals, sizeof(unsigned),1, fd);
        fread(&f.taddress, sizeof(unsigned),1, fd);
        ufuncs.push_back(f);
        printf("ufuncs_%d = %s\n",i,ufuncs[i].id.c_str());
        printf("ufuncs_%d = %s locals=%d taddress=%d\n",i,ufuncs[i].id.c_str(),ufuncs[i].totalLocals,ufuncs[i].taddress);
    }
    struct instruction instr;
    fread(&size, sizeof(unsigned),1, fd);
    instructions.push_back(instruction());
    for(unsigned i=0;i<size-1;i++){
        fread(&instr, sizeof(struct instruction),1, fd);
        instructions.push_back(instr);
    }

    unsigned i;
    char types[12][20] = {
        "label_a",
        "global_a",
        "formal_a",
        "local_a",
        "number_a",
        "string_a",
        "bool_a",
        "nil_a",
        "userfunc_a",
        "libfunc_a",
        "retval_a",
        "unused_a"};
    printf("ARRAYS:\n");
    
    printf("======================================\n");
    
    
    for(i=0;i<const_strings.size();i++){
        
        printf("const_string_%d = %s\n",i,const_strings[i].c_str());
    }

    printf("======================================\n");
   
    for(i=0;i<const_numbers.size();i++){
       
        printf("const_numbers_%d = %d\n",i,(int)const_numbers[i]);
    }

    printf("======================================\n");
    
    for(i=0;i<libfuncs.size();i++){
         printf("libfuncs_%d = %s\n",i,libfuncs[i].c_str());
    }

    printf("======================================\n");
    printf("Line: | OP | TYPE(result value) | TYPE(arg1 value) | TYPE(arg2 value)\n");


    for(i=1;i<instructions.size();i++){

      switch (instructions[i].opcode) {
            case assign_v:
                if(instructions[i].arg1.type == number_a){
                    printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }else if(instructions[i].arg1.type == string_a){
                    printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }else if(instructions[i].arg1.type == userfunc_a){
                    printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);//????????????????????? need to make userfunc array ......
                }else if(instructions[i].arg1.type == libfunc_a){
                    printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }else if(instructions[i].arg1.type == retval_a){
                    printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type],instructions[i].arg1.val);
                }else{
                    printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }
                break;
            case add_v:
                printf("#%d: add_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case sub_v:
                printf("#%d: sub_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case mul_v:
                  printf("#%d: mul_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case div_v:
                  printf("#%d: div_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case mod_v:
                 printf("#%d: mod_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case jeq_v:
                printf("#%d: jeq_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jne_v:
                printf("#%d:  jne_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jle_v:
                printf("#%d: jle_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jge_v:
                printf("#%d: jge_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jlt_v:
                printf("#%d: jlt_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jgt_v:
                printf("#%d: jgt_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case call_v:
                printf("#%d: call_v [arg1: %s, %d]\n", i, types[instructions[i].arg1.type], instructions[i].arg1.val);
                break;
            case pusharg_v:
                 printf("#%d: pusharg_v [arg1: %s, %d]\n", i, types[instructions[i].arg1.type], instructions[i].arg1.val);
                break;
            case funcenter_v:
                printf("#%d: funcenter_v [result: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val);
                break;
            case funcexit_v:
                printf("#%d: funcexit_v [result: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val);
                break;
            case newtable_v:
                printf("#%d: newtable_v [result: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val);
                break;
            case tablegetelem_v:
                printf("%d: tablegetelem_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case tablesetelem_v:
                printf("#%d: tablesetelem_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case nop_v:
                printf("#%d: nop_v \n", i);
                break;
            case jump_v:
                printf("#%d: jump_v [result: %s, %d]\n", i,types[instructions[i].result.type], instructions[i].result.val-1);//change after
                break;
            default:
                printf("#%d: for some reason im here(default in print switch)\n", i);
                break;
        }
    }
   
}
int main(int argc, char **argv) {
    readBinary();
    return 0;
}