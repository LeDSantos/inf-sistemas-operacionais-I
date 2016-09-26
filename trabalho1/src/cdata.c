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

#include <support.h>
#include <ucontext.h>

#include <cthread.h>
#include <cdata.h>

#include <stdlib.h>
#include <stdio.h>

extern int debug;

unsigned int ticket_gen()
{
  unsigned int random = Random2();
  while(random >= 255)
  {
    random = random/8;
  }

  return random;
}

int find_thread(int tid, PFILA2 fila)
{

  TCB_t *thread;
  if(FirstFila2(fila) != 0)
  {
    if(debug == 1)
    {
      printf("#find_thread: FirstFila2: fila vazia ou erro\n\n");
    }

    return -1;
  }

  thread = (TCB_t *)GetAtIteratorFila2(fila);
  if(thread->tid == tid)
  {
    if(debug == 1)
    {
      printf("thread encontrada! tid: %d\n\n", tid);
    }

    return 0;
  }

  while(NextFila2(fila) == 0)
  {
    if(fila->it == 0)
    {
      break;
    }
    else
    {
      thread = (TCB_t *)GetAtIteratorFila2(fila);
      if(thread->tid == tid)
      {
        if(debug == 1)
        {
          printf("thread encontrada! tid: %d\n\n", tid);
        }

        return 0;
      }
    }
  }

  return -1;
}

int remove_thread(int tid, PFILA2 fila)
{
  TCB_t *thread;
  FirstFila2(fila);

  thread = (TCB_t *)GetAtIteratorFila2(fila);

  if(thread->tid == tid)
  {
    DeleteAtIteratorFila2(fila);
    return 0;
  }

  while(NextFila2(fila) == 0)
  {
    if(fila->it == 0)
    {
      break;
    }

    thread = (TCB_t *)GetAtIteratorFila2(fila);

    if(thread->tid == tid)
    {
      DeleteAtIteratorFila2(fila);
      return 0;
    }
  }
  return -1;
}