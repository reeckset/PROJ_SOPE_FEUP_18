#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "patternSearch.h"

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
  while ((line_size = getline(&line, &zero, fileToDivide)) != -1) {
    line_number++;
    // Process line
    if (contains(line, pattern)) {
      // sleep(10);
      printf("%s\n", line);
    }

    line_size = 0;
  }
  free(line);
  return 0;
}
