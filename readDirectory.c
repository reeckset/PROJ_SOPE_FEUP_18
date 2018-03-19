#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readDirectory.h"

void readPath(char *path) {
  struct stat pathInfo;
  stat(path, &pathInfo);

  if (S_ISREG(pathInfo.st_mode)) {
    printf("Path is file, so doesn't have any file inside.\n");
    exit(2);
  } else if (S_ISDIR(pathInfo.st_mode)) {
    printf("Path is a folder/dir. Its files are: \n");

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
      // filesList[counter-1] = *dir;
    }

    // close the dir after processing
    closedir(reqDir);

    displayEntries(filesList, counter);
  }
}
void displayEntries(struct dirent *filesList, size_t size) {
  for (size_t i = 0; i < size; i++) {
    printf(" - %s\n", filesList[i].d_name);
  }
}
