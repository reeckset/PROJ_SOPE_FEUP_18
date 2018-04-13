#include <sys/times.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

FILE *logger = NULL;

void write_to_logger(const char *message) {
  if (logger == NULL) {
    fprintf(stderr, "Logfile is NULL\n");
  }

  clock_t start;
  struct tms t;
  long ticks;
  start = times(&t); /* início da medição de tempo */
  ticks = sysconf(_SC_CLK_TCK);
  fprintf(logger, "%4.2f - %.8d - %s\n", (double)(start), getpid(), message);
}

void initiate_to_logger(const char *log_filename) {
  if ((logger = fopen(log_filename, "a")) == NULL) {
    perror("Wrong file path");
    exit(-1);
  }
}
