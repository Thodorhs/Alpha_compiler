# Alpha_compiler
*A compiler for *Alpha* programming language (project for CS340 course in University Of Crete)*
## Basic idea:
- Create lexical analizer using *flex/lex*  ✅
- Create syntax analyzer using *yacc/bison* ✅
- Create symbol table after syntax analize is done  ✅
- Create intermidiate low level code (fake instructions) of Alpha ⌛ *(almost done)*
- Construct final machine code (executable) and virtual machine for an execution enviroment
## What is Alpha
## Flexibility of Alpha
## Limitations
## Manual
***Run:***
- use: **make** to compile 

- use: **make clean** to clean 

- use: **./intermed "name".txt** to run 
                   
***Output:***       
- quads.txt - File with generated quads *(fake instructions)* from input file

- symbol_table.txt -  File with symbol table entries sorted by scope

