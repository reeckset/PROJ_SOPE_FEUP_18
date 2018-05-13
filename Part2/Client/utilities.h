#ifndef UTILITIES_H
#define UTILITIES_H

int checkValidIntArgument(const char *argument, const char *errorMsg);

void checkArgumentAmount(int argc, int expected, const char *usage);

int getIntAmount(const char *str);

int *stringToIntArray(const char *str, const char *errorMsg, int* size);

void printfIntArray(int *array, int size);

void writeToLog(int fd, char *format, ...);

#endif
