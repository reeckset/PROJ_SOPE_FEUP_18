#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "patternSearch.h"

bool is_binary(const void *data, size_t len) {
  return memchr(data, '\0', len) != NULL;
}


int divideFile(const char *fileName, const char *pattern) {
  FILE *fileToDivide;
  char *line = NULL;
  ssize_t line_size = 0;

  if ((fileToDivide = fopen(fileName, "r")) == NULL) {
    // Log the fail
    return -1;
  }

  int line_number = 0;
  size_t zero = 0;
  bool checked = false;
  while ((line_size = getline(&line, &zero, fileToDivide)) != -1) {
    line_number++;
    // Process line
    if(!checked && is_binary(line, line_size)) {
      printf("%s is binary\n", fileName);
      break;
    }
    checked = true;

    if (contains(line, pattern)) {
      //sleep(1);
      printf("%s\n", line);
    }

    line_size = 0;
  }
  free(line);
  return 0;
}
