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
#include <stdbool.h>
#include <ucontext.h>

#include "../include/cdata.h"
#include "../include/cthread.h"
#include "../include/support.h"

#define CT_STACK_SIZE (10*SIGSTKSZ)

// indicador de inicialização da biblioteca
bool has_init_cthreads;

// toda thread deve passar controle para o scheduler ao sair de execução
ucontext_t scheduler;

int thread_count = 1;

// estados apto, bloqueado e executando
TCB_t running_thread;
FILA2 filaAptos;
FILA2 filaBloqueados;

/*
** sorteia uma thread e manda para o dispatcher
*/
void *scheduler()
{
  if(running_thread->state == PROCST_APTO)
  {
    AppendFila2(&filaAptos, (void *) &running_thread);
  }
  else if(running_thread->state == PROCST_BLOQ)
  {
   AppendFila2(&filaBloqueados, (void *) &running_thread);
  }
  else
  {
    running_thread->state = PROCST_TERMINO;
    cunjoin_thread(running_thread->tid);
    free(running_thread);
  }

  running_thread = 0;

  int draw = Random2();
  int diff = 255;
  int lowest_tid = thread_count;

  FirstFila2(&filaAptos);
  PNODE2 aux_it = filaAptos.it;

  TCB_t *lucky = GetAtIteratorFila2(&filaAptos);
  TCB_t *aux_thread = GetAtIteratorFila2(&filaAptos);

  while(NextFila2(&queue_thread_ready) == 0)
  {
    if(queue_thread_ready.it == NULL)
    {
      break;
    }

    aux_thread = GetAtIteratorFila2(&filaAptos);
    if(aux_thread = NULL) printf("Erro em GetAtIteratorFila2, cdata.c ln 67\n");

    diff = abs(draw - aux_thread->ticket);

    if(aux_thread->ticket == draw && aux_thread->tid < lowest_tid)
    {
      lucky = aux_thread;
      lowest_tid = lucky->tid;
    }
    else if(aux_thread->ticket <= diff)
    {
      if(aux_thread->ticket == diff && aux_thread->tid < lowest_tid)
      {
        lucky = aux_thread;
        lowest_tid = lucky->tid;
      }
      else if(aux->ticket < diff)
      {
        lucky = aux_thread;
        lowest_tid = lucky->tid;
      }
    }
  }

  filaAptos.it = aux_it;
  DeleteAtIteratorFila2(&filaAptos);

  dispatcher(&lucky);

  return 0;
}

/*
** coloca thread sorteada em execução
*/
void dispatcher(TCB_t *thread)
{
  running_thread = &thread;
  thread->state = PROCST_EXEC;
  printf("#Dispatcher#-- entrando em execução a thread: %s\n", ct_to_string(cthread));
  setcontext(&thread->context);
}

/*
** inicialização das estruturas de dados
** criação das threads main e scheduler
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

  // criação de thread para a main
  TCB_t *main_thread = malloc(sizeof(TCB_t));
  main_thread->tid = 0; // id da main tem que ser 0
  main_thread->state = PROCST_EXEC;
  main_thread->ticket = Random2();
  getcontext(&main_thread->context);

  running_thread = &main_thread;

  has_init_cthreads = TRUE;
}

void cunjoin_thread(int tid)
{
  TCB_t *thread;

  if(!get_thread(tid, &thread, filaBloqueados))
  {
    thread = thread->thread;
    AppendFila2(&filaAptos, (void *) &thread);
    DeleteAtIteratorFila2(&filaBloqueados);
    setcontext(&dispatcher);
  }
}

int find_thread(int tid, PFILA2 fila)
{
  TCB_t *thread;
  FirstFila2(fila);

  while(NextFila2(fila) == 0)
  {
    if(fila->it == 0)
    {
      break;
    }

    thread = (TCB_t*)GetAtIteratorFila2(fila);

    if(thread->tid == tid)
    {
      return 0;
    }
  }
  return -1;
}

int get_thread(int tid, TCB_t thread, PFILA2 fila)
{
  FirstFila2(fila);

  while(NextFila2(fila) == 0)
  {
    if(fila->it == 0)
    {
      break;
    }

    *thread = (TCB_t*)GetAtIteratorFila2(fila);

    if(thread->tid == tid)
    {
      return 0;
    }
  }
  return -1;
}