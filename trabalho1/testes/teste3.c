/*
**
** teste3.c
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

	csem_t sem;

void hello(){
	printf("hello executando cwait\n");
	cwait(&sem);
	printf("sou a thread hello com tid: %d\n", thello);
	csignal(&sem);
}

void space(){
	printf("space executando cwait\n");
	cwait(&sem);
	printf("sou a thread space com tid: %d\n", tspace);
	csignal(&sem);
}

void world(){
	printf("sou a thread world com tid: %d\n", tworld);
}

void excl(){
	printf("sou a thread excl com tid: %d\n", texcl);
}

int main()
{
	printf("#main: criando uma thread para a funcao hello\n");
	thello = ccreate((void *)hello, NULL);

	printf("#main: criando uma thread para a funcao space\n");
	tspace = ccreate((void *)space, (void *)NULL);

	printf("#main: criando uma thread para a funcao world\n");
	tworld = ccreate((void *)world, (void *)NULL);

	printf("#main: criando uma thread para a funcao excl\n");
	texcl = ccreate((void *)excl, (void *)NULL);

	printf("#main: criando semaforo\n");
	csem_init(&sem, 1);

	printf("#main: executando cwait\n");
	cwait(&sem);

	printf("#main: 5 yields\n");
	cyield();
	cyield();
	cyield();
	cyield();
	cyield();

	printf("#main: csignal\n");
	csignal(&sem);

	printf("#main: 5 yields\n");
	cyield();
	cyield();
	cyield();
	cyield();
	cyield();

	printf("#main: cjoin na thread hello\n");
	cjoin(thello);

	printf("#main: terminando\n");

	return 0;
}
