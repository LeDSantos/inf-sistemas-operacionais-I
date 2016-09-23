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
#include <ucontext.h>
#include <support.h>

#include <cthread.h>
#include <cdata.h>

#include <stdio.h>
#include <stdlib.h>




// indicador de inicialização da biblioteca
int has_init_cthreads = 0;
int thread_count = 1;

// toda thread deve passar controle para o scheduler ao sair de execução
ucontext_t scheduler;
char ss_scheduler[SIGSTKSZ];

// malloc pra main estava dando segfault
TCB_t main_thread;

// estados apto, bloqueado e executando
FILA2 filaAptos;
FILA2 filaBloqueados;
TCB_t *running_thread;

// fila do join
FILA2 filaJCB;


/*
** verifica se existe thread para dar unjoin
*/
void cunjoin_thread(int tid)
{
  JCB_t *jcb;
  TCB_t *thread;

  FirstFila2(&filaJCB);
  while(NextFila2(&filaJCB) == 0)
  {
    if(filaJCB.it == 0)
    {
      break;
    }

    jcb = (JCB_t *)GetAtIteratorFila2(&filaJCB);
    if(jcb->tid == tid)
    {
      break;
    }
  }

  FirstFila2(&filaBloqueados);
  while(NextFila2(&filaBloqueados) == 0)
  {
    if(filaBloqueados.it == 0)
    {
      break;
    }

    thread = (TCB_t *)GetAtIteratorFila2(&filaBloqueados);
    if(jcb->thread->tid == thread->tid)
    {
      DeleteAtIteratorFila2(&filaJCB);
      remove_thread(thread->tid, &filaBloqueados);

      free(jcb);

      thread->state = PROCST_APTO;
      AppendFila2(&filaAptos, (void *) thread);

      break;
    }

  }
}

/*
** sorteia uma thread e manda para o dispatcher
*/
void *cscheduler()
{
  printf("#scheduler em ação\n\n\n");
  if(running_thread)
  {
    running_thread->state = PROCST_TERMINO;
    cunjoin_thread(running_thread->tid);

    free(running_thread->context.uc_stack.ss_sp);
    free(running_thread);

    running_thread = NULL;
  }

  int draw = ticket_gen();
  int min_diff = 255;
  int diff = 255;
  int lowest_tid = thread_count;

  printf("#scheduler:\nnum sorteado: %d\nnum threads criadas: %d\n\n", draw, lowest_tid);

  if(FirstFila2(&filaAptos) != 0)
  {
    printf("fila aptos está vazia, segue execução\n\n");
    return;
  }

  // PNODE2 aux_it = filaAptos.it;

  TCB_t *lucky;
  TCB_t *aux_thread;

  aux_thread = (TCB_t *)GetAtIteratorFila2(&filaAptos);

  diff = abs(draw - aux_thread->ticket);
  min_diff = diff;
  lowest_tid = aux_thread->tid;
  lucky = aux_thread;
  // printf("procurando ganhador:\ntid: %d\nticket: %d\ndiff: %d\n\n", aux_thread->tid, aux_thread->ticket, diff);
  // printf("atual ganhador, tid: %d\n\n", lucky->tid);

  while(NextFila2(&filaAptos) == 0)
  {
    if(filaAptos.it == NULL)
    {
      break;
    }
    aux_thread = (TCB_t *)GetAtIteratorFila2(&filaAptos);
    diff = abs(draw - aux_thread->ticket);
    // printf("procurando ganhador:\ntid: %d\nticket: %d\ndiff: %d\n\n", aux_thread->tid, aux_thread->ticket, diff);

    if(aux_thread->ticket == draw && aux_thread->tid < lowest_tid)
    {
      lucky = aux_thread;
      lowest_tid = lucky->tid;
      // aux_it = filaAptos.it;
      min_diff = diff;
      // printf("atual ganhador, tid: %d\n\n", lucky->tid);
    }
    else if(diff <= min_diff)
    {
      if(diff == min_diff && aux_thread->tid < lowest_tid)
      {
        lucky = aux_thread;
        lowest_tid = lucky->tid;
        // aux_it = filaAptos.it;
        min_diff = diff;
        // printf("atual ganhador, tid: %d\n\n", lucky->tid);
      }
      else if(diff < min_diff)
      {
        lucky = aux_thread;
        lowest_tid = lucky->tid;
        // aux_it = filaAptos.it;
        min_diff = diff;
        // printf("atual ganhador, tid: %d\n\n", lucky->tid);
      }
    }
  }
  printf("vencedor:\ntid: %d\nticket: %d\ndiff: %d\n\n", lucky->tid, lucky->ticket, diff);
  // filaAptos.it = aux_it;
  // DeleteAtIteratorFila2(&filaAptos);

/*
** dispatcher
** coloca thread sorteada em execução
*/
  running_thread = lucky;
  remove_thread(running_thread->tid, &filaAptos);
  lucky->state = PROCST_EXEC;

  setcontext(&lucky->context);
}

