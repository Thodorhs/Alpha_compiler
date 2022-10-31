#include "target.hpp"
#include<fstream>
#include <assert.h>
#define MAGIC_NUMBER 0xC0DE
vector <instruction> instructions;
vector<string> const_strings;
vector<double> const_numbers;
vector <userfunc> ufuncs;
vector<string> libfuncs;
extern vector <quad*> quads;
vector<incomplete_jump> incomplete_jumps;
unsigned currprocessedquad=0;
stack <unsigned> funcstack;



unsigned newufunc(userfunc s){
    ufuncs.push_back(s);
    return ufuncs.size()-1;
}
unsigned consts_newstring(string s){
    const_strings.push_back(s);
    return const_strings.size()-1;
}

unsigned consts_newnumber(double n){

    const_numbers.push_back(n);
    return const_numbers.size()-1;
}
unsigned libfuncs_newused(string s){
    for(unsigned i=0;i<libfuncs.size();i++){
        if(libfuncs[i]==s){
            return i;
        }
    }
    libfuncs.push_back(s);
    return libfuncs.size()-1;
}

void make_operand(expr* e , vmarg* arg){
    if(e!=NULL){
    switch (e->type){
        case var_e:
            arg->val=e->sym->offset;
            switch (e->sym->space)
            {
            case programvar:
                arg->type=global_a;
                break;
            case functionlocal:
                arg->type=local_a;
                break;
            case formalarg:
                arg->type=formal_a;
                break;
            default:
                assert(0);
                break;
            }
            break;
        case tableitem_e:
            arg->val=e->sym->offset;
            switch (e->sym->space)
            {
            case programvar:
                arg->type=global_a;
                break;
            case functionlocal:
                arg->type=local_a;
                break;
            case formalarg:
                arg->type=formal_a;
                break;
            default:
                assert(0);
                break;
            }
            break;
        case arithexpr_e:
            arg->val=e->sym->offset;
            switch (e->sym->space)
            {
            case programvar:
                arg->type=global_a;
                break;
            case functionlocal:
                arg->type=local_a;
                break;
            case formalarg:
                arg->type=formal_a;
                break;
            default:
                assert(0);
                break;
            }
            break;
        case boolexpr_e:
            arg->val=e->sym->offset;
            switch (e->sym->space)
            {
            case programvar:
                arg->type=global_a;
                break;
            case functionlocal:
                arg->type=local_a;
                break;
            case formalarg:
                arg->type=formal_a;
                break;
            default:
                assert(0);
                break;
            }
            break;
        case newtable_e:
            arg->val=e->sym->offset;
            switch (e->sym->space)
            {
            case programvar:
                arg->type=global_a;
                break;
            case functionlocal:
                arg->type=local_a;
                break;
            case formalarg:
                arg->type=formal_a;
                break;
            default:
                assert(0);
                break;
            }
            break;
        case constbool_e:
            arg->val=(unsigned)e->boolConst;
            arg->type=bool_a;
            break;
        case conststring_e:
            arg->val=consts_newstring(e->charConst);
            arg->type=string_a;
            break;
        case constnum_e:
            arg->val=consts_newnumber(e->numConst);
            arg->type=number_a;
            break;
        case nil_e:
            arg->type=nil_a;
            break;
        case programfunc_e:
            arg->type=userfunc_a;
            arg->val=e->sym->taddress; //array???
            break;
        case libraryfunc_e:
            arg->type=libfunc_a;
            arg->val=libfuncs_newused(string(e->sym->value.funcVal->name));
            break;
        case assignexpr_e:
            arg->val=e->sym->offset;
            switch (e->sym->space)
            {
            case programvar:
                arg->type=global_a;
                break;
            case functionlocal:
                arg->type=local_a;
                break;
            case formalarg:
                arg->type=formal_a;
                break;
            default:
                assert(0);
                break;
            }
            break;
        default:
            assert(0);
            break;
    }
}
}


void make_numberoperand (vmarg* arg, double val) {
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}

void make_booloperand (vmarg* arg, unsigned val) {
    arg->val = val;
    arg->type = bool_a;
}

void make_retvaloperand (vmarg* arg) {
    arg->type = retval_a;
    arg->val=0;
}

void add_incomplete_jump(unsigned instrNo, unsigned iaddress){
    incomplete_jump j;
    j.instrNo=instrNo;
    j.iaddress=iaddress;
    incomplete_jumps.push_back(j);
}
void patch_incomplete_jumps(){
    for (unsigned i=0; i<incomplete_jumps.size(); i++){
        if(incomplete_jumps[i].iaddress-1==quads.size()){
            instructions[incomplete_jumps[i].instrNo].result.val=instructions.size();
        }else{
            instructions[incomplete_jumps[i].instrNo].result.val=quads[incomplete_jumps[i].iaddress-1]->taddress;
        }
    }
}
void emit(instruction instr){
    instructions.push_back(instr);
}
unsigned nextinstructionlabel(){
    return instructions.size();//to use a variable
}
void generate(vmopcode op,quad* q){
    instruction instr;
    instr.opcode=op;
    make_operand(q->arg1,&instr.arg1);
    make_operand(q->arg2,&instr.arg2);
    make_operand(q->result,&instr.result);
    q->taddress=nextinstructionlabel();
    instr.srcLine=q->line;
    emit(instr);
}
void generate_ADD(quad* q) { generate(add_v, q); }
void generate_SUB(quad* q) { generate(sub_v, q); }
void generate_MUL(quad* q) { generate(mul_v, q); }
void generate_DIV(quad* q) { generate(div_v, q); }
void generate_MOD(quad* q) { generate(mod_v, q); }
void generate_UMINUS(quad* q) {
    q->op=mul;
    q->arg2=newexpr(constnum_e,(double)-1);
    generate(mul_v, q);
}

