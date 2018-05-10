#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "macros.h"

int checkValidIntArgument(const char *argument, const char *errorMsg) {
  char *intConversionEndPtr = NULL;
  int result = strtol(argument, &intConversionEndPtr, 10);
  if (*intConversionEndPtr != '\0') {
    printf("Error reading integer argument: %s\n", errorMsg);
    exit(ILLEGAL_ARG_EXIT);
  }
  return result;
}

void checkArgumentAmount(int argc, int expected, const char *usage) {
  if (argc != expected) {
    printf("Invalid arguments. Usage is: %s\n", usage);
    exit(ILLEGAL_ARG_EXIT);
  }
}

int getIntAmount(const char *str) {
  int c, total = 0, lastWasInt = 0;
  int stringLength = strlen(str);
  for (c = 0; c < stringLength; c++) {
    if (str[c] == ' ' && lastWasInt) {
      total++;
      lastWasInt = 0;
    }
    if (isdigit(str[c])) {
      lastWasInt = 1;
      if (c == stringLength - 1) {
        total++;
      }
    } else {
      lastWasInt = 0;
    }
  }
  return total;
}

int *stringToIntArray(const char *str, const char *errorMsg, int *size) {
  *size = getIntAmount(str);
  char *strCopy = strdup(str);
  char *currStrPtr = strCopy;
  int *result = (int *)malloc(*size * sizeof(int));
  int counter = 0;
  while (sscanf(currStrPtr, "%d", result + counter) == 1 && counter < *size) {
    counter++;
    while (!isdigit(*currStrPtr)) {
      currStrPtr++;
    }
    while (isdigit(*currStrPtr)) {
      currStrPtr++;
    }
  }
  if (*size != counter) {
    printf("Invalid conversion from string to int array: %s\n", errorMsg);
    exit(INVALID_STR2INT_ARRAY_EXIT);
  }
  free(strCopy);
  return result;
}


void printfIntArray(int *array, int size) {
  int i;
  for (i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}
