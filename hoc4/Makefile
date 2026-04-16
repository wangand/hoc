run: hoc
	./hoc

hoc: hoc.tab.o lexer.o execerror.o symbol.o init.o math.o
	gcc hoc.tab.o lexer.o execerror.o symbol.o init.o math.o -o hoc

lexer.o: lexer.c hoc.tab.o
	gcc -c lexer.c -o lexer.o

execerror.o: execerror.c
	gcc -c execerror.c -o execerror.o

hoc.tab.o: hoc.tab.c
	gcc -c hoc.tab.c -o hoc.tab.o

hoc.tab.c: hoc.y
	bison hoc.y -d -o hoc.tab.c

symbol.o: symbol.c hoc.tab.h
	gcc -c symbol.c -o symbol.o
math.o: math.c
	gcc -c math.c -o math.o
init.o: init.c
	gcc -c init.c -o init.o
code.o: code.c
	gcc -c code.c -o code.o


test_symbol.o: test_symbol.c hoc.tab.h
	gcc -c test_symbol.c -o test_symbol.o 
test_symbol: test_symbol.o execerror.o symbol.o
	gcc test_symbol.o symbol.o execerror.o -o test_symbol
run_test_symbol: test_symbol
	./test_symbol

test_init.o: test_init.c
	gcc -c test_init.c -o test_init.o
test_init: test_init.o init.o math.o execerror.o symbol.o
	gcc test_init.o init.o math.o execerror.o symbol.o -o test_init
run_test_init: test_init
	./test_init

test_lexer.o: test_lexer.c
	gcc -c test_lexer.c -o test_lexer.o
test_lexer: test_lexer.o lexer.o symbol.o execerror.o
	gcc test_lexer.o lexer.o symbol.o execerror.o -o test_lexer
run_test_lexer: test_lexer
	./test_lexer

test_code.o: test_code.c
	gcc -c test_code.c -o test_code.o
test_code: test_code.o init.o math.o execerror.o symbol.o code.o
	gcc test_code.o init.o math.o execerror.o symbol.o code.o -o test_code
run_test_code: test_code
	./test_code