void generate_NEWTABLE (quad* q) { generate(newtable_v, q); }
void generate_TABLEGETELEM (quad* q) { generate(tablegetelem_v, q); }
void generate_TABLESETELEM (quad* q) { generate(tablesetelem_v, q); }
void generate_ASSIGN (quad* q) { generate(assign_v, q); } 
void generate_NOP (quad* q) { instruction t; t.opcode = nop_v; emit(t); }

void generate_JUMP(quad* q) { generate_relational(jump_v, q); }
void generate_IF_EQ(quad* q) { generate_relational(jeq_v, q); }
void generate_IF_NOTEQ(quad* q) { generate_relational(jne_v, q); }
void generate_IF_GREATER(quad* q) { generate_relational(jgt_v, q); }
void generate_IF_GREATEREQ(quad* q) { generate_relational(jge_v, q); }
void generate_IF_LESS(quad* q) { generate_relational(jlt_v, q); }
void generate_IF_LESSEQ(quad* q) { generate_relational(jle_v, q); }
void generate_relational(vmopcode op, quad* q){
    instruction t;
    t.opcode=op;
    if(q->result==NULL){
        make_operand(q->arg2,&t.arg1);
    }else{
        make_operand(q->result,&t.arg1);
    }
    make_operand(q->arg1,&t.arg2);
    t.result.type=label_a;
    if(q->arg2&&q->label==(unsigned)0){
        if(q->arg2->numConst<currprocessedquad){
            t.result.val=quads[q->arg2->numConst]->taddress;
        }else{
            add_incomplete_jump(nextinstructionlabel(),q->arg2->numConst);
        }
        
    }else{
       if(q->label<currprocessedquad){
            t.result.val=quads[q->label]->taddress;
        }else{
            add_incomplete_jump(nextinstructionlabel(),q->label);
        }
    }
    
    q->taddress=nextinstructionlabel();
    emit(t);
}
void generate_PARAM(quad* q) { 
    q->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=pusharg_v;
    make_operand(q->result,&t.arg1);
    emit(t);
}
void generate_CALL(quad* q) {
    q->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=call_v;
    if(funcstack.size()!=0){
        //q->result->sym->taddress=funcstack.top();
    }
    make_operand(q->result,&t.arg1);
    emit(t);
}
void generate_GETRETVAL(quad* q) {
    q->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=assign_v;
    make_operand(q->result,&t.result);
    make_retvaloperand(&t.arg1);
    emit(t);
}
void generate_FUNCSTART(quad* q) {
    q->taddress=nextinstructionlabel();
    q->arg1->sym->taddress=q->taddress;
    instruction t;
    t.opcode=funcenter_v;
    funcstack.push(q->taddress);
    make_operand(q->arg1,&t.result);
    emit(t);
}
void generate_RETURN(quad *q){
    q->taddress=nextinstructionlabel();
    instruction t;
    t.opcode=assign_v;
    make_retvaloperand(&t.result);
    make_operand(q->result,&t.arg1);
    emit(t);
}
void generate_FUNCEND(quad* q) {
    q->taddress=nextinstructionlabel();
    instruction t;
    userfunc f;
    f.id=q->arg1->sym->value.funcVal->name;
    f.totalLocals=q->arg1->sym->totalLocals;
    t.opcode=funcexit_v;
    make_operand(q->arg1,&t.result);
    f.taddress=t.result.val;
    ufuncs.push_back(f);
    emit(t);
}

typedef void (*generator_func_t)(quad*);

generator_func_t generators[] = {
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_ASSIGN,
    generate_NOP,
    generate_JUMP,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_GREATER,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_LESSEQ,
    generate_PARAM,
    generate_CALL,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_RETURN,
    generate_FUNCEND
};

