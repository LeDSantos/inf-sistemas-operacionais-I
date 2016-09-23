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
	cjoin(world);
	printf("hello");
	printf("sou a thread hello com tid: %d\n", thello);
}

void space(){
	printf(" ");
	printf("sou a thread space com tid: %d\n", tspace);
}

void world(){
	cyield();
	printf("world\n");
	printf("sou a thread world com tid: %d\n", tworld);
}

void excl(){
	printf("!\n");
	printf("sou a thread excl com tid: %d\n", texcl);
}

int main()
{
	printf("criando uma thread para a funcao hello\n");
	thello = ccreate((void *)hello, NULL);

	printf("criando uma thread para a funcao space\n");
	tspace = ccreate((void *)space, (void *)NULL);

	printf("criando uma thread para a funcao world\n");
	tworld = ccreate((void *)world, (void *)NULL);

	printf("criando uma thread para a funcao excl\n");
	texcl = ccreate((void *)excl, (void *)NULL);

	printf("join da main com hello\n");
	cjoin(thello);

	printf("yield na main\n");
	cyield();

	printf("main terminando\n");

	return 0;
}
