#ifndef UTILITIES_H
#define UTILITIES_H

typedef struct {
  int nSeats;
  int nTicketOffices;
  int openTime;
} Input;

int checkValidIntArgument(const char *argument, const char *errorMsg);

void checkArgumentAmount(int argc, int expected, const char *usage);

Input verifiyInput(int argc, const char* argv[]);

int validateIntArgument(const char * arg, const char * errorMsg, int max_val, const char* var_name, const char* max_macro_name);

int getIntAmount(const char *str);

int *stringToIntArray(const char *str, const char *errorMsg, int* size, int* returnVal);

char *intArrayToString(int* array, int size, int digitAmount);

void writeToLog(int fd, int nArgs, char *format, ...);

int getNumberOfDigits(int value);

#endif
