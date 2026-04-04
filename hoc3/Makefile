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

