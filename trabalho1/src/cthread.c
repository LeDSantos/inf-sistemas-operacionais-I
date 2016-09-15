/*
**
** cthread.c
** Biblioteca cthreads
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
#include <stdbool.h>
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

extern bool has_init_cthreads;
extern ucontext_t scheduler;
extern TCB_t running_thread;

/*
** cria uma thread e a coloca na fila de aptos
*/
int ccreate (void* (*start)(void*), void *arg)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

	TCB_t *c_thread = malloc(sizeof(TCB_t));
	c_thread->tid = thread_count; thread_count++;
	c_thread->state = PROCST_CRIACAO;
	c_thread->ticket = Random2();

	getcontext(&c_thread->context);

	c_thread->context.uc_link = &scheduler;
	c_thread->context.uc_stack.ss_sp = malloc(CT_STACK_SIZE);
	c_thread->context.uc_stack.ss_size = sizeof(CT_STACK_SIZE);
	c_thread->context.uc_stack.ss_flags = 0;

	makecontext(&c_thread->context, (void (*)(void)) start, 1, &arg);

	c_thread->state = PROCST_APTO;

	if(!AppendFila2(&filaAptos, (void *) &c_thread))
		return c_thread->tid;
	else
		return -1;

}

/*
** coloca a thread atual na fila de aptos e passa o controle para o scheduler
*/
int cyield(void)
{
		if (!has_init_cthreads)
	{
		init_cthreads();
	}

	thread_running->state = PROCST_APTO;

	if(thread_running->state == PROCST_APTO)
			swapcontext(&thread_running->context, &dispatcher)
			return 0;
		else
			return -1;
}

/*
**
*/
int cjoin(int tid)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

	FirstFila2(&filaAptos);
	PNODE2 aux_it = filaAptos.it;

	TCB_t *thread_to_wait = GetAtIteratorFila2(&filaAptos);

	while(NextFila2(&filaAptos) == 0)
	{

		thread_to_wait = GetAtIteratorFila2(&filaAptos);

		if(filaAptos.it == NULL)
		{
			printf("thread não existe ou já terminou ou não está na fila de aptos\n");
			return -1;
		}
		else
		{
			if (thread_to_wait->tid == tid)
			{
				running_thread->state = PROCST_BLOQ;

				if (running_thread->state == PROCST_BLOQ)
				{	//ver um jeito melhor de fazer, que passe pelo scheduler
					thread_to_wait->context.uc_link = &(running_thread->context.uc_link);
					swapcontext(&thread_running->context, &scheduler)
					return 0;
				}
			}
		}

	printf("não encontrou uma condição de parada no cjoin\n");
	return -1;
}

/*
**
*/
int csem_init(csem_t *sem, int count)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

	return 0;
}

/*
**
*/
int cwait(csem_t *sem)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

	return 0;
}

/*
**
*/
int csignal(csem_t *sem)
{
	if (!has_init_cthreads)
	{
		init_cthreads();
	}

	return 0;
}

/*
** grava identificação do grupo
*/
int cidentify (char *name, int size)
{
	char grupo[size];
	strcpy(grupo, "Cristiano Salla Lunardi - xxxxxx\nGustavo Madeira Santana - 252853");
	if(strcpy(*name, grupo))
		return 0;
	else
		return -1;
}