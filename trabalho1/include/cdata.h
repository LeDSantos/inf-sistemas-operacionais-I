/*
**
** cdata.h
** Biblioteca cthreads
**
** Instituto de Inform�tica - UFRGS
** Sistemas Operacionais I N 2016/2
** Prof. Alexandre Carissimi
**
*/

#ifndef __cdata__
#define __cdata__

#define	PROCST_CRIACAO	0
#define	PROCST_APTO	1
#define	PROCST_EXEC	2
#define	PROCST_BLOQ	3
#define	PROCST_TERMINO	4

/* N�O ALTERAR ESSA struct */
typedef struct s_TCB {
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
// 0: Cria��o; 1: Apto; 2: Execu��o; 3: Bloqueado e 4: T�rmino
	int		ticket;		// 0-255: bilhete de loteria da thread
	ucontext_t 	context;	// contexto de execu��o da thread (SP, PC, GPRs e recursos)
} TCB_t;

int cidentify (char *name, int size);
#endif
