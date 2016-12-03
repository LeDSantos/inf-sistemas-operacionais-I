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
// #include "../include/functions.h"
#include "../include/apidisk.h"
#include "../include/bitmap2.h"

#define GROUP "Gustavo Madeira Santana - 252853, Cristiano Salla Lunardi - 240508"

#define SUCCESS 0
#define ERROR -1

#define ID "T2FS"
#define VERSION 0x7E02
#define DISK "t2fs_disk.dat"

// #define SECTOR_COUNT 32768
#define BLOCK_SIZE 16*SECTOR_SIZE
// #define BITMAP_COUNT 2048 //(superblock->diskSize / superblock->blockSize), 32768 div 16 = 2048
#define MAX_FILESIZE 352 //superblock->blockSize * (( ((superblock->blockSize)*(superblock->blockSize))/16) + ((superblock->blockSize)/4) + 2)
#define MAX_OPENFILES 20

// setor onde começa cada area
#define superblock_sector 0
#define block_bitmap 1
#define inode_bitmap 2
#define inode_area 3
#define data_area 128

typedef struct t2fs_superbloco SB_t;
typedef struct t2fs_record REC_t;
typedef struct t2fs_inode INO_t;

/*
** struct para armazenar estrutura de diretorios
** facilita para percorrer diretorios
** root não tem pai e tem sub como filho
** outros diretorios em root serao irmaos de sub e terao root como pai
*/
typedef struct directory_struct DIR_t;
struct directory_struct {
  char name[32];
  DIR_t*  pai;
  int     paiblock;
  DIR_t*  filho;
  DIR_t*  irmao;
  REC_t*  record;
};

/*
** struct para armazenar informacoes dos arquivos abertos
**  files_open: numero de arquivos abertos;
**  inode: inode do arquivo atual
**  nextfile: proximo arquivo na lista
*/
typedef struct open_files_struct OPEN_t;
struct open_files_struct {
  int     filesopen;
  int     inode;
  char    name[32];
  OPEN_t* nextfile;
};

int path_exists(char* filename);
int path_parser(char* path, char* pathfound);
int get_file_inode(char *filename);
int write_block(int sector);
int update_open_files(int inode_number);
int read_block(int sector);
void debug_buffer_disk(int area, int diskarea, int bloco);
int create_inode_write_to_disk(int freeblock, int freeinode);
int create_record_write_to_disk(int freeblock, int freeinode, char* filename);
int find_record_in_blockbuffer(REC_t* auxrecord, BYTE type, char* filename);
int get_block_for_dir();

static int disk_initialized = 0;
unsigned char buffer[SECTOR_SIZE];
unsigned char blockbuffer[16*SECTOR_SIZE];
SB_t* superblock;
INO_t* current_inode;
REC_t* current_record;
DIR_t* current_dir;
DIR_t root;
OPEN_t* current_file;
OPEN_t open_files;

/*
** inicializa t2fs
** criacao do superbloco
*/

void disk_info()
{
  printf("\n\n[disk_info] \n>> T2FS inicializando...\n");
  printf("ID: %s\n", superblock->id);
  printf("Versao: 0x%X\n", superblock->version);
  printf("Informacoes do disco:\n");
  printf("Tamanho super bloco: %hu setores\n", superblock->superblockSize);
  printf("Tamanho bitmap blocos livres: %hu setores\n", superblock->freeBlocksBitmapSize);
  printf("Tamanho bitmap inodes livres: %hu setores\n", superblock->freeInodeBitmapSize);
  printf("Tamanho area inodes: %hu setores\n", superblock->inodeAreaSize);
  printf("Tamanho bloco logico: %hu setores\n", superblock->blockSize);
  printf("Tamanho area dados: %u setores\n\n", superblock->diskSize);
  disk_initialized = 1;
}

