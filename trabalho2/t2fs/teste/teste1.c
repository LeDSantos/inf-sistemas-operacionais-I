//
//  main.c
//  t2fs
//
//  Created by Henrique Valcanaia on 14/11/16.
//  Copyright © 2016 SISOP. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "../include/t2fs.h"

int main(int argc, const char * argv[]) {

  // debug_buffer_disk(0,1,0);
  // debug_buffer_disk(0,1,2);
  // debug_buffer_disk(0,0,0);

  create2("/file");
  int sub = opendir2("/sub/");
  int file = open2("file");
  open2("/sub/file");
  create2("/sub/file");
  int subfile = open2("/sub/file");
  int arq = open2("/arq");
  int arq2 = open2("/sub/arq2");
  close2(arq);
  close2(arq2);
  close2(sub);
  closedir2(sub);
  closedir2(file);
  close2(file);
  close2(subfile);
  create2("/arq2");
  delete2("/arq2");
  delete2("/sub/file");
  create2("/sub/file2");

  debug_buffer_disk(0,1,0);
  debug_buffer_disk(0,1,2);
  // debug_buffer_disk(0,0,0);

  arq = open2("arq");
  arq2 = open2("/sub/arq2");
  int size = 80;
  char buffer[size];
  int leu = read2(arq, buffer, size);
  int i = 0;
  for (i = leu; i >= 0; --i)
  {
    leu = read2(arq, buffer, size);
    printf("leu %d bytes de %d pedidos: %s\n", leu, size, buffer);
    seek2(arq, i);
    truncate2(arq);
  }

  leu = read2(arq2, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n", leu, size, buffer);
  seek2(arq2, 35);
  truncate2(arq2);
  leu = read2(arq2, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n", leu, size, buffer);

  DIRENT2 dir;
  sub = opendir2("/sub");
  readdir2(sub, &dir);
  printf("nome lido: %s\n", dir.name);
  printf("tipo lido: %x\n", dir.fileType);
  printf("tamanho lido: %d\n", dir.fileSize);

  readdir2(sub, &dir);
  readdir2(sub, &dir);
  create2("/sub/file3");
  create2("/sub/file4");
  readdir2(sub, &dir);
  readdir2(sub, &dir);
  readdir2(sub, &dir);
  readdir2(sub, &dir);
  readdir2(sub, &dir);
  // OK, tudo funcionou ate aqui

  seek2(arq2, 10);
  char *teste = "teste2";
  strcpy(buffer, teste);
  write2(arq2, buffer, strlen(teste));
  leu = read2(arq2, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n", leu, size, buffer);

  // rmdir2("/sub");
  // mkdir2("/dae");
  // mkdir2("/asd");
  // int dae = opendir2("/dae");
  // closedir2(dae);
// 0 = root
// 1 = arq
// 2 = sub
// 3 = arq2
// 4 = file
// 5 = /sub/file

  return 0;
}