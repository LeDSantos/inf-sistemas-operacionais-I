/*
**
** teste1.c
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

void hello();
void space();
void world();
void excl();

int thello, tspace, tworld, texcl, cy;

void hello(){
	printf("hello");
}

void space(){
	printf(" ");
}

void world(){
	printf("world\n");
}

void excl(){
	printf("!\n");
}

int main()
{
	printf("criando uma thread para a funcao hello\n");
	thello = ccreate((void *)hello, NULL);
	printf("sou a thread hello com tid: %d\n", thello);

	printf("criando uma thread para a funcao space\n");
	tspace = ccreate((void *)space, (void *)NULL);
	printf("sou a thread space com tid: %d\n", tspace);

	printf("criando uma thread para a funcao world\n");
	tworld = ccreate((void *)world, (void *)NULL);
	printf("sou a thread world com tid: %d\n", tworld);

	printf("criando uma thread para a funcao excl\n");
	texcl = ccreate((void *)excl, (void *)NULL);
	printf("sou a thread excl com tid: %d\n", texcl);

	printf("yield 1 na main\n");
	cy = cyield();
	printf("cyield: %d\n", cy);

	printf("yield 2 na main\n");
	cy = cyield();
	printf("cyield: %d\n", cy);

	printf("yield 3 na main\n");
	cy = cyield();
	printf("cyield: %d\n", cy);

	printf("yield 4 na main\n");
	cy = cyield();
	printf("cyield: %d\n", cy);

	return 0;
}
