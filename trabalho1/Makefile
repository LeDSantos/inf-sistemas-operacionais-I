#
# cthread.c
# Biblioteca cthreads
#
# Instituto de Informática - UFRGS
# Sistemas Operacionais I N 2016/2
# Prof. Alexandre Carissimi
#
# Gustavo Madeira Santana
# Cristiano Salla Lunardi
#

LIB=./lib/
INC=./include/
BIN=./bin/
SRC=./src/
TST=./testes/

DIRS=$(LIB) $(INC) $(BIN) $(SRC) $(TST)

CC=gcc
CCFLAGS=-c -lm -m32 -Wall -I$(INC)

all: t1 t2 t3

t1: cdata.o cthread.o teste1.o
	$(CC) teste1.o cdata.o cthread.o $(BIN)support.o -o teste1

t2: cdata.o cthread.o teste2.o
	$(CC) teste2.o cdata.o cthread.o $(BIN)support.o -o teste2

t3: cdata.o cthread.o teste3.o
	$(CC) teste3.o cdata.o cthread.o $(BIN)support.o -o teste3

teste1.o:
	$(CC) $(CCFLAGS) $(TST)teste1.c

teste2.o:
	$(CC) $(CCFLAGS) $(TST)teste2.c

teste3.o:
	$(CC) $(CCFLAGS) $(TST)teste3.c

cdata.o:
	$(CC) $(CCFLAGS) $(SRC)cdata.c

cthread.o:
	$(CC) $(CCFLAGS) $(SRC)cthread.c

clean:
	find . -type f ! -name 'support.o' ! -name "*.c" ! -name "*.h" ! -name "Makefile" ! -name "*.pdf" -delete