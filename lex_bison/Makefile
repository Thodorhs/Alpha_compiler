CC = g++
CFLAGS = -Wall -pedantic -g 
parser: ./src/hashtable.o ./src/parser.o ./src/al.o  ./src/la.o
	@$(CC) $(CFLAGS)  $^ -o $@
./src/%.o: ./src/%.cpp 
	@$(CC) $(CFLAGS) -o $@ -c $<
./src/la.cpp: ./src/al.l ./src/al.hpp
	@flex ./src/al.l
./src/parser.cpp: ./src/al.hpp
	@bison --yacc --defines --output=./src/parser.cpp ./src/parser.y
clean:
	@rm -f parser ./src/*.o
	@rm -f parser ./src/*parser.output
	@rm -f ./src/la.cpp ./src/parser.cpp ./src/parser.hpp
	@echo "Cleaned!"