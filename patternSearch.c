#include "patternSearch.h"
#include <cstdbool>
#include <string.h>

bool contains(char *line, char *pattern) {
  return strstr(line, pattern) != NULL;
}

bool containsNonCase(char *line, char *pattern) {
  return strcasestr(line, pattern) != NULL;
}

bool containsWord(char *line, char *pattern) {
  char *foundWord = strstr(line, pattern);
  return isWholeWord(line, foundWord, strlen(pattern));
}

bool containsNonCaseWord(char *line, char *pattern) {
  char *foundWord = strcasestr(line, pattern);
  return isWholeWord(line, foundWord, strlen(pattern));
}

bool isWholeWord(char *line, char *foundWord, unsigned int sizeOfFoundWord) {
  if (foundWord == NULL) {
    return false;
  }
  if (foundWord == line || isDelimiter(*foundWord)) {
    // sanity check - never going to be false because one
    // contains the other
    if (sizeOfFoundWord + 1 <= strlen(line)) {
      return isDelimiter(foundWord[sizeOfFoundWord + 1]);
    }
  }
  return false;
}

bool isDelimiter(char c) {
  // return (c == ' ' || c == '\n' || c== '\t' || c=='\0');
  return !((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') || c == '_');
}
