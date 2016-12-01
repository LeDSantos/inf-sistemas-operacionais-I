#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/functions.h"
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/bitmap2.h"

int path_parser(char* path, char* pathfound)
{
  char* token;
  char aux[500];
  strcpy(aux, path);

  printf ("Path to parse: \"%s\"\n", path);

  int char_size = 1;
  token = strtok (aux, "/");
  while (token != NULL)
  {
    token = strtok (NULL, "/");
    ++char_size;
  }

  char_size = 0;
  const char *paths[char_size];
  strcpy(aux, path);
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