#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "macros.h"
#include "readArgs.h"
#include "readDirectory.h"
#include "signals.h"

void displayEntries(struct dirent *filesList, size_t size);

void checkArgs(int argc, char const *argv[]);

int main(int argc, char const *argv[]) {

  swapSigintHandler();

  checkArgs(argc, argv);

  char pattern[200] = {0};
  char path[200] = {0};

  byte optionsMask = getOptionsMask(argc, argv, pattern, path);

  readPath(path, optionsMask, pattern);

  siginfo_t t;
  while (waitid(P_ALL, -1, &t, WEXITED) != -1);
  printf("Father exit\n");
  return 0;
}
