

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

  int arq = open2("/arq");

  int size = 80;
  char buffer[size];
  int leu = read2(arq, buffer, size);
  int i = 0;
  for (i = leu; i >= 0; i = i - 5)
  {
    leu = read2(arq, buffer, size);
    printf("leu %d bytes de %d pedidos:\n %s\n\n\n", leu, size, buffer);
    seek2(arq, i);
    truncate2(arq);
  }

  printf("\n\n");
  debug_buffer_disk(0,1,0);
  printf("\n\n");
  debug_buffer_disk(0,1,2);
  printf("\n\n");
  debug_buffer_disk(0,0,0);

  return 0;
}