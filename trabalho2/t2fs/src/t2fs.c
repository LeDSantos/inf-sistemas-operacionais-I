/*
**
** t2fs.c
** Task 2 File System
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
#include <string.h>

#include "../include/t2fs.h"
#include "../include/functions.h"
#include "../include/apidisk.h"
#include "../include/bitmap2.h"

#define GROUP "Gustavo Madeira Santana - 252853, Cristiano Salla Lunardi - 240508"

#define SUCCESS 0
#define ERROR -1

#define ID "T2FS"
#define VERSION 0x7E02
#define DISK "t2fs_disk.dat"

//#define SECTOR_SIZE 256
#define SECTOR_COUNT 32768
#define BLOCK_SIZE 16*RECORD_SIZE
#define BLOCK_COUNT 2048 //(superblock->diskSize / superblock->blockSize), 32768 div 16 = 2048
// #define INODE_COUNT 2048
#define RECORD_SIZE 64
#define MAX_FILESIZE 352 //superblock->blockSize * (( ((superblock->blockSize)*(superblock->blockSize))/16) + ((superblock->blockSize)/4) + 2)
#define MAX_OPENFILES 20

// setor onde começa cada area
#define superblock_sector 0
#define block_bitmap 1
#define inode_bitmap 2
#define inode_area 3
#define data_area 128

/* disk struct
// superblock
// bitmap blocks
// bitmap inodes
// inodes
// data
*/

typedef struct directory_struct DIR_t;
struct directory_struct {
    char name[32];
    DIR_t* pai;
    DIR_t* filho;
    DIR_t* irmao;
    REC_t* record;
};

static int disk_initialized = 0;
char buffer[SECTOR_SIZE];
SB_t* superblock;
REC_t* current_record;
DIR_t* current_dir;
DIR_t root;


/*
** inicializa t2fs
** criacao do superbloco
*/

void disk_info()
{
  printf("\n\n>> T2FS inicializando...\n");
  printf("ID: %s\n", superblock->id);
  printf("Versao: 0x%x\n", superblock->version);
  printf("Informacoes do disco (dados em numero de setores):\n");
  printf("Tamanho super bloco: %hu\n", superblock->superblockSize);
  printf("Tamanho bitmap blocos livres: %hu\n", superblock->freeBlocksBitmapSize);
  printf("Tamanho bitmap inodes livres: %hu\n", superblock->freeInodeBitmapSize);
  printf("Tamanho area inodes: %hu\n", superblock->inodeAreaSize);
  printf("Tamanho bloco logico: %hu\n", superblock->blockSize);
  printf("Tamanho area dados: %u\n\n\n", superblock->diskSize);
}

