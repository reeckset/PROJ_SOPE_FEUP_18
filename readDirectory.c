#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "divideFile.h"
#include "readDirectory.h"
#include "signals.h"

int readPath(char *path, byte optionsMask, const char *pattern) {
  struct stat pathInfo;
  stat(path, &pathInfo);
  if (S_ISREG(pathInfo.st_mode)) {
    return divideFile(path, pattern, optionsMask);
  }

  if (S_ISDIR(pathInfo.st_mode)) {
    if (IsRecursiveFlag(optionsMask)) {
      int pid = fork();
      if (pid == 0) {
        ignore_sig_int();
        int n_lines = readDir(path, optionsMask, pattern);
        siginfo_t t;
        while (waitid(P_ALL, -1, &t, WEXITED) != -1) {
          if (t.si_code == CLD_EXITED)
            if (t.si_status > 0)
              n_lines += t.si_status;
        }
        exit(n_lines);
      } else if (pid < 0) {
        perror("Fork error");
        exit(-2);
      }
    } else {
      printf("The inserted path is a directory. Please use the -r recursive "
             "flag to check its contents\n");
    }
  }
  return 0;
}

// TODO DELETE
void displayEntries(struct dirent *filesList, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf(" - %s\n", filesList[i].d_name);
  }
}

int processEntries(struct dirent *filesList, size_t size, byte optionsMask,
                    char *path, const char *pattern) {
  int n_lines = 0;
  for (size_t i = 0; i < size; i++) {
    if ((strlen(filesList[i].d_name) == 1 &&
         strncmp(filesList[i].d_name, ".", 1) == 0) ||
        (strlen(filesList[i].d_name) == 2 &&
         strncmp(filesList[i].d_name, "..", 2) == 0))
      continue;
    char *newPath = getPath(path, filesList[i].d_name);
    if (newPath != 0) {
      n_lines += readPath(newPath, optionsMask, pattern);
      free(newPath);
    }
  }
  return n_lines;
}

/*
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
*/
char *getPath(char *folder, char *name) {
  char *newPath =
      (char *)calloc(strlen(name) + strlen(folder) + 2, sizeof(char));

  strncat(newPath, folder, strlen(folder) + 1);
  strncat(newPath, "/", strlen(newPath) + 2);
  strncat(newPath, name, strlen(newPath) + strlen(name) + 1);
  return newPath;
}

int readDir(char *path, byte optionsMask, const char *pattern) {
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
  return processEntries(filesList, counter, optionsMask, path, pattern);
}
