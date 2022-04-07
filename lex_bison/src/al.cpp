#include "al.hpp"

void* data;
size_t line;
size_t no=1;
int do_nothing=0;
int comment=0;
int linetmp=0;
int wrong_comment=0;
int wrong_string=0;
std::queue<alpha_token_t> tokens;

char* replace_escapes (const char* nstr) {
    int i = 0, j, flag, length;
    char *str=(char*)nstr;
    length = strlen(str);
    while (str[i] != '\0') {
        flag = 0;
        if (str[i] == '\\') {
            if (str[i+1] == 'n') {
                str[i] = '\n';
                flag = 1;
            } else if (str[i+1] == 't') {
                str[i] = '\t';
                flag = 1;
            } else if (str[i+1] == '\\') {
                str[i] = '\\';
                flag = 1;
            } else if (str[i+1] == '\"') {
                str[i] = (char)34;
                flag = 1;
            }
            if (flag == 1) {
                for (j = i+1; j < length; j++) 
                    str[j] = str[j+1];
                str[j] = '\0';
                length--;
            }
        }
        i++;
    }

    return str;
}
static std::string enum_to_string(alpha_token_t token){
    switch(token.type){
        case KEYWORD1:
            return "KEYWORD";
        case OPERATOR1:
            return "OPERATOR";
        case INTCONST1:
            return "INTCONST";
        case REALCONST1:
            return "REALCONST";
        case STRING1:
            return "STRING";
        case CURLY_BRACKET1:
            return "PUNCTUATION CURLY_BRACKET";
        case SQUARE_BRACKET1:
            return "PUNCTUATION SQUARE_BRACKET";
        case PARENTHESIS1:
            return "PUNCTUATION PARENTHESIS";
        case SEMICOLON1:
            return "PUNCTUATION SEMICOLON";
        case COLON1:
            return "PUNCTUATION COLON";
        case COMMA1:
            return "PUNCTUATION COMMA";
        case DOUBLE_COLON1:
            return "PUNCTUATION DOUBLE_COLON";
        case DOT1:
            return "PUNCTUATION DOT";
        case DOUBLE_DOT1:
            return "PUNCTUATION DOUBLE_DOT";
        case IDENT1:
            return "IDENT";
        case COMMENT1:
            return "COMMENT";
        case NESTED_COMMENT1:
            return "NESTED_COMMENT";
        case NOT_CLOSED_STRING1:
            return "NOT_CLOSED_STRING IN LINE";
        case NOT_CLOSED_COMMENT1:
            return "\nNOT_CLOSED_COMMENT IN LINE";
        case UNKNOWN1:
            return "UNKNOWN";
        default:
            return "UNKNOWN";
    }
}

void print_token(alpha_token_t token,FILE* yyout){
    if(token.type==STRING1){
        fprintf(yyout,"%ld: #%ld %s %s", token.line, token.no, token.text.c_str(), enum_to_string(token).c_str());
    }else if(token.type==NOT_CLOSED_COMMENT1){
        fprintf(yyout,"not closed comment %s",enum_to_string(token).c_str());
    }else if(token.type==NOT_CLOSED_STRING1){
        fprintf(yyout,"not closed string %s",enum_to_string(token).c_str());
    }else{
        fprintf(yyout,"%ld: #%ld \"%s\" %s", token.line, token.no, token.text.c_str(), enum_to_string(token).c_str());
    }
    switch(token.type){
        case KEYWORD1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case OPERATOR1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case INTCONST1:
            fprintf(yyout," %d <- int",*(int*)token.value);
            break;
        case REALCONST1:
            fprintf(yyout," %f <- real",*(double*)token.value);
            break;
        case STRING1:
            fprintf(yyout," %s <- char*",replace_escapes(((std::string*)token.value)->c_str()));
            break;
        case CURLY_BRACKET1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case SQUARE_BRACKET1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case PARENTHESIS1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case SEMICOLON1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case COLON1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case COMMA1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case DOUBLE_COLON1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case DOT1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case DOUBLE_DOT1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case IDENT1:
            fprintf(yyout," \"%s\" <- char*",((std::string*)(token.value))->c_str());
            break;
        case NESTED_COMMENT1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case COMMENT1:
            fprintf(yyout," %s <- enumerated",((std::string*)(token.value))->c_str());
            break;
        case UNKNOWN1:
            fprintf(yyout," %s",((std::string*)(token.value))->c_str());
            break;
        default:
            fprintf(yyout," %s",((std::string*)(token.value))->c_str());
            break;
    }
    fprintf(yyout,"\n");
}

int alpha_yylex(void* ylval,int res){
    alpha_token_t token;
    if (res > 0){
        if(do_nothing==0){
            if(comment){
                token.line = linetmp;
                comment=0;
            }else{
                token.line = yylineno;
            }
            token.no = no;
            if(res==COMMENT1||res==NESTED_COMMENT1){
                token.line = linetmp;
                token.text=yytext;
            }else{
                if(instring){
                    token.text = tempstring;
                }else{
                    token.text = yytext;
                }
                
            }
            token.type = (TYPE)res;
            token.value =data;
            no++;
           // print_token(token,yyout);
            tokens.push(token);
        }
        if(token.type==NOT_CLOSED_STRING1||token.type==NOT_CLOSED_COMMENT1){
           // print_token(token,yyout);
            return -1;
        }
        
    }
    if(res==-1){
        token.line=yylineno;
        token.no=0;
        token.text="ERROR:";
        token.type=UNKNOWN1;
        token.value=new std::string("TOKEN");
        tokens.push(token);
        return 1;
        //print_token(token,yyout);
    }
    return res;
}

void print_tokens(){
    alpha_token_t token;
    while(!tokens.empty()){
        token=tokens.front();
        tokens.pop();
        //print_token(token,yyout);
    }
}