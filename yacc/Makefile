#
# Makefile for TINY
# Lex & Yacc Version
# by Kahaani, 2013.06
#

vpath %.h .
vpath %.h ..

CC = gcc
LEX = flex
YACC = bison

CFLAGS = -I. -I..

OBJS = main.o util.o scan.o parse.o symtab.o analyze.o code.o cgen.o

tiny: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o tiny -lfl

main.o: ../main.c globals.h util.h scan.h parse.h analyze.h cgen.h y.tab.h
	$(CC) $(CFLAGS) -c ../main.c

util.o: ../util.c util.h globals.h y.tab.h
	$(CC) $(CFLAGS) -c ../util.c

# modified
scan.o: ../lex/tiny.l scan.h util.h globals.h y.tab.h
	$(LEX) ../lex/tiny.l
	$(CC) $(CFLAGS) -c lex.yy.c -o scan.o

# modified
parse.o y.tab.h: tiny.y parse.h scan.h globals.h util.h y.tab.h
	$(YACC) -d tiny.y
	$(CC) $(CFLAGS) -c y.tab.c -o parse.o

symtab.o: ../symtab.c symtab.h y.tab.h
	$(CC) $(CFLAGS) -c ../symtab.c

analyze.o: ../analyze.c globals.h symtab.h analyze.h y.tab.h
	$(CC) $(CFLAGS) -c ../analyze.c

code.o: ../code.c code.h globals.h y.tab.h
	$(CC) $(CFLAGS) -c ../code.c

cgen.o: ../cgen.c globals.h symtab.h code.h cgen.h y.tab.h
	$(CC) $(CFLAGS) -c ../cgen.c

clean:
	-rm $(OBJS)
	-rm lex.yy.c
	-rm y.tab.c
	-rm y.tab.h

tm: ../tm.c
	$(CC) $(CFLAGS) ../tm.c -o tm

all: tiny tm

