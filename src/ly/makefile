ss: lex.yy.o ra.tab.o test.cpp
	g++  lex.yy.o ra.tab.o test.cpp
lex.yy.o: lex.yy.c ra.tab.h
	g++ -c lex.yy.c
lex.yy.c: lex.l 
	flex lex.l
ra.tab.o: ra.tab.h ra.tab.c
	g++ -c ra.tab.c
ra.tab.c ra.tab.h: yacc.y
	bison -dvt -b ra yacc.y