void disk_init()
{
  superblock = malloc(sizeof(SB_t));

  if (read_sector(0, buffer) != 0)
  {
    printf("[disk_info] erro ao ler setor do super bloco\n");
    exit(ERROR);
  }

  strncpy(superblock->id, buffer, 4);
  if (strncmp(superblock->id, ID, 4) != 0)
  {
    printf("[disk_info] ID no disco nao reonhecida\n");
    exit(ERROR);
  }

  superblock->version = *((WORD *)(buffer + 4));
  if (superblock->version != VERSION)
  {
    printf("[disk_info] versao no disco nao reonhecida\n");
    exit(ERROR);
  }

  superblock->superblockSize = *((WORD *)(buffer + 6));
  superblock->freeBlocksBitmapSize = *((WORD *)(buffer + 8));
  superblock->freeInodeBitmapSize = *((WORD *)(buffer + 10));
  superblock->inodeAreaSize = *((WORD *)(buffer + 12));
  superblock->blockSize = *((WORD *)(buffer + 14));
  superblock->diskSize = *((DWORD *)(buffer + 16));

  disk_info();

  open_files.filesopen = 0;
  open_files.inode = -1;
  open_files.nextfile = -1;

  // verificar o que já existe no disco
  // inicializar estruturas auxiliares para percorrer diretorios
  current_dir = malloc(sizeof(DIR_t));

  char auxname[] = "root";
  strncpy(root.name, auxname, 4);
  root.pai = NULL;
  root.paiblock = 0;
  root.irmao = NULL;
  root.filho = NULL;

  REC_t *rootrecord = malloc(16*sizeof(int));
  rootrecord->TypeVal = 0x02;
  strncpy(rootrecord->name, auxname, 4);
  rootrecord->blocksFileSize = 1;
  rootrecord->bytesFileSize = -1;
  rootrecord->inodeNumber = 0;
  root.record = rootrecord;

  DIR_t* auxdir = malloc(sizeof(DIR_t));
  auxdir->pai = NULL;
  auxdir->irmao = NULL;
  auxdir->filho = NULL;
  auxdir->record = NULL;

  if(read_sector(data_area, buffer) != 0 )
  {
    printf("[disk_info] erro ao ler setor de dados: 0\n");
  }

  current_record = malloc(16*sizeof(int));
  current_record = NULL;

  printf("[disk_info] diretorios no disco:\n\n");

  if (read_block(data_area) != 0)
  {
    printf("[disk_info] falha ao ler bloco de dados da raiz\n");
    return ERROR;
  }

  current_record = NULL;
  int iterator = 0;       //um setor pode ter no maximo 4 records (64 * 4 = 256)
  while (iterator < 64)
  {
    REC_t* auxrecord = malloc(16*sizeof(int));
    auxrecord->TypeVal = *((BYTE *)(blockbuffer + iterator*64));
    strncpy(auxrecord->name, blockbuffer + iterator*64 + 1, 32);
    auxrecord->blocksFileSize = *((DWORD *)(blockbuffer + iterator*64 + 33));
    auxrecord->bytesFileSize = *((DWORD *)(blockbuffer + iterator*64 + 37));
    auxrecord->inodeNumber = *((int *)(blockbuffer + iterator*64 + 41));
    // adicionar diretorios na estrutura para percorrer diretorios
    if (auxrecord->TypeVal == 0x02)
    {
      printf("nome: %s\n", auxrecord->name);
      printf("tipo: %x                >>|1: arquivo, 2: dir, 3: invalido|\n", auxrecord->TypeVal);
      printf("blocks file size: %u\n", auxrecord->blocksFileSize);
      printf("bytes file size: %u\n", auxrecord->bytesFileSize);
      printf("inode number: %d\n\n", auxrecord->inodeNumber);

      auxdir->pai = &root;
      auxdir->record = auxrecord;
      strncpy(auxdir->name, auxrecord->name, 32);
      root.filho = auxdir;

      //TODO
      // PEGAR BLOCO APONTADO PELO INODE DO DIR E COLOCAR EM DIR_t

      //TODO
      // SE TEM DIRETORIO, PERCORRER ESSE DIRETORIO E ACHAR SUBDIRETORIOS EXISTENTES
      // GARANTIR QUE TESTES DIFERENTES EM SEQUENCIA FUNCIONARAO

    } else free(auxrecord);

    ++iterator;
  }

  current_dir = &root;
  printf("[disk_info] diretorio %s: '/'\n", current_dir->name);

  current_dir = root.filho;
  printf("[disk_info] subdiretorios na raiz: \n");
  do
  {
    // printf("%s\n\n", current_dir->name);
    current_dir = current_dir->irmao;
  } while(current_dir);

  printf("[disk_info] bitmaps ocupados:\n");
  int ii = 0;
  int bmp;
  for (ii = 0; ii < 2048; ++ii)
  {
     bmp = getBitmap2(BITMAP_INODE, ii);
     if (bmp == 1) printf("inode %d esta ocupado\n", ii);
     bmp = getBitmap2(BITMAP_DADOS, ii);
     if (bmp == 1) printf("bloco %d esta ocupado\n", ii);
  }

  printf("\n\n[disk_info] >> T2FS inicializado!\n\n");
  disk_initialized = 1;
  current_dir = &root;
}