/*
** inicialização das estruturas de dados
** criação das threads main e scheduler
*/
void init_cthreads()
{
  printf("#init_cthreads em ação\n\n\n");
  char *aaa;
  int bbb;
  cidentify(aaa, bbb);

  if(CreateFila2(&filaAptos) != 0)
  {
    printf("falha ao criar fila aptos\n");
  }
  if(CreateFila2(&filaBloqueados) != 0)
  {
    printf("falha ao criar fila bloqueados\n");
  }
  if(CreateFila2(&filaJCB) != 0)
  {
    printf("falha ao criar fila join\n");
  }

  // inicialização do scheduler
  getcontext(&scheduler);
  scheduler.uc_link = &main_thread.context; //scheduler volta para main
  scheduler.uc_stack.ss_sp = ss_scheduler;
  scheduler.uc_stack.ss_size = SIGSTKSZ;
  makecontext(&scheduler, (void (*)(void))cscheduler, 0);

  // inicializa main
  main_thread.tid = 0; // id da main tem que ser 0
  main_thread.state = PROCST_EXEC;
  main_thread.ticket = ticket_gen();
  getcontext(&main_thread.context);

  // precisa malloc pra stack da main?
  // main_thread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
  // main_thread->context.uc_stack.ss_size = SIGSTKSZ;

  running_thread = &main_thread;

  has_init_cthreads = 1;
}

/*
** cria uma thread e a coloca na fila de aptos
*/
int ccreate (void* (*start)(void*), void *arg)
{
  if(has_init_cthreads == 0)
  {
    init_cthreads();
  }
  TCB_t *cthread = malloc(sizeof(TCB_t));
  cthread->tid = thread_count; thread_count++;
  cthread->state = PROCST_CRIACAO;
  cthread->ticket = ticket_gen();
  // cthread->context = malloc(sizeof(ucontext_t));
  getcontext(&cthread->context);
  cthread->context.uc_link = &scheduler;
  cthread->context.uc_stack.ss_sp = malloc(SIGSTKSZ);
  cthread->context.uc_stack.ss_size = SIGSTKSZ;
  //cthread->context.uc_stack.ss_flags = 0;

  makecontext(&(cthread->context), (void (*)(void)) start, 1, &arg);

  cthread->state = PROCST_APTO;

  if(AppendFila2(&filaAptos, (void *) cthread) != 0)
  {
    printf("#ccreate: falha ao criar tid %d\n", cthread->tid);
    return -1;
  }
  printf("#ccreate:\nthread tid %d\nestado: %d\nticket: %d\n\n", cthread->tid, cthread->state, cthread->ticket);
  return cthread->tid;
}

