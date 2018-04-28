#include <stdbool.h>

bool isDelimiter(char c);
bool containsWord(char *line, const char *pattern);
bool isWholeWord(char *line, char *foundWord, unsigned int sizeOfFoundWord);
bool containsNonCase(char *line, const char *pattern);
bool containsNonCaseWord(char *line, const char *pattern);
bool contains(char *line, const char *pattern);
