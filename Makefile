hoc1run:
	./steps/hoc1/hoc1

hoc1: hoc1.tab.o lexer.o
	gcc steps/hoc1/hoc1.tab.o lexer.o -o steps/hoc1/hoc1 -ll

lexer.o: lexer.c
	gcc -c lexer.c -o lexer.o

hoc1.tab.o: hoc1.tab.c
	gcc -c steps/hoc1/hoc1.tab.c -o steps/hoc1/hoc1.tab.o

hoc1.tab.c: steps/hoc1/hoc1.y
	bison steps/hoc1/hoc1.y -d -o steps/hoc1/hoc1.tab.c


clean:
	rm -rf *.o

