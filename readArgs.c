#include "macros.h"
#include "readArgs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

byte getOptionsMask(int argc, char const *argv[], char *pattern, char *path) {
  int i;
  byte mask = 0;
  for (i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
      mask |= getMaskedValue(argv[i] + 1);
    } else {
      memcpy(pattern, argv[i], strlen(argv[i]));
      if (i < argc - 1) {
        memcpy(path, argv[i + 1], strlen(argv[i + 1]));
      } else {
        mask |= readstdin;
      }
      break;
    }
  }
  return mask;
}

byte getMaskedValue(char const *option) {
  byte result = 0;
  while (option[0] != 0) {
    switch (option[0]) {
    case 'i':
      result |= notCaseFlag;
      break;
    case 'l':
      result |= showFileNamesFlag;
      break;
    case 'n':
      result |= showLineNumberFlag;
      break;
    case 'c':
      result |= nLinesFlag;
      break;
    case 'w':
      result |= wholeWordFlag;
      break;
    case 'r':
      result |= recursiveFlag;
      break;
    }
    option++;
  }
  return result;
}

void checkArgs(int argc, char const *argv[]) {
  if (argc < 2) {
    printf(
        "Usage: %s <options(optional)> <pattern> <path to analyze(optional)>\n",
        argv[0]);
    exit(1);
  }
}
