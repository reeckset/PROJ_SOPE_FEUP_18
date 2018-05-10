#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <stdio.h>

#include "timeout.h"

void gracefulShutdownOnTimeout(int timeout) {
  int sleepTime = sleep(timeout);
  printf("Missing time: %d\n", sleepTime);
  kill(0, SIGKILL);
  // TODO BEFORE KILLING CHILD, CHECK IF FIFO IS EMPTY TO AVOID INFORMATION LOSS
  // EITHER DO FSEEK
}
