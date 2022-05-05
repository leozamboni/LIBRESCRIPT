TARGET=libres
CC=cc

cc:
	$(CC) -o $(TARGET) lex.yy.c parser.tab.c symtab.c error.c

flex: 
	flex lexer.l

bison:
	bison -d -t parser.y


