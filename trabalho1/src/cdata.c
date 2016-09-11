/*
**
** cdata.c
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
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

#define CT_STACK_SIZE (10*SIGSTKSZ)

// indicador de inicialização das estruturas de dados
bool has_init_cthreads = false;

// toda thread deve passar controle para o scheduler ao sair de execução
ucontext_t scheduler;

int threadCount = 1;

// estados apto, bloqueado e executando
TCB_t *running_thread;
FILA2 filaAptos;
FILA2 filaBloqueados;

/*
** sorteia uma thread e manda para o dispatcher
*/
void scheduler()
{
	int draw = Random2();

	TCB_t *lucky = malloc(sizeof(TCB_t));
	PNODE2 aux;

	int diff = 255;
	int lowest_tid = threadCount;

	for(aux = filaAptos.first; aux != NULL; aux = aux.next)
	{
		if(aux.node.ticket == draw && aux.node.tid < lowest_tid)
		{
			lowest_tid = aux.node.tid;
			lucky = &aux.node;
		} else if(abs(lucky - aux.node.ticket) <= diff){
			diff = abs(lucky - aux.node.ticket);
			lowest_tid = aux.node.tid;
			lucky = &aux.node;
		}
	}

	for(aux = filaAptos.first; aux != NULL; aux = aux.next)
	{
		if aux.node.tid == lucky.tid;
			aux = NULL;
	}

	DeleteAtIteratorFila2(filaAptos);

	dispatcher(*lucky); //chama pra colocar em exec
}

/*
** coloca thread sorteada em execução
*/
static void dispatcher(TCB_t *thread)
{
	running_thread = &thread;
	thread->state = PROCST_EXEC;
	printf("#Dispatcher#-- entrando em execução a thread: %s\n", ct_to_string(cthread));
	setcontext(&thread->context);
}

/*
** inicialização das estruturas de dados
*/
static void init_cthread()
{
	CreateFila2(&filaAptos);
	CreateFila2(&filaBloqueados);

	// inicialização do scheduler
	getcontext(&scheduler);
	scheduler.uc_link = 0; //scheduler volta para main
	scheduler.uc_stack.ss_sp = malloc(CT_STACK_SIZE);
	scheduler.uc_stack.ss_size = sizeof(CT_STACK_SIZE);
	makecontext(&scheduler, (void (*)(void))scheduler, 0);

	// criação de thread para o main
	TCB_t *main_thread = malloc(sizeof(TCB_t));
	main_thread->tid = 0; // id da main tem que ser 0
	main_thread->state = PROCST_EXEC;
	main_thread->ticket = Random2();
	getcontext(&main_thread->context);

	running_thread = &main_thread;

	has_init_cthreads = TRUE;
}

/*
** retorna identificação do grupo
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

/*
** imprimir dados da thread pra debugar
*/
char *thread_to_string(s_tcb *cthread)
{
	char *template = "{
	%p,
	tid :%d,
	state: %d,
	&context: %p,
	context.uc_link: %p}";
	snprintf((char *) ct_string, sizeof(ct_string), template,
	cthread,
	cthread->tid,
	cthread->state,
	&cthread->context,
	cthread->context.uc_link);
	return (char *) ct_string;
}

int tamanho_fila(PFILA2 fila)
{
	PNODE2 aux;
	int count = 0;
	for(aux = fila->first; aux != NULL; aux = aux->next)
		count++;
	return count;
}
