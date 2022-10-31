HY340

There is available Makefile.
==============================================
To compile:
- make all

To execute:
- ./targetCode <input_file> ***(path to text code file)***
- ./alpha_vm (it takes as input out.bin)

Output files:
- quads.txt - File with generated quads from input file
- symbol_table.txt -  File with symbol table entries sorted by scope
- out.bin - Binary file with generated target code


===============================================
Phase 4 is fully implemented
Virtual machine implementation is not finished. The uploaded version  reads from binary file and loads consts arrays,  user functions array and binary code. Phase 5 is going to be fully implemented.
