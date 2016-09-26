/*
**
** teste2.c
** Teste da Biblioteca cthreads
**
** Instituto de Informática - UFRGS
** Sistemas Operacionais I N 2016/2
** Prof. Alexandre Carissimi
**
** Gustavo Madeira Santana
** Cristiano Salla Lunardi
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

int	id1, id2;

void func1(void *arg) {
	printf("ID1 join ID2\n");
	cjoin(id2);
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

void func2(void *arg) {
	printf("Eu sou a thread ID2 imprimindo %d\n", *((int *)arg));
}

int main(int argc, char *argv[]) {

	printf("debug ligado\n");
	debugOn();

	int i;

	id1 = ccreate((void *)func1, (void *)&i);
	id2 = ccreate((void *)func2, (void *)&i);

	printf("Eu sou a main apos a criacao de ID1 e ID2\n\n");

	printf("Main:\nID1 tid: %d\nID2 tid: %d\n\n", id1, id2);

	printf("main dando join em ID1\n");
	cjoin(id1);

	printf("Eu sou a main voltando para terminar o programa\n");

	return 0;
}