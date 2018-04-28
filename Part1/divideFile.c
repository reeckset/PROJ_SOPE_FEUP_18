#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "macros.h"
#include "patternSearch.h"
#include "logger.h"

bool is_binary(const void *data, size_t len) {
  return memchr(data, '\0', len) != NULL;
}

bool print_options(const char *fileName, char *line, int line_n,
                   const char *patten, byte optionsMask) {
  if (IsShowFileNamesFlag(optionsMask)) {
    printf("%s\n", fileName);
    return true;
  } else if (IsShowLineNumberFlag(optionsMask)) {
    printf("%s, %d:  %s\n", fileName, line_n, line);
  } else {
    printf("%s:  %s\n", fileName, line);
  }
  return false;
}

int choose_options(const char *fileName, char *line, int line_number,
                   const char *pattern, byte mask) {
  if (IsNotCaseFlag(mask) && IsWholeWordFlag(mask) &&
      containsNonCaseWord(line, pattern)) {
    if (print_options(fileName, line, line_number, pattern, mask))
      return -1;
    else
      return 1;
  } else if (IsWholeWordFlag(mask) && !IsNotCaseFlag(mask) &&
             containsWord(line, pattern)) {
    if (print_options(fileName, line, line_number, pattern, mask))
      return -1;
    else
      return 1;
  } else if (IsNotCaseFlag(mask) && !IsWholeWordFlag(mask) &&
             containsNonCase(line, pattern)) {
    if (print_options(fileName, line, line_number, pattern, mask))
      return -1;
    else
      return 1;
  } else if (!IsWholeWordFlag(mask) && !IsNotCaseFlag(mask) &&
             contains(line, pattern)) {
    if (print_options(fileName, line, line_number, pattern, mask))
      return -1;
    else
      return 1;
  }
  return 0;
}

int divideFile(const char *fileName, const char *pattern, byte optionsMask) {
  FILE *fileToDivide;
  char *line = NULL;
  ssize_t line_size = 0;
  int n_lines = 0;

  if ((fileToDivide = fopen(fileName, "r")) == NULL) {
    // Log the fail
    return -1;
  }

  char *msg_buffer = NULL;
  asprintf(&msg_buffer, "Opening %s", fileName);
  write_to_logger(msg_buffer);
  free(msg_buffer);

  int line_number = 0;
  size_t zero = 0;
  bool checked = false;
  while ((line_size = getline(&line, &zero, fileToDivide)) != -1) {
    line_number++;
    // Process line
    if (!checked && is_binary(line, line_size)) {
      printf("%s is binary\n", fileName);
      break;
    }
    checked = true;
    int ret_value;
    if ((ret_value = choose_options(fileName, line, line_number, pattern,
                                    optionsMask)) == -1)
      return 0;
    else
      n_lines += ret_value;
    line_size = 0;
  }
  free(line);
  fclose(fileToDivide);
  msg_buffer = NULL;
  asprintf(&msg_buffer, "Closing %s", fileName);
  write_to_logger(msg_buffer);
  free(msg_buffer);
  return n_lines;
}

int divideFilePtr(FILE *fileToDivide , const char *pattern, byte optionsMask) {
  char fileName[] = "STDIN";
  char *line = NULL;
  ssize_t line_size = 0;
  int n_lines = 0;
  int line_number = 0;
  size_t zero = 0;
  bool checked = false;
  while ((line_size = getline(&line, &zero, fileToDivide)) != -1) {
    line_number++;
    // Process line
    if (!checked && is_binary(line, line_size)) {
      printf("%s is binary\n", fileName);
      break;
    }
    checked = true;
    int ret_value;
    if ((ret_value = choose_options(fileName, line, line_number, pattern,
                                    optionsMask)) == -1)
      return 0;
    else
      n_lines += ret_value;
    line_size = 0;
  }
  free(line);
  return n_lines;
}
