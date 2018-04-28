int checkValidIntArgument(const char *argument, const char *errorMsg);

void checkArgumentAmount(int argc, int expected, const char *usage);

int getIntAmount(const char *str);

int *stringToIntArray(const char *str, const char *errorMsg, int* size);
