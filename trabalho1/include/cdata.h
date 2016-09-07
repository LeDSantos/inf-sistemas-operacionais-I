/*
**
** cdata.h
** Biblioteca cthreads
**
** Instituto de Informática - UFRGS
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

/* NÃO ALTERAR ESSA struct */
typedef struct s_TCB {
	int		tid; 		// identificador da thread
	int		state;		// estado em que a thread se encontra
// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
	int		ticket;		// 0-255: bilhete de loteria da thread
	ucontext_t 	context;	// contexto de execução da thread (SP, PC, GPRs e recursos)
} TCB_t;

int cidentify (char *name, int size);
#endif
