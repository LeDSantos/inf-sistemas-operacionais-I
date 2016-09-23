# INF01142 - Sistemas Operacionais I N
Trabalhos da disciplina de Sistemas Operacionais I N - Instituto de Informática da UFRGS

### Trabalho 1
Biblioteca de threads *cthreads*

Biblioteca de threads em nível de usuário (modelo N:1). Essa biblioteca de threads, denominada de compact thread (ou apenas cthread), deverá oferecer capacidades básicas para programação com threads como criação, execução, sincronização, término e trocas de contexto.

```c
int ccreate (void *(*start)(void *), void *arg);

Parâmetros:
start: ponteiro para a função que a thread executará.
arg: um parâmetro que pode ser passado para a thread na sua criação. (Obs.: é um único parâmetro. Se for necessário passar mais de um valor deve-se empregar um ponteiro para uma struct)
Retorno:
Quando executada corretamente: retorna um valor positivo, que representa o identificador da thread criada Caso contrário, retorna um valor negativo.
```

```c
int cyield(void);

Retorno:
Quando executada corretamente: retorna 0 (zero) Caso contrário, retorna um valor negativo.
```

```c
int csem_init (csem_t *sem, int count);

Parâmetros:
sem: ponteiro para uma variável do tipo csem_t. Aponta para uma estrutura de dados que representa a variável semáforo. count: valor a ser usado na inicialização do semáforo. Representa a quantidade de recursos controlador pelo semáforo.
Retorno:
Quando executada corretamente: retorna 0 (zero) Caso contrário, retorna um valor negativo.
```

```c
int cwait (csem_t *sem);

Parâmetros:
sem: ponteiro para uma variável do tipo semáforo.
Retorno:
Quando executada corretamente: retorna 0 (zero) Caso contrário, retorna um valor negativo.
```

```c
int csignal (csem_t *sem);

Parâmetros:
sem: ponteiro para uma variável do tipo semáforo. Retorno:
Quando executada corretamente: retorna 0 (zero) Caso contrário, retorna um valor negativo.
```

```c
int cidentify (char *name, int size);

Parâmetros:
name: ponteiro para uma área de memória onde deve ser escrito um string que contém os nomes dos componentes do grupo e seus números de cartão. Deve ser uma linha por componente.
size: quantidade máxima de caracteres que podem ser copiados para o string de identificação dos componentes do grupo.
Retorno:
Quando executada corretamente: retorna 0 (zero) Caso contrário, retorna um valor negativo.
```

### Trabalho 2
Sistema de arquivos