void disk_init()
{
  superblock = malloc(sizeof(superblock));

  getBitmap2(BITMAP_INODE, 0); //nao faco ideia por que, mas se tirar isso para de funcionar tudo
  // printf("valor do bitmap inode 0: %d\n", x);

  if (read_sector(superblock_sector, buffer) != 0)
  {
    printf("Erro ao ler setor do super bloco\n");
    exit(ERROR);
  }

  strncpy(superblock->id, buffer, 4);
  if (strncmp(superblock->id, ID, 4) != 0)
  {
    printf("ID no disco nao reonhecida\n");
    exit(ERROR);
  }

  superblock->version = *((WORD *)(buffer + 4));
  if (superblock->version != VERSION)
  {
    printf("Versao no disco nao reonhecida\n");
    exit(ERROR);
  }

  superblock->superblockSize = *((WORD *)(buffer + 6));
  superblock->freeBlocksBitmapSize = *((WORD *)(buffer + 8));
  superblock->freeInodeBitmapSize = *((WORD *)(buffer + 10));
  superblock->inodeAreaSize = *((WORD *)(buffer + 12));
  superblock->blockSize = *((WORD *)(buffer + 14));
  superblock->diskSize = *((DWORD *)(buffer + 16));

  disk_info();

  // verificar o que já existe no disco e estrutura de diretorios

  // inicializar estruturas auxiliares para percorrer diretorios
  current_dir = malloc(sizeof(DIR_t));

  char auxname[] = "root";
  strncpy(root.name, auxname, 4);
  root.pai = NULL;
  root.irmao = NULL;
  root.filho = NULL;
  root.record = NULL;

  DIR_t* auxdir = malloc(sizeof(DIR_t));
  auxdir->pai = NULL;
  auxdir->irmao = NULL;
  auxdir->filho = NULL;
  auxdir->record = NULL;

  if(read_sector(data_area, buffer) != 0 )
  {
    printf("Erro ao ler setor de dados: 0\n");
  }

  current_record = malloc(sizeof(RECORD_SIZE + 1));
  current_record->TypeVal = -1;

  printf("Conteudo existente no disco:\n\n");

  int iterator = 0;       //um setor pode ter no maximo 4 records (64 * 4 = 256)
  int read_x_times = 0;   //iterador pra ler setores contiguamente, incrementado toda vez que faz um read sector
  while (iterator < 4)
  {
    if (iterator == 3)
    {
      iterator = 0;
      ++read_x_times;
      if(read_sector(data_area + read_x_times, buffer) != 0)
      {
        printf("Erro ao ler setor do de dados começando no setor: %d\n", read_x_times);
      }
    }
  current_record->TypeVal = *((BYTE *)(buffer + iterator*64));
  strncpy(current_record->name, buffer + iterator*64 + 1, 32);
  current_record->blocksFileSize = *((DWORD *)(buffer + iterator*64 + 33));
  current_record->bytesFileSize = *((DWORD *)(buffer + iterator*64 + 37));
  current_record->inodeNumber = *((int *)(buffer + iterator*64 + 41));

  if (current_record->TypeVal == 0x00)
  {
    break;
  }

  printf("name arquivo: %s\n", current_record->name);
  printf("tipo: %x                >>|1: arquivo, 2: dir, 3: invalido|\n", current_record->TypeVal);
  printf("blocks file size: %u\n", current_record->blocksFileSize);
  printf("bytes file size: %u\n", current_record->bytesFileSize);
  printf("inode number: %d\n\n", current_record->inodeNumber);

  // adicionar diretorios na estrutura para percorrer diretorios
  if (current_record->TypeVal == 0x02)
  {

    auxdir->pai = &root;
    auxdir->record = current_record;
    strncpy(auxdir->name, current_record->name, 32);
    root.filho = auxdir;
  }

  ++iterator;
  }

  current_dir = &root;
  printf("Diretorio raiz: %s : '/'\n", current_dir->name);

  current_dir = root.filho;
  printf("subdiretorios na raiz:\n", current_dir->name);
  do
  {
    printf("%s\n", current_dir->name);
    current_dir = current_dir->irmao;
  } while(current_dir);

  printf("\n\n>> T2FS inicializado!\n\n");
}


int identify2 (char *name, int size)
{
  printf("\nGustavo Madeira Santana - 252853\nCristiano Salla Lunardi - 240508\n\n");
  disk_info();

  if(size < sizeof(GROUP))
  {
    return ERROR;
  }

  strncpy(name, GROUP, sizeof(GROUP));

  return SUCCESS;
}

/*-----------------------------------------------------------------------------
Função: Criar um novo arquivo.
  O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
  O contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
  Caso já exista um arquivo ou diretório com o mesmo nome, a função deverá retornar um erro de criação.
  A função deve retornar o identificador (handle) do arquivo.
  Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.

Entra:  filename -> nome do arquivo a ser criado.

Saída:  Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo).
  Em caso de erro, deve ser retornado um valor negativo.
-----------------------------------------------------------------------------*/
FILE2 create2 (char *filename)
{

  /*
  Criar um arquivo:
  bitmap blocos livres
  */
  int handle;

  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(filename = NULL)
  {
    return ERROR;
  }

  return handle;
}

