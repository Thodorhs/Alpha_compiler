# Alpha_compiler
*A compiler for *Alpha* programming language (project for CS340 course in University Of Crete)*
## Developers

- **Thodoris Pontzouktzidis** https://github.com/Thodorhs

- **Alexandros Markodimitrakis** https://github.com/AlexM010

- **Xaris Apostolou** https://github.com/Harisap17

## Basic idea
- Create lexical analizer using *flex/lex*  ✅
- Create syntax analyzer using *yacc/bison* ✅
- Create symbol table after syntax analize is done  ✅
- Create intermidiate low level code (fake instructions) of Alpha ✅
- Construct final machine code (executable) and virtual machine for an execution enviroment ⌛
## What is Alpha
## Flexibility of Alpha
## Limitations
## Prerequisites
- Make || ***apt install make***

- g++ || ***apt install g++***

- Flex || ***apt install flex***

- Bison || ***apt install bison***

## Manual
***Run:***
- use: **run.sh youfile.txt** to compile-run.

- use: **run.sh -c** to clean unnecessary files.

- use: **run.sh -h** for help.

***Generated files:***
- ***out*** directory is created. IF not present run.sh proceeds to make clean and then make all *(compile)*.

- ***out/quads.txt*** - File with generated quads *(pseudo-assembly)* from input file.

- ***out/instructions.txt*** -  File with vm instructions.

- The rest are not to be taken under consideration.

