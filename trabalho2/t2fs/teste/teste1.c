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

  create2("/file");
  create2("/file0");
  create2("/file1");
  create2("/file2");
  create2("/file3");
  create2("/file4");
  create2("/file5");
  create2("/file6");
  create2("/file7");
  create2("/file8");
  create2("/file9");
  create2("/file10");
  create2("/file11");
  create2("/file12");
  create2("/file13");
  create2("/file14");
  create2("/file15");
  create2("/file16");
  create2("/file17");
  create2("/file18");
  create2("/file19");
  create2("/file20");
  mkdir2("/dir1");
  mkdir2("/dir2");
  mkdir2("/dir3");
  mkdir2("/sub/dir");

  opendir2("/sub");
  open2("file");
  open2("/file0");
  open2("/file1");
  open2("/file2");
  open2("/file3");
  open2("/file4");
  open2("/file5");
  open2("/file6");
  open2("/file7");
  open2("/file8");
  open2("/file9");
  open2("/file10");
  open2("/file11");
  open2("/file12");
  open2("/file13");
  open2("/file14");
  open2("/file15");
  open2("/file16");
  open2("/file17");
  open2("/file18");
  open2("/file19");
  open2("/file20");
  printf("\n\n");
  debug_buffer_disk(0,1,0);
  printf("\n\n");
  debug_buffer_disk(0,1,2);
  printf("\n\n");
  debug_buffer_disk(0,0,0);
  return 0;
}