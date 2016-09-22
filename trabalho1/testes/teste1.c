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

int thello, tspace, tworld, texcl;

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
	printf("criar thread para função hello\n");
	thello = ccreate((void *)hello, NULL);
	printf("sou a thread %d", thello);

	tspace = ccreate((void *)space, (void *)NULL);
	printf("sou a thread %d", tspace);

	tworld = ccreate((void *)world, (void *)NULL);
	printf("sou a thread %d", tworld);

	texcl = ccreate((void *)excl, (void *)NULL);
	printf("sou a thread %d", texcl);

	cyield();

	return 0;
}
