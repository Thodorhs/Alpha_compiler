#include "hashtable.hpp"

ofstream file("symbol_table.txt");
static const char* lib_funcs[]={"print","input","objectmemberkeys","objecttotalmembers","objectcopy","totalarguments","argument","typeof","strtonum","sqrt","cos","sin"};

HashTable::HashTable(size_t size){
    SymbolTableEntry* s;
    this->size = size;
    this->symbolTable = new list<SymbolTableEntry*>[size];
    this->scopeLinks.push_back(list<SymbolTableEntry*>());
    for(int i=0;i<LIBFUNC_NUM;i++){
        s=new SymbolTableEntry;
        s->isActive = true;
        s->type = LIBFUNC;
        s->value.funcVal = new Function;
        s->value.funcVal->name=lib_funcs[i];
        s->value.funcVal->scope = 0;
        s->value.funcVal->line = 0;
        this->symbolTable[this->hash(lib_funcs[i])].push_back(s);
        this->scopeLinks[0].push_back(s);
    }
    
}
unsigned int HashTable::hash(const char *key){
   size_t ui;
   unsigned int uiHash = 0U;
    for (ui = 0U; key[ui] != '\0'; ui++){
        uiHash = uiHash * HASH_MULTIPLIER + key[ui];
    }
    return uiHash%this->size;
}

void HashTable::insert(bool active,enum SymbolType type,const char* name,unsigned int scope,unsigned int line){
    SymbolTableEntry* entry;
    entry=lookup(name,scope);
    if(entry==NULL){
        entry = new SymbolTableEntry;
        entry->isActive = active;
        entry->type = type;
        if(type == USERFUNC || type == LIBFUNC){
            entry->value.funcVal = new Function;
            entry->value.funcVal->name = (new string(name))->c_str();
            entry->value.funcVal->scope = scope;
            entry->value.funcVal->line = line;
        }else{
            entry->value.varVal = new Variable;
            entry->value.varVal->name = (new string(name))->c_str();
            entry->value.varVal->scope = scope;
            entry->value.varVal->line = line;
        }
        this->symbolTable[this->hash(name)].push_back(entry);
    }else{
        entry->isActive=true;
    }
    if(scope > this->scopeLinks.size()-1){
        for(unsigned int i=this->scopeLinks.size()-1;i<scope;i++){
            this->scopeLinks.push_back(list<SymbolTableEntry*>());
        }
    }
    this->scopeLinks[scope].push_back(entry);
}
SymbolTableEntry* HashTable::lookup(const char *key){
    list<SymbolTableEntry*>::iterator it;
    for(it=this->symbolTable[this->hash(key)].begin();it!=this->symbolTable[this->hash(key)].end();it++){
        if((*it)->isActive){
            if((*it)->type == USERFUNC || (*it)->type == LIBFUNC){
                if(strcmp((*it)->value.funcVal->name,key)==0){
                    return *it;
                }
            }else{
                if(strcmp((*it)->value.varVal->name,key)==0){
                    return *it;
                }
            }
        }
    }
    return NULL;
}
SymbolTableEntry* HashTable::lookup(const char *key,unsigned int scope){
    if(scope>=this->scopeLinks.size()){
        return NULL;
    }
    list<SymbolTableEntry*>::iterator it;
    list<SymbolTableEntry*>::iterator last=this->scopeLinks[scope].end();

    for(it=this->scopeLinks[scope].begin();it!=last;it++){
        if((*it)->isActive){
            if((*it)->type == USERFUNC || (*it)->type == LIBFUNC){
                if(strcmp((*it)->value.funcVal->name,key)==0){
                    return *it;
                }
            }else{
                if(strcmp((*it)->value.varVal->name,key)==0){
                    return *it;
                }
            }
        }
    }
    return NULL;
}

