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

#include <cthread.h>

// indicador de inicialização da biblioteca
int has_init_cthreads = FALSE;
int thread_count = 1;

// toda thread deve passar controle para o scheduler ao sair de execução
ucontext_t scheduler;

// estados apto, bloqueado e executando
TCB_t *running_thread;
FILA2 filaAptos;
FILA2 filaBloqueados;

// fila do join
FILA2 filaJCB;

/*
** verifica se existe thread para dar unjoin
*/
void cunjoin_thread(int tid)
{
  JCB_t *join_thread;
  TCB_t *thread;

  if(!get_jcb(tid, &join_thread, &filaJCB))
  {
    thread = join_thread->thread;
    AppendFila2(&filaAptos, (void *) thread);
    DeleteAtIteratorFila2(&filaBloqueados);
    setcontext(&scheduler);
  }
}

/*
** sorteia uma thread e manda para o dispatcher
*/
void *cscheduler()
{
  if(running_thread->state == PROCST_APTO)
  {
    AppendFila2(&filaAptos, (void *) running_thread);
  }
  else if(running_thread->state == PROCST_BLOQ)
  {
   AppendFila2(&filaBloqueados, (void *) running_thread);
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

  while(NextFila2(&filaAptos) == 0)
  {
    if(filaAptos.it == NULL)
    {
      break;
    }

    aux_thread = GetAtIteratorFila2(&filaAptos);
    if(aux_thread = NULL) printf("Erro em GetAtIteratorFila2, cthread.c ln 78\n");

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
      else if(aux_thread->ticket < diff)
      {
        lucky = aux_thread;
        lowest_tid = lucky->tid;
      }
    }
  }

  filaAptos.it = aux_it;
  DeleteAtIteratorFila2(&filaAptos);

/*
** dispatcher
** coloca thread sorteada em execução
*/

  running_thread = lucky;
  lucky->state = PROCST_EXEC;
  setcontext(&lucky->context);
}

/*
** inicialização das estruturas de dados
** criação das threads main e scheduler
*/
void init_cthreads()
{
  CreateFila2(&filaAptos);
  CreateFila2(&filaBloqueados);
  CreateFila2(&filaJCB);

  // inicialização do scheduler
  getcontext(&scheduler);
  scheduler.uc_link = 0; //scheduler volta para main
  scheduler.uc_stack.ss_sp = malloc(SIGSTKSZ);
  scheduler.uc_stack.ss_size = SIGSTKSZ;
  makecontext(&scheduler, (void (*)(void))cscheduler, 0);

  // criação de thread para a main
  TCB_t *main_thread = malloc(sizeof(TCB_t));
  main_thread->tid = 0; // id da main tem que ser 0
  main_thread->state = PROCST_EXEC;
  main_thread->ticket = Random2();
  getcontext(&main_thread->context);

  // precisa malloc pra stack da main?
  // main_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
  // main_thread->context.uc_stack.ss_size = SIGSTKSZ;

  running_thread = main_thread;

  has_init_cthreads = TRUE;
}

/*
** cria uma thread e a coloca na fila de aptos
*/
int ccreate (void* (*start)(void*), void *arg)
{
  if(has_init_cthreads == FALSE)
  {
    init_cthreads();
  }

  TCB_t *cthread = malloc(sizeof(TCB_t));
  cthread->tid = thread_count; thread_count++;
  cthread->state = PROCST_CRIACAO;
  cthread->ticket = Random2();
  // cthread->context = malloc(sizeof(ucontext_t));
  getcontext(&(cthread->context));

  cthread->context.uc_link = &scheduler;
  cthread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
  cthread->context.uc_stack.ss_size = SIGSTKSZ;
  //cthread->context.uc_stack.ss_flags = 0;

  makecontext(&(cthread->context), (void (*)(void)) start, 1, &arg);

  cthread->state = PROCST_APTO;

  if(AppendFila2(&filaAptos, (void *) cthread))
  {
    return -1;
  }
  return cthread->tid;
}

/*
** coloca a thread atual na fila de aptos e passa o controle para o scheduler
*/
int cyield(void)
{
  if(has_init_cthreads == FALSE)
  {
    init_cthreads();
  }

  running_thread->state = PROCST_APTO;

  if(running_thread->state == PROCST_APTO)
  {
    swapcontext(&running_thread->context, &scheduler);
    return 0;
  }
  else
    return -1;
}

/*
** thread em execução vai para bloqueado e espera pela thread com tid recebida
** thread do tid recebido passa contexto para a thread que chamou cjoin ao temrinar sua execução
*/
int cjoin(int tid)
{
  if(find_thread(tid, &filaAptos) || find_thread(tid, &filaBloqueados))
  {
    printf("thread não existe ou já terminou ou não está na fila de aptos\n");
    return -1;
  }

  TCB_t *thread = running_thread;
  JCB_t *join_thread = malloc(sizeof(JCB_t));

  join_thread->tid = tid;
  join_thread->thread = thread;

  AppendFila2(&filaBloqueados, (void*) join_thread);

  thread->state = PROCST_BLOQ;

  swapcontext(&thread->context, &scheduler);

  return 0;
}


/*
** inicializa semáforo
*/
int csem_init(csem_t *sem, int count)
{
  if(has_init_cthreads == FALSE)
  {
    init_cthreads();
  }

  sem->count = count;
  sem->fila = malloc(sizeof(filaAptos));

  if(CreateFila2(sem->fila))
  {
    printf("falha ao criar semáforo\n");
    return -1;
  }
    printf("semáforo criado; recursos: %d\n", sem->count);
    return 0;
}

/*
** coloca a thread em execução no semáforo
** se a fila estiver vazia, continua executando
*/
int cwait(csem_t *sem)
{
  if(has_init_cthreads == FALSE)
  {
    init_cthreads();
  }

  if(FirstFila2(&(sem->fila)))
  {
    printf("semáforo não foi inicializado corretamente\n");
    return -1;
  }

  sem->count--;

  if(sem->count < 0)
  {
    printf("nenhum recurso disponível, entrou na fila\n");
    running_thread->state = PROCST_BLOQ;
    AppendFila2(&(sem->fila), (void *) running_thread);
    swapcontext(&running_thread->context, &scheduler);
    return 0;
  }

  return 0;
}

/*
** libera recurso ao semáforo
** se tem thread no semáforo, coloca em execução
*/
int csignal(csem_t *sem)
{
  if(has_init_cthreads == FALSE)
  {
    init_cthreads();
  }

  if(FirstFila2(&(sem->fila)))
  {
    printf("semáforo não foi inicializado corretamente\n");
    return -1;
  }

  sem->count++;

  if(sem->count > 0)
  {
    FirstFila2(&(sem->fila));

    TCB_t *thread = GetAtIteratorFila2(&(sem->fila));
    thread->state = PROCST_APTO;

    AppendFila2(&filaAptos, (void *) thread);

    return 0;
  }

  return 0;
}

/*
** grava identificação do grupo
*/
int cidentify (char *name, int size)
{
  // char *aux = name;
  // char grupo[size];
  // strcpy(grupo, "Cristiano Salla Lunardi - 240508\nGustavo Madeira Santana - 252853");
  // strcpy(aux, grupo);

  printf("Cristiano Salla Lunardi - 240508\nGustavo Madeira Santana - 252853\n");

  return 0;
}