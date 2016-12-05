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

  int sub = opendir2("/sub/");
  int file = open2("file");
  int arq = open2("/arq");
  int arq2 = open2("/sub/arq2");

  int size = 80;
  char buffer[size];
  int leu = read2(arq, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n\n\n", leu, size, buffer);
  size = 10;
  leu = read2(arq, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n\n\n", leu, size, buffer);

  leu = read2(arq2, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n\n\n", leu, size, buffer);
  size = 80;
  leu = read2(arq2, buffer, size);
  printf("leu %d bytes de %d pedidos: %s\n\n\n", leu, size, buffer);

  close2(arq);
  close2(arq2);
  close2(sub);
  closedir2(sub);
  closedir2(file);
  close2(file);

  printf("\n\n");
  debug_buffer_disk(0,1,0);
  printf("\n\n");
  debug_buffer_disk(0,1,2);
  printf("\n\n");
  debug_buffer_disk(0,0,0);
  return 0;
}