/*
**
** cdata.c
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

#define CT_STACK_SIZE (10*SIGSTKSZ)

// indicador de inicialização das estruturas de dados
static bool has_init_cthreads = false;

// toda thread deve passar controle para o scheduler ao sair de execução
static ucontext_t scheduler;

int threadCount = 1;
TCB_t *main_cthread;

// estados apto, bloqueado e executando
TCB_t *curent_cthread;
FILA2 filaAptos;
FILA2 filaBloqueados;

/*
** inicialização das estruturas de dados
*/
void init_cthread()
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
		TCB_t main_cthread;
		main_cthread.tid = 0; // id da main tem que ser 0
		main_cthread.state = PROCST_EXEC;
		main_cthread.ticket = Random2();
		getcontext(&main_cthread.context);

		curent_cthread = &main_cthread;

		has_init_cthreads = TRUE;
}

/*
** sorteia uma thread e manda para o dispatcher
*/
void scheduler()
{
	int draw = Random2();

	TCB_t *lucky;
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
** coloca cthread em execução
*/
void dispatcher(TCB_t *cthread)
{
	curent_cthread = &cthread;
	cthread->state = PROCST_EXEC;
	printf("#Dispatcher#-- entrando em execução a thread: %s\n", ct_to_string(cthread));
	setcontext(&cthread->context);
}

/*
** retorna identificação do grupo
*/
int cidentify (char *name, int size)
{

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