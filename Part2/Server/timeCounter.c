#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "macros.h"
#include "timeCounter.h"

void initTimer(Input inputs, int pid) {

  int remainingTime = inputs.openTime;

  // Hold for MAX_OPEN_TIME
  while ((remainingTime = sleep(remainingTime)))
    ;

  // Send signal to close ticket offices
  kill(pid, SIGUSR1);
  int status;
  if (wait(&status) != pid) {
    exit(FORK_WAIT_ERROR);
  };
}
