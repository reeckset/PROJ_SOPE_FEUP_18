#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "macros.h"
#include "readArgs.h"

void displayEntries(struct dirent *filesList, size_t size);

void checkArgs(int argc, char const *argv[]);

int main(int argc, char const *argv[]) {

  checkArgs(argc, argv);

  char emptyChar = 0;
  char *pattern = &emptyChar;
  char *path = &emptyChar;
  byte optionsMask = getOptionsMask(argc, argv, pattern, path);
  return 0;
}
