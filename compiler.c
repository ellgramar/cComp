#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#define int long long // work with 64bit target
int token;            // current token
char *src, *old_src;  // pointer to source code string;
int poolsize;         // default size of text/data/stack
int line;             // line number
int *text,            // text segment
    *old_text,        // for dump text segment
    *stack;           // stack
char *data;           // data segment
int *pc, *bp, *sp, ax, cycle; // virtual machine registers
// instructions
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };
// tokens and classes (operators last and in precedence order)
enum {
  Num = 128, Fun, Sys, Glo, Loc, Id,
  Char, Else, Enum, If, Int, Return, Sizeof, While,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};
/*
Symbol table:
----+-----+----+----+----+-----+-----+-----+------+------+----
 .. |token|hash|name|type|class|value|btype|bclass|bvalue| ..
----+-----+----+----+----+-----+-----+-----+------+------+----
    |<---       one single identifier                --->|
*/
int token_val;              //val of current token
int *current_id, *symbols;  //cureent parsed ID, symbol table
enum {Token, Hash, Name, Type, Class, Value, BType, Bclass, Bvalue, IdSize};
//lexer
void next() {
    char *last_pos;
    int hash;
    while (token = *src){
        ++src;
        //parse token here
        if (token == '\n'){
            ++line;
        }
        else if (token == '#'){
            //skip macro, not supported
            while (*src != 0 && *src != 'n'){
                src++;
            }
        }
        else if ((token >= 'a' && token <= 'z') || (token >= 'A' && token <= 'Z') || (token == '_')){
            last_pos = src - 1;
            hash = token;
            while ((*src >= 'a' && *src <= 'z') || (*src <= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9') || (*src == '_')){
                hash = hash * 147 + *src;
                src++;
            }
            //linear serach for exiesting identifier
            current_id = symbols;
            while (current_id[Token]){
                if (current_id[Hash] == hash && !memcmp((char*)current_id[Name], last_pos, src-last_pos)){
                    //found one, return
                    token = current_id[Token];
                    return;
                }
                current_id = current_id + IdSize;
            }
            current_id[Name] = (int)last_pos;
            current_id[Hash] = hash;
            token = current_id[Token] = Id;
            return;
        }

    }
    return;
}
void expression(int level) {
    // do nothing
}
void program() {
    next(); // get next token
    while (token > 0) {
        printf("token is: %c\n", token);
        next();
    }
}
int eval() {
    int op, *tmp;
    while (1) {
        op = *pc++; // get next operation code
        switch (op){
            case IMM:
            {ax = *pc++;}                                     
            break;
            case LC:
            {ax = *(char *)ax;}                               
            break;
            case LI:
            {ax = *(int *)ax;}                                
            break;
            case SC:
            {ax = *(char *)*sp++ = ax;}                       
            break;
            case SI:
            {*(int *)*sp++ = ax;}                             
            break;
            case PUSH:
            {*--sp = ax;}                                     
            break;
            case JMP:
            {pc = (int *)*pc;}                                
            break;
            case JZ:
            {pc = ax ? pc + 1 : (int *)*pc;}                  
            break;
            case JNZ:
            {pc = ax ? (int *)*pc : pc + 1;}                  
            break;
            case CALL:
            {*--sp = (int)(pc+1); pc = (int *)*pc;}           
            break;
            //case RET:
            //{pc = (int *)*sp++;}                              
            //break;
            case ENT:
            {*--sp = (int)bp; bp = sp; sp = sp - *pc++;}      
            break;
            case ADJ:
            {sp = sp + *pc++;}                                
            break;
            case LEV:
            {sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}   
            break;
            case LEA:
            {ax = (int)(bp + *pc++);}                         
            break;
            case OR:
            ax = *sp++ | ax;
            break;
            case XOR:
            ax = *sp++ ^ ax;
            break;
            case AND:
            ax = *sp++ & ax;
            break;
            case EQ:
            ax = *sp++ == ax;
            break;
            case NE:
            ax = *sp++ != ax;
            break;
            case LT:
            ax = *sp++ < ax;
            break;
            case LE:
            ax = *sp++ <= ax;
            break;
            case GT:
            ax = *sp++ >  ax;
            break;
            case GE:
            ax = *sp++ >= ax;
            break;
            case SHL:
            ax = *sp++ << ax;
            break;
            case SHR:
            ax = *sp++ >> ax;
            break;
            case ADD:
            ax = *sp++ + ax;
            break;
            case SUB:
            ax = *sp++ - ax;
            break;
            case MUL:
            ax = *sp++ * ax;
            break;
            case DIV:
            ax = *sp++ / ax;
            break;
            case MOD:
            ax = *sp++ % ax;
            break;
            case EXIT:
            { printf("exit(%d)", *sp); return *sp;}
            break;
            case OPEN:
            { ax = open((char *)sp[1], sp[0]); }
            break;
            case CLOS:
            { ax = close(*sp);}
            break;
            case READ:
            { ax = read(sp[2], (char *)sp[1], *sp); }
            break;
            case PRTF:
            { tmp = sp + pc[1]; ax = printf((char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]); }
            break;
            case MALC:
            { ax = (int)malloc(*sp);}
            break;
            case MSET:
            { ax = (int)memset((char *)sp[2], sp[1], *sp);}
            break;
            case MCMP:
            { ax = memcmp((char *)sp[2], (char *)sp[1], *sp);}break;
            default:{printf("unknown instruction:%d\n", op);return -1;}
        }
    }
    return 0;
}
#undef int // Mac/clang needs this
int main(int argc, char **argv){
    #define int long long // work with 64bit target
    int i, fd;
    argc--;
    argv++;
    poolsize = 256 * 1024; // arbitrary size
    line = 1;
    if ((fd = open(*argv, 0)) < 0) {
        printf("could not open(%s)\n", *argv);
        return -1;
    }
    if (!(src = old_src = malloc(poolsize))) {
        printf("could not malloc(%d) for source area\n", poolsize);
        return -1;
    }
    // read the source file
    if ((i = read(fd, src, poolsize-1)) <= 0) {
        printf("read() returned %d\n", i);
        return -1;
    }
    src[i] = 0; // add EOF character
    close(fd);
    // allocate memory for virtual machine
    if (!(text = old_text = malloc(poolsize))) {
        printf("could not malloc(%d) for text area\n", poolsize);
        return -1;
    }
    if (!(data = malloc(poolsize))) {
        printf("could not malloc(%d) for data area\n", poolsize);
        return -1;
    }
    if (!(stack = malloc(poolsize))) {
        printf("could not malloc(%d) for stack area\n", poolsize);
        return -1;
    }
    memset(text, 0, poolsize);
    memset(data, 0, poolsize);
    memset(stack, 0, poolsize);
    bp = sp = (int *)((int)stack + poolsize);

    program();
    return eval();
}
