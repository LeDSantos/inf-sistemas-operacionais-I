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

LIB=./lib/libt2fs.a
INC=./include/
BIN=./bin/
SRC=./src/
TST=./teste/

DIRS=$(LIB) $(INC) $(BIN) $(SRC) $(TST) $(EXP)

CC=gcc
CCFLAGS=-c -lm -m32 -Wall -I$(INC)

all: lib

lib: t2fs.o
	ar -cru $(LIB) t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o

# shell: shell.o
# 	$(CC) shell.o t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o -o shell

# t1: t2fs.o teste1.o
# 	$(CC) teste1.o t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o -o teste1

# t2: t2fs.o teste2.o
# 	$(CC) teste2.o t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o -o teste2

# t3: t2fs.o teste3.o
# 	$(CC) teste3.o t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o -o teste3

# t4: t2fs.o teste4.o
# 	$(CC) teste4.o t2fs.o $(BIN)apidisk.o $(BIN)bitmap2.o -o teste4

# shell.o:
# 	$(CC) $(CCFLAGS) $(TST)shell.c

# teste1.o:
# 	$(CC) $(CCFLAGS) $(TST)teste1.c

# teste2.o:
# 	$(CC) $(CCFLAGS) $(TST)teste2.c

# teste3.o:
# 	$(CC) $(CCFLAGS) $(TST)teste3.c

# teste4.o:
# 	$(CC) $(CCFLAGS) $(TST)teste4.c

t2fs.o:
	$(CC) $(CCFLAGS) $(SRC)t2fs.c

clean:
	find . -type f ! -name '*.dat' ! -name 'apidisk.o' ! -name 'bitmap2.o' ! -name "*.c" ! -name "*.h" ! -name "Makefile" ! -name "*.pdf" ! -name "*.tex" ! -name "*.png" -delete
