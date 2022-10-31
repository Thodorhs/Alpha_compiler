#ifndef __SYMTABLE__H__
#define __SYMTABLE__H__
#include <iterator>
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
#define LIBFUNC_NUM 12
#define HASH_MULTIPLIER 65999

using namespace std;

typedef struct Variable {
    const char *name;
    unsigned int scope;
    unsigned int line;
} Variable;

typedef struct Function {
    const char *name;
    //List of arguments
    unsigned int scope;
    unsigned int line;
} Function;

enum SymbolType {
    GLOBAL, LOCAL1, FORMAL,// these 3 are var_s
    USERFUNC //programfunc_s
    , LIBFUNC // libraryfunc_s
};

enum scopespace_t{
    programvar,functionlocal,formalarg
};

typedef struct SymbolTableEntry {
    scopespace_t space;
    unsigned offset;
    bool isActive;
    union {
        Variable *varVal;
        Function *funcVal;
    } value;
    unsigned iaddress;
    unsigned taddress;
    unsigned totalLocals;
    enum SymbolType type;
} SymbolTableEntry;

class HashTable{
    public:
        size_t size;
        list<SymbolTableEntry*>* symbolTable;
        vector<list<SymbolTableEntry*> > scopeLinks;
        HashTable(size_t size);
        unsigned int hash(const char *key);
        void insert(bool active,enum SymbolType type,const char* name,unsigned int scope,unsigned int line);
        SymbolTableEntry *lookup(const char *key);
        SymbolTableEntry *lookup(const char *key,unsigned int scope);
        void hide(size_t scope);
        void print();
        void printScopes();
        
};

extern ofstream file;
#endif