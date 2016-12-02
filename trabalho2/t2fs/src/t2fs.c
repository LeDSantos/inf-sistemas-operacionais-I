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
  printf("\n\n>> T2FS inicializando...\n");
  printf("ID: %s\n", superblock->id);
  printf("Versao: 0x%x\n", superblock->version);
  printf("Informacoes do disco (dados em numero de setores):\n");
  printf("Tamanho super bloco: %hu\n", superblock->superblockSize);
  printf("Tamanho bitmap blocos livres: %hu\n", superblock->freeBlocksBitmapSize);
  printf("Tamanho bitmap inodes livres: %hu\n", superblock->freeInodeBitmapSize);
  printf("Tamanho area inodes: %hu\n", superblock->inodeAreaSize);
  printf("Tamanho bloco logico: %hu\n", superblock->blockSize);
  printf("Tamanho area dados: %u\n\n", superblock->diskSize);
}

void disk_init()
{
  superblock = malloc(sizeof(SB_t));

  if (read_sector(0, buffer) != 0)
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

  open_files.filesopen = 0;
  open_files.inode = -1;
  open_files.nextfile = -1;

  // verificar o que já existe no disco
  // inicializar estruturas auxiliares para percorrer diretorios
  current_dir = malloc(sizeof(DIR_t));

  char auxname[] = "root";
  strncpy(root.name, auxname, 4);
  root.pai = NULL;
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
    printf("Erro ao ler setor de dados: 0\n");
  }

  current_record = malloc(16*sizeof(int));
  current_record = NULL;

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

    REC_t* auxrecord = malloc(16*sizeof(int));
    auxrecord->TypeVal = *((BYTE *)(buffer + iterator*64));
    strncpy(auxrecord->name, buffer + iterator*64 + 1, 32);
    auxrecord->blocksFileSize = *((DWORD *)(buffer + iterator*64 + 33));
    auxrecord->bytesFileSize = *((DWORD *)(buffer + iterator*64 + 37));
    auxrecord->inodeNumber = *((int *)(buffer + iterator*64 + 41));

    current_record = auxrecord;
    if (auxrecord->TypeVal == 0x00)
    {
      break;
    }

    printf("nome: %s\n", auxrecord->name);
    printf("tipo: %x                >>|1: arquivo, 2: dir, 3: invalido|\n", auxrecord->TypeVal);
    printf("blocks file size: %u\n", auxrecord->blocksFileSize);
    printf("bytes file size: %u\n", auxrecord->bytesFileSize);
    printf("inode number: %d\n\n", auxrecord->inodeNumber);

    // adicionar diretorios na estrutura para percorrer diretorios
    if (auxrecord->TypeVal == 0x02)
    {
      auxdir->pai = &root;
      auxdir->record = auxrecord;
      strncpy(auxdir->name, auxrecord->name, 32);
      root.filho = auxdir;
    }

    ++iterator;
  }

  current_dir = &root;
  printf("Diretorio raiz: %s : '/'\n", current_dir->name);

  current_dir = root.filho;
  printf("Subdiretorios na raiz: \n");
  do
  {
    printf("%s\n\n", current_dir->name);
    current_dir = current_dir->irmao;
  } while(current_dir);

  printf("Bitmaps ocupados:\n");
  int ii = 0;
  int bmp;
  for (ii = 0; ii < 2048; ++ii)
  {
     bmp = getBitmap2(BITMAP_INODE, ii);
     if (bmp == 1) printf("inode %d esta ocupado\n", ii);
     bmp = getBitmap2(BITMAP_DADOS, ii);
     if (bmp == 1) printf("bloco %d esta ocupado\n", ii);
  }

  printf("\n\n>> T2FS inicializado!\n\n");
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
  /*
FILE2 create2 (char *filename)
{
  Criar um arquivo:
  bitmap blocos livres


  if(disk_initialized == 0)
  {
    disk_init();
  }

  if(filename = NULL)
  {
    return ERROR;
  }

  //procura uma posicao fazia no setor dos inodes.
  int numberInode = searchBitmap2(BITMAP_INODE, 0);
  if(numberInode < 0)
  {
    return ERROR;
  }
  //seta a posicao no bitmap do inode ocupado.
  setBitmap2 (BITMAP_INODE, numberInode, 1);

  REC_t recT;

  recT.TypeVal = 0x01; //indica que é arquivo regular
  recT.name = filename; //nome do arquivo
  recT.blocksFileSize = 1; // Tamanho do arquivo, expresso em número de blocos de dados
  recT.bytesFileSize = 0;  // Tamanho do arquivo. Expresso em número de bytes

  INO_t inoT;

  recT.inodeNumber = numberInode;

  procura um bloco de dados livre para o inode apontar
  int dataBlock = searchBitmap2(BITMAP_DADOS, 0);
  if(dataBlock < 0)
  {
    return ERROR;
  }
  //coloca a posicao do bloco de dados como ocupada
  setBitmap2(BITMAP_DADOS, dataBlock, 1);

  inoT.dataPtr[0] = dataBlock*16;
  inoT.dataPtr[1] = 0x00;
  inoT.singleIndPtr = 0x00;
  inT.doubleIndPtr = 0x00;

  //FALTAAA
  // i-node escrever na área de i-node
  // record no bloco de diretorio corrente
  return numberInode;
}
*/
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
  printf("handle: %d\narquivos abertos: %d\n", handle, open_files.filesopen);
  if(handle < 0)
  {
    //não existe diretorio ou arquivo
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


int path_exists(char* filename)
{
  if(!filename)
  {
    return ERROR;
  }

  char *path[25];
  int dirs = path_parser(filename, &path);

  printf("possiveis diretorios: %d   ", dirs);
  int i;
  for (i = 0; i < dirs; ++i)
  {
    printf(">> %s  ", path[i]);
  }

  i = 0;
  int encontrou = 0;
  current_dir = root.filho;

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
      printf("\nprocurando por: %s dentro de %s ...\n", searching_for1, searching_for1_father);
      if (strcmp(searching_for1, searching_for2) == 0)
      {
        encontrou = 1;
        break;
      }
      current_dir = current_dir->irmao;
    } while(current_dir);

    if (encontrou == 0)
    {
      printf("%s nao e um diretorio\n", path[i]);
      current_dir = found;
      int foundinode = get_file_inode(path[i]);
      return foundinode;
    } else if (current_dir->filho == NULL && iterator < dirs)
      {
      printf("\"%s\" encontrado! e nao tem subdiretorios\n", searching_for1);
      int foundinode = get_file_inode(path[i+1]);
      return foundinode;
      }

    encontrou = 0;
    i++;
    current_dir = current_dir->filho;
  }

  return SUCCESS;
}

