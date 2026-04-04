run: hoc
	./hoc

hoc: hoc.tab.o lexer.o
	gcc hoc.tab.o lexer.o -o hoc -ll

lexer.o: lexer.c
	gcc -c lexer.c -o lexer.o

hoc.tab.o: hoc.tab.c
	gcc -c hoc.tab.c -o hoc.tab.o

hoc.tab.c: hoc.y
	bison hoc.y -d -o hoc.tab.c

