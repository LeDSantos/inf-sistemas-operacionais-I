#
# Makefile
# T2FS
#
# Instituto de Informática - UFRGS
# Sistemas Operacionais I N 2016/2
# Prof. Alexandre Carissimi
#
# Gustavo Madeira Santana
# Cristiano Salla Lunardi
#

LIB=./lib/libcthread.a
INC=./include/
BIN=./bin/
SRC=./src/
TST=./teste/

DIRS=$(LIB) $(INC) $(BIN) $(SRC) $(TST) $(EXP)

CC=gcc
CCFLAGS=-c -lm -m32 -Wall -I$(INC)

all: t1

t1: t2fs.o teste1.o
	$(CC) teste1.o t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o -o teste1

teste1.o:
	$(CC) $(CCFLAGS) $(TST)teste1.c

t2fs.o:
	$(CC) $(CCFLAGS) $(SRC)t2fs.c

clean:
	find . -type f ! -name '*.dat' ! -name 'apidisk.o' ! -name 'bitmap2.o' ! -name "*.c" ! -name "*.h" ! -name "Makefile" ! -name "*.pdf" ! -name "*.tex" ! -name "*.png" -delete
