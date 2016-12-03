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

  debug_buffer_disk(0,0,0);
  debug_buffer_disk(0,1,0);
  debug_buffer_disk(0,1,2);

  // create2("/file");
  // opendir2("/sub/");
  create2("/sub/file");

  debug_buffer_disk(0,0,0);
  debug_buffer_disk(0,1,0);
  debug_buffer_disk(0,1,2);

  // testando criar um arquivo dentro de sub
  // create2 nao sabe qual é o bloco de sub e não consegue criar

  // opendir2("/opa/sub");
  // opendir2("/sub/");
  // opendir2("/sub/asd");
  // opendir2("/opa/asd");
  // opendir2("/opa/asd/asd");

  // open2("file");
  // open2("/opa/file");
  // open2("/sub");
  // open2("/wsub/asd/sd");
  // open2("/sub/asd/");
  // open2("/sub/asd/asdasdsa");
  // create2("file");

  return 0;
}