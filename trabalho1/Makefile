
#
# Makefile ESQUELETO
#
# OBRIGATÓRIO ter uma regra "all" para geração da biblioteca e de uma
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#  1. Cuidado com a regra "clean" para não apagar o "fila2.o"
#
# OBSERVAR que as variáveis de ambiente consideram que o Makefile está no diretótio "cthread"
#

LIB=./lib/
INC=./include/
BIN=./bin/
SRC=./src/
TST=./testes/

DIRS=$(LIB) $(INC) $(BIN) $(SRC) $(TST)

CC=gcc
CCFLAGS=-c -m32 -Wall -I$(INC)

all: teste

teste: cdata.o cthread.o teste1.o
	$(CC) teste1.o cdata.o cthread.o $(BIN)support.o -o teste1

teste1.o:
	$(CC) $(CCFLAGS) $(TST)teste1.c

cdata.o:
	$(CC) $(CCFLAGS) $(SRC)cdata.c

cthread.o:
	$(CC) $(CCFLAGS) $(SRC)cthread.c

clean:
	find . -type f ! -name 'support.o' ! -name "*.c" ! -name "*.h" ! -name "Makefile" ! -name "*.pdf" -delete