int path_parser(char* path, char* pathfound)
{
  char* token;
  char aux[2048];
  strcpy(aux, path);

  printf ("Path: \"%s\"\n", path);

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
  printf("procurando por \"%s\" como arquivo em %s\n", filename, current_dir->name);
  int dir_inode = current_dir->record->inodeNumber;
  int sector_to_read;
  div_t output = div(dir_inode, 256);
  sector_to_read = inode_area + output.quot;

  printf("inode area sector: %d\n", inode_area);
  printf("sector to read: %d\n", sector_to_read);
  printf("dir inode number: %d\n", dir_inode);

  // lendo inode do diretorio atual
  read_sector(sector_to_read, buffer);

  INO_t* inode_aux = malloc(sizeof(INO_t));
  inode_aux->dataPtr[0] = *((int *)(buffer + dir_inode*16));
  inode_aux->dataPtr[1] = *((int *)(buffer + dir_inode*16 + 4));
  inode_aux->singleIndPtr = *((int *)(buffer + dir_inode*16 + 8));
  inode_aux->doubleIndPtr = *((int *)(buffer + dir_inode*16 + 12));
  current_inode = inode_aux;
  printf("pointers: %d %d %d %d\n", inode_aux->dataPtr[0], inode_aux->dataPtr[1], inode_aux->singleIndPtr, inode_aux->doubleIndPtr);

  // ler bloco de dados do diretorio atual para procurar arquivo

  sector_to_read = data_area + inode_aux->dataPtr[0];
  printf("%d\n", sector_to_read);

  read_block(sector_to_read);

  current_record = NULL;
  REC_t* auxrecord = malloc(16*sizeof(int));
  int iterator = 0;       //um bloco pode ter no maximo 64 records (64 * 4 * 16 = 4096)
  while (iterator < 64)
  {

    auxrecord->TypeVal = *((BYTE *)(blockbuffer + iterator*64));
    strncpy(auxrecord->name, blockbuffer + iterator*64 + 1, 32);
    auxrecord->blocksFileSize = *((DWORD *)(blockbuffer + iterator*64 + 33));
    auxrecord->bytesFileSize = *((DWORD *)(blockbuffer + iterator*64 + 37));
    auxrecord->inodeNumber = *((int *)(blockbuffer + iterator*64 + 41));


    {
      printf("record encontrado:\n");
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
    printf("%d\n", iterator);
  }
  if (current_record == NULL)
  {
    printf("arquivo %s não existe\n", filename);
    return ERROR;
  }
  printf("arquivo %s encontrado!\n", filename);

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
    printf("erro: numero maximo de arquivos abertos atingido: %d\n", MAX_OPENFILES);
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
    aux_file->inode = inode_number;
    aux_file->nextfile = -1;
    open_files.filesopen++;
  }

  printf("arquivos abertos: %d\n", open_files.filesopen);
  printf("arquivo aberto tem inode: %d \n", current_file->inode);

  return i;
}

void read_block(int sector)
{
  int i;
  int x = 0;
  for (i = 0; i < 16; ++i)
  {
    read_sector(sector + i, buffer);
    memcpy((blockbuffer + x), buffer, sizeof(buffer));
    x = x + 256;
  }
}

int get_handle()
{

}