/*
** coloca a thread atual na fila de aptos e passa o controle para o scheduler
*/
int cyield(void)
{
  if(has_init_cthreads == 0)
  {
    init_cthreads();
  }

  if(FirstFila2(&filaAptos) != 0)
  {
    printf("fila aptos vazia, segue executando\n");
    return 0;
  }

  TCB_t *thread;
  thread = running_thread;
  thread->state = PROCST_APTO;

  running_thread = NULL;

  if(AppendFila2(&filaAptos, (void *) thread) != 0)
  {
    return -1;
  }
  swapcontext(&thread->context, &scheduler);
  return 0;
}

/*
** thread em execução vai para bloqueado e espera pela thread com tid recebida
** thread do tid recebido passa contexto para a thread que chamou cjoin ao temrinar sua execução
*/
int cjoin(int tid)
{
  if(has_init_cthreads == 0)
  {
    init_cthreads();
  }

  if(find_thread(tid, &filaAptos) != 0)
  {
    if(find_thread(tid, &filaBloqueados) != 0){
    printf("#cjoin: tid %d não existe ou já terminou\n\n", tid);
    return -1;
    }
  }

  TCB_t *thread;
  thread = running_thread;

  JCB_t *jcb = malloc(sizeof(JCB_t));
  jcb->tid = tid;
  printf("#cjoin: jcb->tid: %d\n", jcb->tid);
  jcb->thread = thread;

  thread->state = PROCST_BLOQ;
  if(AppendFila2(&filaBloqueados, (void*) jcb) != 0)
  {
    printf("#cjoin: erro ao inserir na fila bloqueados\n\n");
  }

  running_thread = NULL;

  swapcontext(&thread->context, &scheduler);
  return 0;
}


/*
** inicializa semáforo
*/
int csem_init(csem_t *sem, int count)
{
  if(has_init_cthreads == 0)
  {
    init_cthreads();
  }

  sem->count = count;
  sem->fila = malloc(sizeof(FILA2));

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
  if(has_init_cthreads == 0)
  {
    init_cthreads();
  }

  if(sem->fila == NULL)
  {
    sem->fila = malloc(sizeof(FILA2));
    CreateFila2(sem->fila);
  }

  sem->count--;

  if(sem->count < 0)
  {
    printf("nenhum recurso disponível, entrou na fila\n recursos: %d\n", sem->count);

    TCB_t *thread = running_thread;
    thread->state = PROCST_BLOQ;

    AppendFila2(sem->fila, (void *) thread);
    AppendFila2(&filaBloqueados, (void *) thread);

    running_thread = NULL;

    swapcontext(&thread->context, &scheduler);
    return 0;
  }

  printf("recursos: %d\n", sem->count);
  return 0;
}

/*
** libera recurso ao semáforo
** se tem thread no semáforo, coloca em execução
*/
int csignal(csem_t *sem)
{
  if(has_init_cthreads == 0)
  {
    init_cthreads();
  }

  if(sem->fila == NULL)
  {
    printf("semáforo não inicializado ou usou signal antes de wait\n");
    return -1;
  }

  if(FirstFila2(sem->fila))
  {
    printf("semáforo vazio, liberando\n");
    free(sem->fila);
    sem->fila = NULL;
    return 0;
  }

  sem->count++;

  if(sem->count > 0)
  {
    if(FirstFila2(sem->fila))
    {
      printf("erro firstfila csignal ln.407\n");
    }

    TCB_t *thread = (TCB_t *)GetAtIteratorFila2(sem->fila);
    thread->state = PROCST_APTO;
    DeleteAtIteratorFila2(sem->fila);

    if(remove_thread(thread->tid, &filaBloqueados) != 0)
    {
      printf("falha ao remover thread // cthread ln.416\n");
      return -1;
    }

    AppendFila2(&filaAptos, (void *) thread);

    printf("recursos: %d\n", sem->count);
    return 0;
  }

  printf("recursos: %d\n", sem->count);
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

  // professor falou que podia ser apenas um printf
  printf("Cristiano Salla Lunardi - 240508\nGustavo Madeira Santana - 252853\n\n");

  return 0;
}