void generate(void) {
    ofstream out("../out/instructions.txt");
    instructions.push_back(instruction());
    long unsigned int i;
    for (i = 0; i < nextquadlabel(); ++i) {
        generators[quads[i]->op](quads[i] );
        currprocessedquad++;
    }
    patch_incomplete_jumps();
    print_instructions();
}
void print_instructions() {
    FILE* fd;
    fd=fopen("../out/out.bin","wb");

    unsigned i;
    /*char types[12][20] = {
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
        "unused_a"};*/
    //printf("PINAKES:\n");
    unsigned a;
    a=(unsigned)MAGIC_NUMBER;
    fwrite(&a,sizeof(unsigned),1,fd);
    //printf("======================================\n");
    a=const_strings.size();
    fwrite(&a, sizeof(unsigned),1, fd);
    for(i=0;i<const_strings.size();i++){
        a=strlen(const_strings[i].c_str());
        fwrite(&a, sizeof(unsigned),1, fd);
        fwrite( const_strings[i].c_str() , sizeof(char),strlen(const_strings[i].c_str()), fd);
        //printf("const_string_%d = %s\n",i,const_strings[i].c_str());
    }

    //printf("======================================\n");
    a=const_numbers.size();
    fwrite(&a, sizeof(unsigned),1, fd);
    for(i=0;i<const_numbers.size();i++){
        fwrite(&const_numbers[i], sizeof(double),1, fd);
        //printf("const_numbers_%d = %d\n",i,(int)const_numbers[i]);
    }

    //printf("======================================\n");
    a=libfuncs.size();
    fwrite(&a, sizeof(unsigned),1, fd);
    for(i=0;i<libfuncs.size();i++){
    a=strlen(libfuncs[i].c_str());
    fwrite(&a, sizeof(unsigned),1, fd);
    fwrite(libfuncs[i].c_str() , sizeof(char),strlen(libfuncs[i].c_str()), fd);
         //printf("libfuncs_%d = %s\n",i,libfuncs[i].c_str());
    }
    //printf("======================================\n");
    a=ufuncs.size();
    fwrite(&a, sizeof(unsigned),1, fd);
    for(i=0;i<ufuncs.size();i++){
        a=strlen((ufuncs[i].id).c_str());
        fwrite(&a, sizeof(unsigned),1, fd);
        fwrite(ufuncs[i].id.c_str(),sizeof(char), strlen((ufuncs[i].id).c_str()), fd);
        fwrite(&(ufuncs[i].totalLocals), sizeof(unsigned),1, fd);
        fwrite(&(ufuncs[i].taddress), sizeof(unsigned),1, fd);
        //printf("ufuncs_%d = %s locals=%d taddress=%d\n",i,ufuncs[i].id.c_str(),ufuncs[i].totalLocals,ufuncs[i].taddress);
    }
    //printf("======================================\n");
    //printf("Line: | OP | TYPE(result value) | TYPE(arg1 value) | TYPE(arg2 value)\n");
    a=instructions.size();
    fwrite(&a, sizeof(unsigned),1, fd);
    for(i=1;i<instructions.size();i++){
        fwrite((struct instruction*) &instructions[i], sizeof(struct instruction),1, fd);
      switch (instructions[i].opcode) {
            case assign_v:
                if(instructions[i].arg1.type == number_a){
                    //printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }else if(instructions[i].arg1.type == string_a){
                    //printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }else if(instructions[i].arg1.type == userfunc_a){
                    //printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);//????????????????????? need to make userfunc array ......
                }else if(instructions[i].arg1.type == libfunc_a){
                    //printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }else if(instructions[i].arg1.type == retval_a){
                    //printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type],instructions[i].arg1.val);
                }else{
                    //printf("#%d: assign_v [result: %s, %d] [arg1: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val);
                }
                break;
            case add_v:
                //printf("#%d: add_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case sub_v:
                //printf("#%d: sub_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case mul_v:
                  //printf("#%d: mul_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case div_v:
                  //printf("#%d: div_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case mod_v:
                 //printf("#%d: mod_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case jeq_v:
                //printf("#%d: jeq_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jne_v:
                //printf("#%d:  jne_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jle_v:
                //printf("#%d: jle_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jge_v:
                //printf("#%d: jge_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jlt_v:
                //printf("#%d: jlt_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case jgt_v:
                //printf("#%d: jgt_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val-1, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);//change after
                break;
            case call_v:
                //printf("#%d: call_v [arg1: %s, %d]\n", i, types[instructions[i].arg1.type], instructions[i].arg1.val);
                break;
            case pusharg_v:
                 //printf("#%d: pusharg_v [arg1: %s, %d]\n", i, types[instructions[i].arg1.type], instructions[i].arg1.val);
                break;
            case funcenter_v:
                //printf("#%d: funcenter_v [result: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val);
                break;
            case funcexit_v:
                //printf("#%d: funcexit_v [result: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val);
                break;
            case newtable_v:
                //printf("#%d: newtable_v [result: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val);
                break;
            case tablegetelem_v:
                //printf("%d: tablegetelem_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case tablesetelem_v:
                //printf("#%d: tablesetelem_v [result: %s, %d] [arg1: %s, %d] [arg2: %s, %d]\n", i, types[instructions[i].result.type], instructions[i].result.val, types[instructions[i].arg1.type], instructions[i].arg1.val, types[instructions[i].arg2.type], instructions[i].arg2.val);
                break;
            case nop_v:
                //printf("#%d: nop_v \n", i);
                break;
            case jump_v:
                //printf("#%d: jump_v [result: %s, %d]\n", i,types[instructions[i].result.type], instructions[i].result.val-1);//change after
                break;
            default:
                //printf("#%d: for some reason im here(default in print switch)\n", i);
                break;
        }
    }
    fclose(fd);
}