TARGET=libres
CLANG=cc

FLEX=flex lexer.l
BISON=bison -d -t parser.y
CC=$(CLANG) -o $(TARGET) lex.yy.c parser.tab.c symtab.c error.c

default:
	$(FLEX) 
	$(BISON) 
	$(CC)

cc:
	$(CC)

flex: 
	$(FLEX)

bison:
	$(BISON)


