#include <sys/stat.h>
#include <sys/types.h>
#include "macros.h"


int readPath(char *path, byte optionsMask, const char* pattern);
void displayEntries(struct dirent *filesList, size_t size);
int processEntries(struct dirent *filesList, size_t size, byte optionsMask, char *path, const char* pattern);
void processFiles(struct dirent *filesList, size_t size, char* path, const char* pattern);
char *getPath(char *folder, char *name);
int readDir(char *path, byte optionsMask, const char *pattern);
