int identify2 (char *name, int size)
{
  printf("\nGustavo Madeira Santana - 252853\nCristiano Salla Lunardi - 240508\n\n");
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

    if()
  {
    return -1;
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

  if()
  {
    return -1;
  }

  return 0;
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

  if()
  {
    return -1;
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

  if()
  {
    return -1;
  }

  return 0;
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

  if()
  {
    return -1;
  }

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

  if()
  {
    return -1;
  }

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

  if()
  {
    return -1;
  }

  return 0;
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

  if()
  {
    return -1;
  }

  return 0;
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

  if()
  {
    return -1;
  }

  return 0;
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

  if()
  {
    return -1;
  }

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

  if()
  {
    setInvalid(dentry);
    return -1;
  }

  return 0;
}


/*-----------------------------------------------------------------------------
Função: Fecha o diretório identificado pelo parâmetro "handle".

Entra:  handle -> identificador do diretório que se deseja fechar (encerrar a operação).

Saída:  Se a operação foi realizada com sucesso, a função retorna "0" (zero).
  Em caso de erro, será retornado um valor diferente de zero.
-----------------------------------------------------------------------------*/
int closedir2 (DIR2 handle)
{

  if()
  {
    return -1;
  }

  return 0;
}
