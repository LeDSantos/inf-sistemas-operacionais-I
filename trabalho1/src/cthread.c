
// typedef struct s_sem {
// 	int	count;	// indica se recurso est� ocupado ou n�o (livre > 0, ocupado = 0)
// 	PFILA2	fila; 	// ponteiro para uma fila de threads bloqueadas no sem�foro
// } csem_t;

int ccreate (void* (*start)(void*), void *arg)
{

}
int cyield(void)
{

}
int cjoin(int tid)
{

}
int csem_init(csem_t *sem, int count)
{

}
int cwait(csem_t *sem)
{

}
int csignal(csem_t *sem)
{

}
