/*
**
** cdata.h
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

#ifndef __cdata__
#define __cdata__

#include <support.h>
#include <cthread.h>
#include <ucontext.h>

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

#define TRUE 1
#define FALSE 0
#define CT_STACK_SIZE (10*SIGSTKSZ)

/* NÃO ALTERAR ESSA struct */
typedef struct s_TCB {
	int 				tid;		// identificador da thread
	int 				state;		// estado em que a thread se encontra
// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	int 				ticket;		// 0-255: bilhete de loteria da thread
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos)
} TCB_t;

typedef struct s_JCB {
    int tid;  //tid da thread bloqueante
    TCB_t *thread; // thread bloqueada
} JCB_t;

int find_thread(int tid, PFILA2 fila);
int get_thread(int tid, TCB_t *thread, PFILA2 fila);
int get_jcb(int tid, JCB_t *thread, PFILA2 fila);

#endif
