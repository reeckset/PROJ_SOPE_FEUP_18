#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "divideFile.h"
#include "readDirectory.h"
#include "signals.h"

void readPath(char *path, byte optionsMask, const char *pattern) {
  struct stat pathInfo;
  stat(path, &pathInfo);
  if (S_ISREG(pathInfo.st_mode)) {
    divideFile(path, pattern);
    return;
  }

  if (S_ISDIR(pathInfo.st_mode)) {
    if (IsRecursiveFlag(optionsMask)) {
      int pid = fork();
      if (pid == 0) {
        ignore_sig_int();
        readDir(path, optionsMask, pattern);
        exit(0);
      } else if (pid < 0) {
        perror("Fork error");
        exit(2);
      }
    } else {
      printf("The inserted path is a directory. Please use the -r recursive "
             "flag to check its contents\n");
    }
  }
}

// TOD  O DELETE
void displayEntries(struct dirent *filesList, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf(" - %s\n", filesList[i].d_name);
  }
}

void processEntries(struct dirent *filesList, size_t size, byte optionsMask,
                    char *path, const char *pattern) {
  for (size_t i = 0; i < size; i++) {
    if ((strlen(filesList[i].d_name) == 1 &&
         strncmp(filesList[i].d_name, ".", 1) == 0) ||
        (strlen(filesList[i].d_name) == 2 &&
         strncmp(filesList[i].d_name, "..", 2) == 0))
      continue;
    char *newPath = getPath(path, filesList[i].d_name);
    if (newPath != 0) {
      readPath(newPath, optionsMask, pattern);
      free(newPath);
    }
  }
}

void processFiles(struct dirent *filesList, size_t size, char *path,
                  const char *pattern) {
  for (size_t i = 0; i < size; i++) {
    struct stat pathInfo;
    char *filePath = getPath(path, filesList[i].d_name);
    if (filePath == 0)
      continue;
    stat(filePath, &pathInfo);
    if (S_ISREG(pathInfo.st_mode)) {
      divideFile(filePath, pattern);
    }
    free(filePath);
  }
}

char *getPath(char *folder, char *name) {
  char *newPath =
      (char *)calloc(strlen(name) + strlen(folder) + 2, sizeof(char));

  strncat(newPath, folder, strlen(folder) + 1);
  strncat(newPath, "/", strlen(newPath) + 2);
  strncat(newPath, name, strlen(newPath) + strlen(name) + 1);
  return newPath;
}

void readDir(char *path, byte optionsMask, const char *pattern) {
  DIR *reqDir;
  // Since it is a folder, it can be opened
  reqDir = opendir(path);

  struct dirent *dir;
  struct dirent *filesList = (struct dirent *)malloc(0);

  size_t counter = 0;
  while ((dir = readdir(reqDir)) != NULL) { // If failed reading dir
    counter++;
    filesList =
        (struct dirent *)realloc(filesList, counter * sizeof(struct dirent));
    memcpy(&filesList[counter - 1], dir, sizeof(*dir));
  }
  // close the dir after processing
  closedir(reqDir);
  processEntries(filesList, counter, optionsMask, path, pattern);
}