int identify2 (char *name, int size)
{
  printf("\nGustavo Madeira Santana - 252853\nCristiano Salla Lunardi - 240508\n\n");

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
  //Criar um arquivo:
  //bitmap blocos livres

  if (disk_initialized == 0)
  {
    disk_init();
  }

  if (strlen(filename) > 32)
  {
    printf("[create2] limite de 32 caracteres para nome de arquivo, tente novamente\n");
    return ERROR;
  }

  int freeinode = searchBitmap2(BITMAP_INODE, 0);
  if (freeinode < 0)
  {
    printf("[create2]nao existe inode disponivel, apague algum arquivo antes de criar outro\n");
    return ERROR;
  }

  int freeblock = searchBitmap2(BITMAP_DADOS, 0);
  if(freeblock < 0)
  {
    printf("[create2] disco cheio, apague algum arquivo e tenta novamente\n");
    return ERROR;
  }

  printf("[create2] inode livre identificado: %d\n", freeinode);
  printf("[create2] bloco livre identificado: %d\n", freeblock);

  // printf("***********\n");
  // debug_buffer_disk(0,0,0);
  // printf("***********\n");
  if (create_inode_write_to_disk(freeblock, freeinode) != 0)
  {
    printf("[create2] falha ao escrever inode no disco\n");
    return ERROR;
  }
  if (setBitmap2 (BITMAP_INODE, freeinode, 1) < 0)
  {
    printf("[create2] falaha o setar bitmap inode\n");
    return ERROR;
  }

  printf("***********\n");
  debug_buffer_disk(0,0,0);
  printf("***********\n");


  // printf("\n\n********\n");
  // debug_buffer_disk(0,1,current_dir->paiblock);
  // printf("\n\n********\n");

  if (create_record_write_to_disk(freeblock, freeinode, filename) != 0)
  {
    printf("[create2] falha ao criar record\n");
    return ERROR;
  }

  if (setBitmap2 (BITMAP_INODE, freeblock, 1) < 0)
  {
    printf("[create2] falha o setar bitmap inode\n");
    return ERROR;
  }

  printf("\n\n********\n");
  debug_buffer_disk(0, 1, current_dir->paiblock);
  printf("\n\n********\n");

  return freeinode;
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

  // handle recebe posicão do vetor open_files que tem inode do arquivo a ser aberto
  int handle = path_exists(filename);
  if(handle == -1)
  {
    printf("[open2] %s nao eh um arquivo valido\n", filename);
    return ERROR;
  } else if (handle == -2)
    {
      printf("[open2] \"%s\" eh um diretorio e nao um arquivo\ndigite o caminho de um arquivo e tente novamente\n", filename);
      return ERROR;
    } else if (handle == -3)
      {
        printf("[open2] %s nao eh um caminho valido\n", filename);
      }
  printf("[open2] arquivos abertos: %d\n", open_files.filesopen);
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


int path_exists(char* filename)
{
  if(!filename)
  {
    return ERROR;
  }

  char *path[25];
  int dirs = path_parser(filename, &path);

  printf("[path_exists] possiveis diretorios: %d  ", dirs);
  int i;
  for (i = 0; i < dirs; ++i)
  {
    printf(">> %s  ", path[i]);
  }

  i = 0;
  int encontrou = 0;
  current_dir = root.filho;
  // printf("Comecando em %s\n", current_dir->name);

  DIR_t* found;

  char* searching_for1;
  char* searching_for1_father;
  char* searching_for2;

  int iterator;
  for (iterator = 0; iterator < dirs; ++iterator)
  {
    do
    {
      searching_for1 = path[i];
      searching_for1_father = current_dir->pai->name;
      searching_for2 = current_dir->name;
      found = current_dir->pai;
      printf("\n[path_exists] procurando por: %s dentro de %s ...\n", searching_for1, searching_for1_father);
      // printf("%s %s\n", searching_for1, searching_for2);
      if (strcmp(searching_for1, searching_for2) == 0)
      {
        encontrou = 1;
        break;
      }
      current_dir = current_dir->irmao;
      // printf("%s\n", current_dir->name);
    } while(current_dir);

    if (encontrou == 0)
    {
      printf("[path_exists] %s nao e um diretorio\n", path[i]);
      if (!current_dir)
      {
        current_dir = found;
      }

      if (i+1 < dirs)
      {
        return ERROR;
      }

      int foundinode = get_file_inode(path[i]);
      return foundinode;

    } else if (iterator < dirs) // current_dir->filho == NULL &&
      {
        printf("[path_exists] \"%s\" encontrado!\n", searching_for1);
        if (!current_dir->filho)
        {
            printf("e nao tem subdiretorios\n");
          if (!current_dir)
          {
            current_dir = found;
          }
          if (i+1 == dirs)
          {
            return -2;
          } else if ((i+2) <= dirs)
            {
              return -3;
            }
          char* newpath = path[i+1];
          int foundinode = get_file_inode(newpath);
          return foundinode;
        }
      }

    encontrou = 0;
    i++;
    current_dir = current_dir->filho;
  }

  return ERROR;
}

int path_parser(char* path, char* pathfound)
{
  char* token;
  char aux[2048];
  strcpy(aux, path);

  printf ("[path_parser] caminho: \"%s\"\n", path);

  int char_size = 1;
  token = strtok (aux, "/");
  while (token != NULL)
  {
    token = strtok (NULL, "/");
    ++char_size;
  }

  const char *paths[char_size];
  strcpy(aux, path);

  char_size = 0;
  token = strtok (aux, "/");
  while (token != NULL)
  {
    // printf("token: %s\n", token);
    paths[char_size] = token;
    ++char_size;
    token = strtok (NULL, "/");
  }

  strcpy(pathfound, paths);

  return char_size;
}

int get_file_inode(char *filename)
{
  // pegando o numero do inode do diretorio atual
  // if (current_dir)
  // {
  //   current_dir = current_dir->pai;
  // }
  printf("[get_file_inode] procurando por \"%s\" como arquivo em %s\n", filename, current_dir->name);
  int dir_inode = current_dir->record->inodeNumber;
  int sector_to_read;
  div_t output = div(dir_inode, 256);
  sector_to_read = inode_area + output.quot;

  printf("[get_file_inode] inode area sector: %d\n", inode_area);
  printf("[get_file_inode] sector to read: %d\n", sector_to_read);
  printf("[get_file_inode] dir inode number: %d\n", dir_inode);

  // lendo inode do diretorio atual
  read_sector(sector_to_read, buffer);

  INO_t* inode_aux = malloc(sizeof(INO_t));
  inode_aux->dataPtr[0] = *((int *)(buffer + dir_inode*16));
  inode_aux->dataPtr[1] = *((int *)(buffer + dir_inode*16 + 4));
  inode_aux->singleIndPtr = *((int *)(buffer + dir_inode*16 + 8));
  inode_aux->doubleIndPtr = *((int *)(buffer + dir_inode*16 + 12));
  current_inode = inode_aux;
  printf("[get_file_inode] pointers: %d %d %d %d\n", inode_aux->dataPtr[0], inode_aux->dataPtr[1], inode_aux->singleIndPtr, inode_aux->doubleIndPtr);

  // ler bloco de dados do diretorio atual para procurar arquivo

  sector_to_read = data_area + inode_aux->dataPtr[0];
  // printf("%d\n", sector_to_read);

  read_block(sector_to_read);

  current_record = NULL;
  REC_t* auxrecord = malloc(16*sizeof(int));
  int iterator = 0;       //um bloco pode ter no maximo 64 records (64 * 4 * 16 = 4096)
  while (iterator < 256)
  {

    auxrecord->TypeVal = *((BYTE *)(blockbuffer + iterator*64));
    strncpy(auxrecord->name, blockbuffer + iterator*64 + 1, 32);
    auxrecord->blocksFileSize = *((DWORD *)(blockbuffer + iterator*64 + 33));
    auxrecord->bytesFileSize = *((DWORD *)(blockbuffer + iterator*64 + 37));
    auxrecord->inodeNumber = *((int *)(blockbuffer + iterator*64 + 41));

    if (auxrecord->TypeVal == 0x01)
    {
      printf("[get_file_inode] record encontrado:\n");
      printf("nome: %s\n", auxrecord->name);
      printf("tipo: %x                >>|1: arquivo, 2: dir, 3: invalido|\n", auxrecord->TypeVal);
      printf("blocks file size: %u\n", auxrecord->blocksFileSize);
      printf("bytes file size: %u\n", auxrecord->bytesFileSize);
      printf("inode number: %d\n\n", auxrecord->inodeNumber);

      if(strcmp(filename, auxrecord->name) == 0)
      {
        current_record = auxrecord;
        break;
      }
    }
    ++iterator;
    // printf("%d\n", iterator);
  }
  if (current_record == NULL)
  {
    printf("[get_file_inode] arquivo %s não existe\n", filename);
    return ERROR;
  }
  printf("[get_file_inode] arquivo %s encontrado!\n", filename);

  int handle = update_open_files(current_record->inodeNumber);
  if(handle < 0)
    return ERROR;

  return handle;
}


int update_open_files(int inode_number)
{
  int iterator = open_files.filesopen;

  if (iterator == MAX_OPENFILES)
  {
    printf("[update_open_files] erro: numero maximo de arquivos abertos atingido: %d\n", MAX_OPENFILES);
    return ERROR;
  }

  current_file = &open_files;

  int i = 0;
  if (iterator == 0)
  {
    open_files.inode = inode_number;
    open_files.nextfile = -1;
    open_files.filesopen++;
  } else {
    for (i = 0; i < iterator; ++i)
    {
      current_file = current_file->nextfile;
    }
    OPEN_t* aux_file = malloc(sizeof(OPEN_t));

    current_file->nextfile = aux_file;
    open_files.filesopen++;
    aux_file->inode = inode_number;
    strcpy(aux_file->name, current_record->name);
    aux_file->nextfile = -1;
  }

  printf("[update_open_files] arquivo aberto tem inode: %d \n", current_file->inode);
  printf("[update_open_files] arquivos abertos: %d\n", open_files.filesopen);

  return i;
}

int read_block(int sector)
{
  // printf("lendo setor %d\n", sector);
  int i;
  int x = 0;
  for (i = 0; i < 16; ++i)
  {
    if(read_sector(sector + i, buffer) != 0)
        return ERROR;
    memcpy((blockbuffer + x), buffer, sizeof(buffer));
    x = x + 256;
  }

  return SUCCESS;
}

int write_block(int sector)
{
  // printf("escrevendo no setor %d\n", sector);
  int i;
  int x = 0;
  for (i = 0; i < 16; ++i)
  {
    memcpy(buffer, (blockbuffer + x), sizeof(buffer));
    if(write_sector(sector + i, buffer) != 0)
        return ERROR;
    x = x + 256;
  }

  return SUCCESS;
}

// Area: 0 = disco; 1 = blockbuffer
// Disk: 0 = inode; 1 = data area
void debug_buffer_disk(int area, int diskarea, int bloco)
{
  if (area == 0)
  {
    if (diskarea == 0)
    {
      printf("DEBUG INODE DISCO\n");
      read_block(inode_area);

      INO_t newinode;

      int x = 0;
      while(x < 256)
      {
      newinode.dataPtr[0] = *((int *)(blockbuffer + x*16));
      newinode.dataPtr[1] = *((int *)(blockbuffer + x*16 + 4));
      newinode.singleIndPtr = *((int *)(blockbuffer + x*16 + 8));
      newinode.doubleIndPtr = *((int *)(blockbuffer + x*16 + 12));

      printf("%d %d %d %d\n", newinode.dataPtr[0], newinode.dataPtr[1], newinode.singleIndPtr, newinode.doubleIndPtr);
      ++x;
      }
    } else if (diskarea == 1)
      {
        printf("DEBUG DATA DISCO\n");
        read_block(data_area + bloco * 16);

        REC_t newrecord;

        int x = 0;
        while(x < 64)
        {
          newrecord.TypeVal = *((BYTE *)(blockbuffer + x*64));
          strncpy(newrecord.name, blockbuffer + x*64 + 1, 32);
          newrecord.blocksFileSize = *((DWORD *)(blockbuffer + x*64 + 33));
          newrecord.bytesFileSize = *((DWORD *)(blockbuffer + x*64 + 37));
          newrecord.inodeNumber = *((int *)(blockbuffer + x*64 + 41));

          printf("%d %s %d %d %d\n", newrecord.TypeVal, newrecord.name, newrecord.blocksFileSize, newrecord.bytesFileSize, newrecord.inodeNumber);
          ++x;
        }
      }
  } else if (area == 1)
  {
    if (diskarea == 0)
    {
      printf("DEBUG INODE BUFFER\n");
      INO_t newinode;

      int x = 0;
      while(x < 256)
      {
      newinode.dataPtr[0] = *((int *)(blockbuffer + x*16));
      newinode.dataPtr[1] = *((int *)(blockbuffer + x*16 + 4));
      newinode.singleIndPtr = *((int *)(blockbuffer + x*16 + 8));
      newinode.doubleIndPtr = *((int *)(blockbuffer + x*16 + 12));

      printf("%d %d %d %d\n", newinode.dataPtr[0], newinode.dataPtr[1], newinode.singleIndPtr, newinode.doubleIndPtr);
      ++x;
      }
    } else if (diskarea == 1)
      {
        printf("DEBUG DATA BUFFER\n");

        REC_t newrecord;

        int x = 0;
        while(x < 64)
        {
          newrecord.TypeVal = *((BYTE *)(blockbuffer + x*64));
          strncpy(newrecord.name, blockbuffer + x*64 + 1, 32);
          newrecord.blocksFileSize = *((DWORD *)(blockbuffer + x*64 + 33));
          newrecord.bytesFileSize = *((DWORD *)(blockbuffer + x*64 + 37));
          newrecord.inodeNumber = *((int *)(blockbuffer + x*64 + 41));

          printf("%d %s %d %d %d\n", newrecord.TypeVal, newrecord.name, newrecord.blocksFileSize, newrecord.bytesFileSize, newrecord.inodeNumber);
          ++x;
        }
      }
  }
}


int create_inode_write_to_disk(int freeblock, int freeinode)
{
  int offset= freeinode*16;
  read_block(inode_area);

  INO_t newinode;

  // newinode.dataPtr[0] = *((int *)(blockbuffer + offset));
  // newinode.dataPtr[1] = *((int *)(blockbuffer + offset + 4));
  // newinode.singleIndPtr = *((int *)(blockbuffer + offset + 8));
  // newinode.doubleIndPtr = *((int *)(blockbuffer + offset + 12));

  // printf("%d %d %d %d\n", newinode.dataPtr[0], newinode.dataPtr[1], newinode.singleIndPtr, newinode.doubleIndPtr);

  newinode.dataPtr[0] = freeblock;
  newinode.dataPtr[1] = INVALID_PTR;
  newinode.singleIndPtr = INVALID_PTR;
  newinode.doubleIndPtr = INVALID_PTR;
  memcpy((blockbuffer + offset), &newinode.dataPtr[0], sizeof(int));
  memcpy((blockbuffer + offset + 4), &newinode.dataPtr[1], sizeof(int));
  memcpy((blockbuffer + offset + 8), &newinode.singleIndPtr, sizeof(int));
  memcpy((blockbuffer + offset + 12), &newinode.doubleIndPtr, sizeof(int));

  newinode.dataPtr[0] = *((int *)(blockbuffer + offset));
  newinode.dataPtr[1] = *((int *)(blockbuffer + offset + 4));
  newinode.singleIndPtr = *((int *)(blockbuffer + offset + 8));
  newinode.doubleIndPtr = *((int *)(blockbuffer + offset + 12));

  if (newinode.dataPtr[0] != freeblock)
  {
    printf("erro ao escrever inode no buffer\n");
  }

  printf("%d %d %d %d\n", newinode.dataPtr[0], newinode.dataPtr[1], newinode.singleIndPtr, newinode.doubleIndPtr);

  // debug_buffer_disk(1,0,0);

  if (write_block(inode_area) != 0)
      return ERROR;

  return SUCCESS;
}

int create_record_write_to_disk(int freeblock, int freeinode, char* filename)
{
  int current_block = current_dir->paiblock;
  // printf("%d\n", current_block);

  if (read_block(data_area + current_block * 16) != 0)
  {
    printf("[create_record_write_to_disk] erro ao ler bloco em create_record_write_to_disk\n");
    return ERROR;
  }

  REC_t* auxrecord = malloc(16*sizeof(int));

  int offset = find_record_in_blockbuffer(auxrecord, 0x00, filename);
  if (offset == -1)
  {
    printf("[create_record_write_to_disk] nenhum record livre disponivel no bloco\n");
    return ERROR;
  } else if (offset == -2)
  {
    return ERROR;
  }

  auxrecord->TypeVal = 0x01;
  strcpy(auxrecord->name, filename);
  auxrecord->blocksFileSize = 1;
  auxrecord->bytesFileSize = 0;
  auxrecord->inodeNumber = freeinode;

  memcpy((blockbuffer + offset), &auxrecord->TypeVal, sizeof(BYTE));
  memcpy((blockbuffer + offset + 1), &auxrecord->name, 32*sizeof(char));
  memcpy((blockbuffer + offset + 33), &auxrecord->blocksFileSize, sizeof(DWORD));
  memcpy((blockbuffer + offset + 37), &auxrecord->bytesFileSize, sizeof(DWORD));
  memcpy((blockbuffer + offset + 41), &auxrecord->inodeNumber, sizeof(int));

  // printf("\n\n********\n");
  // debug_buffer_disk(1,1,0);
  // printf("\n\n********\n");

  // auxrecord->TypeVal = *((BYTE *)(blockbuffer + offset));
  // strncpy(auxrecord->name, blockbuffer + offset + 1, 32);
  // auxrecord->blocksFileSize = *((DWORD *)(blockbuffer + offset + 33));
  // auxrecord->bytesFileSize = *((DWORD *)(blockbuffer + offset + 37));
  // auxrecord->inodeNumber = *((int *)(blockbuffer + offset + 41));

  printf("[create_record_write_to_disk] inode number: %d\n\n", auxrecord->inodeNumber);
  printf("[create_record_write_to_disk] Record a ser criado:\n");
  printf("nome: %s\n", auxrecord->name);
  printf("tipo: %x                >>|1: arquivo, 2: dir, 3: invalido|\n", auxrecord->TypeVal);
  printf("blocks file size: %u\n", auxrecord->blocksFileSize);
  printf("bytes file size: %u\n", auxrecord->bytesFileSize);
  printf("inode number: %d\n\n", auxrecord->inodeNumber);

  if(write_block(data_area + current_block) != 0)
    return ERROR;

  free(auxrecord);
  return SUCCESS;
}

int find_record_in_blockbuffer(REC_t* auxrecord, BYTE type, char* filename)
{
  int iterator = 0;
  while (iterator < 64)
  {

    auxrecord->TypeVal = *((BYTE *)(blockbuffer + iterator*64));
    strncpy(auxrecord->name, blockbuffer + iterator*64 + 1, 32);
    auxrecord->blocksFileSize = *((DWORD *)(blockbuffer + iterator*64 + 33));
    auxrecord->bytesFileSize = *((DWORD *)(blockbuffer + iterator*64 + 37));
    auxrecord->inodeNumber = *((int *)(blockbuffer + iterator*64 + 41));

    if (auxrecord->TypeVal != 0x00 && (strcmp(filename, auxrecord->name) == 0))
    {
      printf("[find_record_in_blockbuffer] %s ja existe, tente novamente com outro nome\n", filename);
      // printf("nome: %s\n", auxrecord->name);
      // printf("tipo: %x                >>|1: arquivo, 2: dir, 3: invalido|\n", auxrecord->TypeVal);
      // printf("blocks file size: %u\n", auxrecord->blocksFileSize);
      // printf("bytes file size: %u\n", auxrecord->bytesFileSize);
      // printf("inode number: %d\n\n", auxrecord->inodeNumber);

      return -2;
    }

    if (auxrecord->TypeVal == type)
    {
      printf("[find_record_in_blockbuffer] record encontrado!\n");
      return iterator*64;
    }
    ++iterator;
    // printf("%d\n", iterator);
  }
  return ERROR;
}

int get_block_for_dir()
{
}