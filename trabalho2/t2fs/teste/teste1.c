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

  // debug_buffer_disk(0,0,0);
  // debug_buffer_disk(0,1,0);
  // debug_buffer_disk(0,1,2);

  create2("/file");
  int sub = opendir2("/sub/");
  create2("/sub/file");

  // debug_buffer_disk(0,0,0);
  // debug_buffer_disk(0,1,0);
  // debug_buffer_disk(0,1,2);

  open2("file");
  open2("/sub/file");
  create2("/sub/file");
  int arq = open2("/arq");
  int arq2 = open2("/sub/arq2");
  close2(arq);
  close2(arq2);
  close2(sub);

  return 0;
}