#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int devideFile(const char *fileName) {
  FILE *fileToDivide;
  char* line = NULL;
  size_t line_size = 0;

  if ((fileToDivide = fopen(fileName, "r")) == NULL) {
    // Log the fail
    return -1;
  }

  int line_number = 0;
  while ((line_size = getline(&line, &line_size, fileToDivide)) != -1) {
      line_number++;
      // Process line
      printf("Line %i: %s\n", line_number, line);
      free(line);
      line_size = 0;
  }
  return 0;
}
