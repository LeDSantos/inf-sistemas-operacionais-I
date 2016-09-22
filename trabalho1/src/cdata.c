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

#include <cdata.h>

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

int get_thread(int tid, TCB_t *thread, PFILA2 fila)
{
  TCB_t *cthread = thread;
  FirstFila2(fila);

  while(NextFila2(fila) == 0)
  {
    if(fila->it == 0)
    {
      break;
    }

    cthread = (TCB_t*)GetAtIteratorFila2(fila);

    if(cthread->tid == tid)
    {
      return 0;
    }
  }
  return -1;
}

int get_jcb(int tid, JCB_t *thread, PFILA2 fila)
{
  JCB_t *cthread = thread;
  FirstFila2(fila);

  while(NextFila2(fila) == 0)
  {
    if(fila->it == 0)
    {
      break;
    }

    cthread = (TCB_t*)GetAtIteratorFila2(fila);

    if(cthread->tid == tid)
    {
      return 0;
    }
  }
  return -1;
}