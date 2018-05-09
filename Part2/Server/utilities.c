#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utilities.h"
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

Input verifiyInput(int argc, char const *argv[]) {
  checkArgumentAmount(argc, 3, "Usage:  server <num_row_seats> <num_ticket_offices> <open_time>");

  Input returnInputs;
  returnInputs.nSeats = validateIntArgument(argv[1], "Invalid number for <num_row_seats>", MAX_ROOM_SEATS, "max_row_seats", "MAX_ROOM_SEATS");
  returnInputs.nTicketOffices = validateIntArgument(argv[2], "Invalid number for <num_ticket_offices>", MAX_TICKET_OFFICES, "num_ticket_offices", "MAX_TICKET_OFFICES");
  returnInputs.openTime = validateIntArgument(argv[3], "Invalid number for <open_time>", MAX_OPEN_TIME, "open_time", "MAX_OPEN_TIME");

  return returnInputs;
}

int validateIntArgument(const char * arg, const char * errorMsg, int max_val, const char* var_name, const char* max_macro_name) {
  int desiredVal = checkValidIntArgument(arg, errorMsg);
  int val = min(desiredVal, max_val);
  if(desiredVal != val) {
    printf("The Specified %s is greater than %s (%d). The max value has been set.\n", var_name, max_macro_name, max_val);
  }

  return val;
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
