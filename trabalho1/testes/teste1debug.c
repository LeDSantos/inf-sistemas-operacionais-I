/*
**
** teste1debug.c
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
	cjoin(tworld);
	printf("hello\n");
	printf("sou a thread hello com tid: %d\n", thello);
}

void space(){
	cjoin(tworld);
	printf("_\n");
	printf("sou a thread space com tid: %d\n", tspace);
}

void world(){
	cjoin(thello);
	printf("world\n");
	cyield();
	printf("sou a thread world com tid: %d\n", tworld);
}

void excl(){
	cjoin(texcl);
	printf("!\n");
	printf("sou a thread excl com tid: %d\n", texcl);
}

int main()
{

	printf("debug ligado\n");
	debugOn();

	printf("criando uma thread para a funcao hello\n");
	thello = ccreate((void *)hello, NULL);

	printf("criando uma thread para a funcao space\n");
	tspace = ccreate((void *)space, (void *)NULL);

	printf("criando uma thread para a funcao world\n");
	tworld = ccreate((void *)world, (void *)NULL);

	printf("criando uma thread para a funcao excl\n");
	texcl = ccreate((void *)excl, (void *)NULL);

	printf("criando 100 threads para a funcao hello\n");
	int i = 0;
	for (i = 0; i < 100; ++i)
	{
		ccreate((void *)hello, NULL);
	}


	printf("dando 50 yields na main\n");

	for (int i = 0; i < 50; ++i)
	{
		cyield();
	}

	printf("main terminando\n");

	return 0;
}