/*-----------------------------------------------------------------------------
Função: Apagar um arquivo do disco.
  O nome do arquivo a ser apagado é aquele informado pelo parâmetro "filename".

Entra:  filename -> nome do arquivo a ser apagado.

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int delete2 (char *filename)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(filename = NULL)
  {
    return ERROR;
  }

  return SUCCESS;
}


/*-----------------------------------------------------------------------------
Função: Abre um arquivo existente no disco.
  O nome desse novo arquivo é aquele informado pelo parâmetro "filename".
  Ao abrir um arquivo, o contador de posição do arquivo (current pointer) deve ser colocado na posição zero.
  A função deve retornar o identificador (handle) do arquivo.
  Esse handle será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do arquivo criado.
  Todos os arquivos abertos por esta chamada são abertos em leitura e em escrita.
  O ponto em que a leitura, ou escrita, será realizada é fornecido pelo valor current_pointer (ver função seek2).

Entra:  filename -> nome do arquivo a ser apagado.

Saída:  Se a operação foi realizada com sucesso, a função retorna o handle do arquivo (número positivo)
  Em caso de erro, deve ser retornado um valor negativo
-----------------------------------------------------------------------------*/
FILE2 open2 (char *filename)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  char *path[25];
  int dirs = path_parser(filename, &path);

  printf("dirs found:\n");

  int i;
  for (i = 0; i < dirs; ++i)
  {
    printf("%s\n", path[i]);
  }

  printf("Blocos ocupados:\n");
  int ii = 0;
  int bmp;
  for (ii = 0; ii < 1250; ++ii)
  {
     bmp = getBitmap2(BITMAP_INODE, ii);
     if (bmp == 1) printf("inode %d: %d\n", ii, bmp);
     bmp = getBitmap2(BITMAP_DADOS, ii);
     if (bmp == 1) printf("dados %d: %d\n", ii, bmp);
  }

  int handle = 0;

  if(filename = NULL)
  {
    return ERROR;
  }

  return handle;
}


