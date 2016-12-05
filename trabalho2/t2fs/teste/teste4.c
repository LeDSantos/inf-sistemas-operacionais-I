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

  printf("\n\n");
  debug_buffer_disk(0,1,0);
  printf("\n\n");
  debug_buffer_disk(0,1,2);
  printf("\n\n");
  debug_buffer_disk(0,0,0);


  create2("/sub/file3");
  create2("/sub/file4");
  create2("/sub/file5");
  create2("/sub/file6");
  create2("/sub/file7");

  DIRENT2 dir;
  int sub = opendir2("/sub");
  readdir2(sub, &dir);
  printf("nome lido: %s\n", dir.name);
  printf("tipo lido: %x\n", dir.fileType);
  printf("tamanho lido: %d\n", dir.fileSize);

  int i;
  for (i = 0; i < 8; ++i)
  {
    readdir2(sub, &dir);
    printf("nome lido: %s\n", dir.name);
    printf("tipo lido: %x\n", dir.fileType);
    printf("tamanho lido: %d\n\n\n", dir.fileSize);
  }

  int arq2 = open2("/sub/arq2");

  char *teste = "**ALTERANDO**";
  char buffer[strlen(teste)];
  strcpy(buffer, teste);

  seek2(arq2, 25);
  write2(arq2, buffer, strlen(teste));
  int leu = read2(arq2, buffer, 50);
  printf("leu %d bytes de 50 pedidos: %s\n\n\n", leu, buffer);

  strcpy(buffer, teste);
  seek2(arq2, 5);
  write2(arq2, buffer, strlen(teste));
  leu = read2(arq2, buffer, 45);
  printf("leu %d bytes de 45 pedidos: %s\n\n\n", leu, buffer);

  close2(arq2);
  delete2("/sub/file3");
  delete2("/sub/file4");
  delete2("/sub/file5");
  rmdir2("/dir1");


  printf("\n\n");
  debug_buffer_disk(0,1,0);
  printf("\n\n");
  debug_buffer_disk(0,1,2);
  printf("\n\n");
  debug_buffer_disk(0,0,0);

  return 0;
}