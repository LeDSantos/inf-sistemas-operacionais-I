/*
**
** cthread.c
** Biblioteca cthreads
**
** Instituto de Informática - UFRGS
** Sistemas Operacionais I N 2016/2
** Prof. Alexandre Carissimi
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>

#include <cdata.h>
#include <cthread.h>
#include <support.h>

//extern?

int ccreate (void* (*start)(void*), void *arg)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

	// criação da thread
	TCB_t cthread_t;
	cthread_t.tid = threadCount;	threadCount++;
	cthread_t.state = PROCST_CRIACAO;
	cthread.ticket = Random2();

	getcontext(&cthread_t.context);

	cthread_t.context.uc_link = &schduler;
	cthread_t.context.uc_stack.ss_sp = malloc(CT_STACK_SIZE);
	cthread_t.context.uc_stack.ss_size = sizeof(CT_STACK_SIZE);
	cthread_t.context.uc_stack.ss_flags   = 0;

	makecontext(&cthread_t.context, (void (*)(void)) start, 1, &arg);

	//coloca thread na fila de aptos
	AppendFila2(&filaAptos, (void *) &cthread_t)
	cthread_t->state = PROCST_APTO;

	return cthread_t.tid;
}

int cyield(void)
{
		if (!has_init_cthreads)
	{
		init_cthreads();
	}

	TCB_t *cthread_t;
	getcontext(&cthread_t);
	swapcontext(&cthread_t, schduler());
	AppendFila2(&filaAptos, (void *) cthread_t)

	return 0;
}
int cjoin(int tid)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

}
int csem_init(csem_t *sem, int count)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

}
int cwait(csem_t *sem)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

}
int csignal(csem_t *sem)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

}
