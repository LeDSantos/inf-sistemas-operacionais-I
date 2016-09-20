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
extern int thread_count = 1;
extern TCB_t running_thread;
extern FILA2 filaAptos;
extern FILA2 filaBloqueados;

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
      swapcontext(&thread_running->context, &dispatcher);
      return 0;
    else
      return -1;
}

/*
** thread em execução vai para bloqueado e espera pela thread com tid recebida
** thread do tid recebido passa contexto para a thread que chamou cjoin ao temrinar sua execução
*/
int cjoin(int tid)
{
  if(!find_thread(tid, &filaAptos) || !find_thread(tid, &filaBloqueados))
  {
    TCB_t *thread = thread_running;
    JCB_t *join_thread = malloc(sizeof(JCB_t));

    join_thread->tid = tid;
    join_thread->thread = thread;

    AppendFila2(&filaBloqueados, (void*)join_thread);

    thread->state = PROCST_BLOQ;

    swapcontext(&thread->context, &dispatcher);

    return 0;
  }
  else
  {
    printf("thread não existe ou já terminou ou não está na fila de aptos\n");
    return -1;
  }
}


/*
** inicializa semáforo
*/
int csem_init(csem_t *sem, int count)
{
  if(!has_init_cthreads)
  {
    init_cthreads();
  }

  sem->count = count;
  FILA2 *fila_sem = malloc(sizeof(30*TCB_t));

  if(CreateFila2(&fila_sem))
  {
    printf("falha ao criar semáforo\n");
    return -1;
  }
    sem->fila = &fila;
    printf("semáforo criado; recursos: %d\n", sem->count);
    return 0;
}

/*
** coloca a thread em execução no semáforo
** se a fila estiver vazia, continua executando
*/
int cwait(csem_t *sem)
{
  if(!has_init_cthreads)
  {
    init_cthreads();
  }

  if(FirstFila2(&(sem->fila)))
  {
    printf("semáforo não foi inicializado corretamente\n");
    return -1
  }

  if(sem->count == 0)
  {
    printf("nenhum recurso disponível, entrou na fila\n");
    running_thread->state = PROCST_BLOQ;
    AppendFila2(&(sem->fila), (void *) &running_thread);
    swapcontext(&running_thread->context, &scheduler);
    return 0;
  }

  sem->count--;
  return 0;
}

/*
** libera recurso ao semáforo
** se tem thread no semáforo, coloca em execução
*/
int csignal(csem_t *sem)
{
  if(!has_init_cthreads)
  {
    init_cthreads();
  }

  if(FirstFila2(&(sem->fila)))
  {
    printf("semáforo não foi inicializado corretamente\n");
    return -1
  }

  if(sem->count == 0)
  {
    FirstFila2(&(sem->fila));

    TCB_t *thread = GetAtIteratorFila2(&(sem->fila));
    thread->state = PROCST_APTO;

    AppendFila2(&filaAptos, (void *) &thread);

    return 0;
  }

  sem->count++;
  return 0;
}

/*
** grava identificação do grupo
*/
int cidentify (char *name, int size)
{
  char grupo[size];
  strcpy(grupo, "Cristiano Salla Lunardi - 240508\nGustavo Madeira Santana - 252853");
  if(strcpy(*name, grupo))
    return 0;
  else
    return -1;
}