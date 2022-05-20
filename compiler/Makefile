CC = g++
CFLAGS = -Wall -pedantic -g 
intermed: ./yacc/hashtable.o ./yacc/parser.o ./yacc/al.o  ./yacc/la.o ./intermediate/intermediate.o
	@$(CC) $(CFLAGS)  $^ -o $@
./intermediate/%.o:  ./intermediate/intermediate.cpp
	@$(CC) $(CFLAGS) -o $@ -c $<
./yacc/%.o: ./yacc/%.cpp 
	@$(CC) $(CFLAGS) -o $@ -c $<
./yacc/la.cpp: ./yacc/al.l ./yacc/al.hpp
	@flex ./yacc/al.l
./yacc/parser.cpp: ./yacc/al.hpp
	@bison --yacc --defines -v --output=./yacc/parser.cpp ./yacc/parser.y
clean:
	@rm -f intermed ./yacc/*.o
	@rm -f ./yacc/*parser.output quads.txt
	@rm -f ./intermediate/*.o symbol_table.txt
	@rm -f ./yacc/la.cpp ./yacc/parser.cpp ./yacc/parser.hpp
	@echo "Cleaned!"