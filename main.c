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
#include "divideFile.h"

void displayEntries(struct dirent *filesList, size_t size);

void checkArgs(int argc, char const *argv[]);

int main(int argc, char const *argv[]) {

  swapSigintHandler();

  checkArgs(argc, argv);

  char pattern[200] = {0};
  char path[200] = {0};

  byte optionsMask = getOptionsMask(argc, argv, pattern, path);
  int n_lines = 0;
  if (IsReadStdin(optionsMask)) {
    FILE* std_file;
    if((std_file = fdopen(STDIN_FILENO, "r")) == NULL) {
      perror("Opening error");
      exit(-1);
    }
    n_lines =  divideFilePtr(std_file, pattern, optionsMask);
  } else {
    n_lines = readPath(path, optionsMask, pattern);
    siginfo_t t;
    while (waitid(P_ALL, -1, &t, WEXITED) != -1) {
      if (t.si_code == CLD_EXITED)
        if (t.si_status > 0)
          n_lines += t.si_status;
    }
  }
  if (IsNLinesFlag(optionsMask))
    printf("Found %d lines.\n", n_lines);
  return 0;
}