/*-----------------------------------------------------------------------------
Função: Fecha o arquivo identificado pelo parâmetro "handle".

Entra:  handle -> identificador do arquivo a ser fechado

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int close2 (FILE2 handle)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(handle = NULL)
  {
    return ERROR;
  }

  return SUCCESS;
}

/*-----------------------------------------------------------------------------
Função: Realiza a leitura de "size" bytes do arquivo identificado por "handle".
  Os bytes lidos são colocados na área apontada por "buffer".
  Após a leitura, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último lido.

Entra:  handle -> identificador do arquivo a ser lido
  buffer -> buffer onde colocar os bytes lidos do arquivo
  size -> número de bytes a serem lidos

Saída:  Se a operação foi realizada com sucesso, a função retorna o número de bytes lidos.
  Se o valor retornado for menor do que "size", então o contador de posição atingiu o final do arquivo.
  Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int read2 (FILE2 handle, char *buffer, int size)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(handle = NULL)
  {
    return ERROR;
  }

  int bytesread;

  return bytesread;
}


/*-----------------------------------------------------------------------------
Função: Realiza a escrita de "size" bytes no arquivo identificado por "handle".
  Os bytes a serem escritos estão na área apontada por "buffer".
  Após a escrita, o contador de posição (current pointer) deve ser ajustado para o byte seguinte ao último escrito.

Entra:  handle -> identificador do arquivo a ser escrito
  buffer -> buffer de onde pegar os bytes a serem escritos no arquivo
  size -> número de bytes a serem escritos

Saída:  Se a operação foi realizada com sucesso, a função retorna o número de bytes efetivamente escritos.
  Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
int write2 (FILE2 handle, char *buffer, int size)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(handle = NULL)
  {
    return ERROR;
  }
  int byteswritten;

  return byteswritten;
}


/*-----------------------------------------------------------------------------


Função: Função usada para truncar um arquivo.
  Remove do arquivo todos os bytes a partir da posição atual do contador de posição (current pointer)
  Todos os bytes desde a posição indicada pelo current pointer até o final do arquivo são removidos do arquivo.

Entra:  handle -> identificador do arquivo a ser truncado

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int truncate2 (FILE2 handle)
{

  return SUCCESS;
}


/*-----------------------------------------------------------------------------
Função: Reposiciona o contador de posições (current pointer) do arquivo identificado por "handle".
  A nova posição é determinada pelo parâmetro "offset".
  O parâmetro "offset" corresponde ao deslocamento, em bytes, contados a partir do início do arquivo.
  Se o valor de "offset" for "-1", o current_pointer deverá ser posicionado no byte seguinte ao final do arquivo,
    Isso é útil para permitir que novos dados sejam adicionados no final de um arquivo já existente.

Entra:  handle -> identificador do arquivo a ser escrito
  offset -> deslocamento, em bytes, onde posicionar o "current pointer".

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int seek2 (FILE2 handle, DWORD offset)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(handle = NULL)
  {
    return ERROR;
  }

  return SUCCESS;
}


/*-----------------------------------------------------------------------------
Função: Criar um novo diretório.
  O caminho desse novo diretório é aquele informado pelo parâmetro "pathname".
    O caminho pode ser ser absoluto ou relativo.
  A criação de um novo subdiretório deve ser acompanhada pela criação, automática, das entradas "." e ".."
  A entrada "." corresponde ao descritor do subdiretório recém criado
  A entrada ".." corresponde à entrada de seu diretório pai.
  São considerados erros de criação quaisquer situações em que o diretório não possa ser criado.
    Isso inclui a existência de um arquivo ou diretório com o mesmo "pathname".

Entra:  pathname -> caminho do diretório a ser criado

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int mkdir2 (char *pathname)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(pathname == NULL)
  {
    return ERROR;
  }

  return SUCCESS;
}


/*-----------------------------------------------------------------------------
Função: Apagar um subdiretório do disco.
  O caminho do diretório a ser apagado é aquele informado pelo parâmetro "pathname".
  São considerados erros quaisquer situações que impeçam a operação.
    Isso inclui:
      (a) o diretório a ser removido não está vazio;
      (b) "pathname" não existente;
      (c) algum dos componentes do "pathname" não existe (caminho inválido);
      (d) o "pathname" indicado não é um arquivo;
      (e) o "pathname" indica os diretórios "." ou "..".

Entra:  pathname -> caminho do diretório a ser criado

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int rmdir2 (char *pathname)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(pathname == NULL)
  {
    return ERROR;
  }

  return SUCCESS;
}


/*-----------------------------------------------------------------------------
Função: Abre um diretório existente no disco.
  O caminho desse diretório é aquele informado pelo parâmetro "pathname".
  Se a operação foi realizada com sucesso, a função:
    (a) deve retornar o identificador (handle) do diretório
    (b) deve posicionar o ponteiro de entradas (current entry) na primeira posição válida do diretório "pathname".
  O handle retornado será usado em chamadas posteriores do sistema de arquivo para fins de manipulação do diretório.

Entra:  pathname -> caminho do diretório a ser aberto

Saída:  Se a operação foi realizada com sucesso, a função retorna o identificador do diretório (handle).
  Em caso de erro, será retornado um valor negativo.
-----------------------------------------------------------------------------*/
DIR2 opendir2 (char *pathname)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(pathname == NULL)
  {
    return ERROR;
  }

  int handle;

  return handle;
}


/*-----------------------------------------------------------------------------
Função: Realiza a leitura das entradas do diretório identificado por "handle".
  A cada chamada da função é lida a entrada seguinte do diretório representado pelo identificador "handle".
  Algumas das informações dessas entradas devem ser colocadas no parâmetro "dentry".
  Após realizada a leitura de uma entrada, o ponteiro de entradas (current entry) deve ser ajustado para a próxima entrada válida, seguinte à última lida.
  São considerados erros:
    (a) qualquer situação que impeça a realização da operação
    (b) término das entradas válidas do diretório identificado por "handle".

Entra:  handle -> identificador do diretório cujas entradas deseja-se ler.
  dentry -> estrutura de dados onde a função coloca as informações da entrada lida.

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero ( e "dentry" não será válido)
-----------------------------------------------------------------------------*/
int readdir2 (DIR2 handle, DIRENT2 *dentry)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(handle = NULL)
  {
    setInvalid(dentry);
    return ERROR;
  }

  return SUCCESS;
}


/*-----------------------------------------------------------------------------
Função: Fecha o diretório identificado pelo parâmetro "handle".

Entra:  handle -> identificador do diretório que se deseja fechar (encerrar a operação).

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle)
{
  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(handle = NULL)
  {
    return ERROR;
  }

  return SUCCESS;
}