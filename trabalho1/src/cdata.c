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

//indicador de inicialização das estruturas de dados
static bool has_init_cthreads = false;

// toda thread deve passar controle para o scheduler ao sair de execução
static ucontext_t scheduler;

int ct_tid = 1;
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
		scheduler.uc_link = 0; //scheduler volta para main ao terminar
		scheduler.uc_stack.ss_sp = malloc(CT_STACK_SIZE);
		scheduler.uc_stack.ss_size = sizeof(CT_STACK_SIZE);
		makecontext(&dispatcher, (void (*)(void))scheduler, 0);

		// criação de thread para o main
		TCB_t main_cthread;
		main_cthread.tid = 0;
		main_cthread.state = 1;
		main_cthread.ticket = Random2();
		getcontext(&main_cthread.context);

		curent_cthread = &main_cthread;

		is_init_cthreads = TRUE;
}

TCB_t *getCurrentThread()
{
	return curent_cthread;
}

/*
** coloca cthread em execução
*/
void dispatcher(TCB_t *cthread)
{
	curent_cthread = cthread;
	cthread->state = 2;
	printf("#Dispatcher: entrando em execução thread: %s\n", ct_to_string(cthread));
	setcontext(&cthread->context);
}

/*
** sorteia uma thread e manda para o dispatcher
*/
void schduler()
{
	int drawn = Random2();

	for (int i = 0; i < count; ++i)
	{
		/* code */
	}
	TCB_t *cthread_t;
	//percore filaAptos até achar mais próximo de drawn

	cthread_t = &thread_escolhida

	dispatcher(*cthread); //chama pra colocar em exec
}

/*
** retorna identificação do grupo
*/
int cidentify (char *name, int size)
{

}

/*
 * Usada para fins de debug.
 * Retorna uma representação da thread em um string.
 * A string retornada está em uma variável global e já é alocada pelo compilador.
 * Isso foi feito por simplicidade e para eliminar o memory leak caso fosse alocada dinâmicamente.
 * CUIDADO: cada chamada à essa função sobrepõe a variável global.
 */
char *ct_to_string(s_tcb *cthread)
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