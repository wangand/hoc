run: hoc
	./hoc

hoc: hoc.tab.o lexer.o execerror.o
	gcc hoc.tab.o lexer.o execerror.o -o hoc

lexer.o: lexer.c
	gcc -c lexer.c -o lexer.o

execerror.o: execerror.c
	gcc -c execerror.c -o execerror.o

hoc.tab.o: hoc.tab.c
	gcc -c hoc.tab.c -o hoc.tab.o

hoc.tab.c: hoc.y
	bison hoc.y -d -o hoc.tab.c

symbol.o: symbol.c hoc.tab.h
	gcc -c symbol.c -o symbol.o



test_symbol.o: test_symbol.c hoc.tab.h
	gcc -c test_symbol.c -o test_symbol.o 
test_symbol: test_symbol.o execerror.o symbol.o
	gcc test_symbol.o symbol.o execerror.o -o test_symbol
run_test_symbol: test_symbol
	./test_symbol