void HashTable::hide(size_t scope){
    list<SymbolTableEntry*>::iterator it;
    if(scope==0){
        file<<"Cannot hide global scope"<<endl;
        return;
    }
    if(scope>=this->scopeLinks.size()){
        return;
    }
    for(it=this->scopeLinks[scope].begin();it!=this->scopeLinks[scope].end();it++){
        (*it)->isActive = false;
    }
}
void HashTable::printScopes(){
    for(long unsigned int i=0;i<this->scopeLinks.size();i++){
        file<<"================== Scope #"<<i<<" =================="<<endl;
        list<SymbolTableEntry*>::iterator it;
        for(it=this->scopeLinks[i].begin();it!=this->scopeLinks[i].end();it++){
            if((*it)->type == USERFUNC ){
                file<<"\""<<(*it)->value.funcVal->name<<"\""<<" [user function]"<<" (line "<<(*it)->value.funcVal->line<<") "<<"(scope "<<(*it)->value.funcVal->scope<<")" << endl;
            }else if((*it)->type == LIBFUNC){
                file<<"\""<<(*it)->value.funcVal->name<<"\""<<" [library function]"<<" (line "<<(*it)->value.funcVal->line<<") "<<"(scope "<<(*it)->value.funcVal->scope<<")" << endl;
            }else{
                if(i==0){
                    file<<"\""<<(*it)->value.varVal->name<<"\""<<" [global variable]"<<" (line "<<(*it)->value.varVal->line<<") "<<"(scope "<<(*it)->value.varVal->scope<<")";
                }else{
                    if((*it)->type == FORMAL){
                        file<<"\""<<(*it)->value.varVal->name<<"\""<<" [formal argument]"<<" (line "<<(*it)->value.varVal->line<<") "<<"(scope "<<(*it)->value.varVal->scope<<")";
                    }else{
                        file<<"\""<<(*it)->value.varVal->name<<"\""<<" [local variable]"<<" (line "<<(*it)->value.varVal->line<<") "<<"(scope "<<(*it)->value.varVal->scope<<")";
                    }
                }
                if((*it)->space==programvar){
                    file<<" [program variable]";
                } else if((*it)->space==formalarg){
                    file<<" [formal argument]";
                } else if((*it)->space==functionlocal){
                    file<<" [functionlocal variable]";
                }
                file<<"[offset - "<<(*it)->offset<<"]"<<endl;
            }
        }
    }
}
void HashTable::print(){
    //print all entries in symbolTable
    for(long unsigned int i=0;i<this->size;i++){
        file<<"================== Bucket #"<<i<<" =================="<<endl;
        list<SymbolTableEntry*>::iterator it;
        for(it=this->symbolTable[i].begin();it!=this->symbolTable[i].end();it++){
                if((*it)->type == USERFUNC ){
                file<<"\""<<(*it)->value.funcVal->name<<"\""<<" [user function]"<<" (line "<<(*it)->value.funcVal->line<<") "<<"(scope "<<(*it)->value.funcVal->scope<<")"<<endl;
            }else if((*it)->type == LIBFUNC){
                file<<"\""<<(*it)->value.funcVal->name<<"\""<<" [library function]"<<" (line "<<(*it)->value.funcVal->line<<") "<<"(scope "<<(*it)->value.funcVal->scope<<")"<<endl;
            }else{
                if((*it)->type == FORMAL){
                    file<<"\""<<(*it)->value.varVal->name<<"\""<<" [formal argument]"<<" (line "<<(*it)->value.varVal->line<<") "<<"(scope "<<(*it)->value.varVal->scope<<")"<<endl;
                }else{
                    file<<"\""<<(*it)->value.varVal->name<<"\""<<" [local variable]"<<" (line "<<(*it)->value.varVal->line<<") "<<"(scope "<<(*it)->value.varVal->scope<<")"<<endl;
                }
            }
        }
    }
}
const char* name(SymbolTableEntry *e){
    if(e->type == USERFUNC){
        return e->value.funcVal->name;
    }else if (e->type == LIBFUNC){
        return e->value.funcVal->name;
    }else{
        return e->value.varVal->name;
    }
}