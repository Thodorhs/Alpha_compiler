CC = g++
CFLAGS = -Wall -pedantic -g 
all: alpha_vm targetCode
alpha_vm: ./avm/avm.cpp
	@$(CC) $(CFLAGS) -o ../out/$@ $^
targetCode: ./yacc/hashtable.o ./yacc/parser.o ./yacc/al.o  ./yacc/la.o ./intermediate/intermediate.o ./target/target.o 
	@$(CC) $(CFLAGS)  $^ -o ../out/$@
./intermediate/%.o:  ./intermediate/intermediate.cpp
	@$(CC) $(CFLAGS) -o $@ -c $<
./target/%.o:  ./target/target.cpp
	@$(CC) $(CFLAGS) -o $@ -c $<
./yacc/%.o: ./yacc/%.cpp 
	@$(CC) $(CFLAGS) -o $@ -c $<
./yacc/la.cpp: ./yacc/al.l ./yacc/al.hpp
	@flex ./yacc/al.l
./yacc/parser.cpp: ./yacc/al.hpp
	@bison --yacc --defines -v --output=./yacc/parser.cpp ./yacc/parser.y
out:
	mkdir ../out
clean:
	@rm -f ../out/alpha_vm
	@rm -f ../out/targetCode ./yacc/*.o ../out/out.bin
	@rm -f ./yacc/*parser.output ../out/quads.txt
	@rm -f ./intermediate/*.o symbol_table.txt
	@rm -f ./target/*.o ../out/instructions.txt
	@rm -f ./yacc/la.cpp ./yacc/parser.cpp ./yacc/parser.hpp
	@rm -rf ../out
	@echo "Cleaned!"