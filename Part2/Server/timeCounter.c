#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "timeCounter.h"
#include "macros.h"

void initTimer(Input inputs, int pid) {

  int remainingTime = MAX_OPEN_TIME;

  //Hold for MAX_OPEN_TIME
  while((remainingTime = sleep(remainingTime)));

  //Send signal to close ticket offices
  kill(pid, SIGUSR1);

  int status;
  if(wait(&status) != pid) {
    exit(FORK_WAIT_ERROR);
  };
}
