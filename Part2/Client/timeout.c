#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"
#include "macros.h"
#include "timeout.h"

void gracefulShutdownOnTimeout(int timeout, int pid) {
  int sleepTime = usleep(timeout * 1000);
  printf("Missing time: %d\n", sleepTime);

  char *fifoName = NULL;
  asprintf(&fifoName, "/tmp/ans%d", getpid());

  sem_t *sem = get_client_fifo_semaphore(getpid());
  sem_wait(sem);

  kill(pid, SIGKILL);
  int fdResponse = open(fifoName, O_RDWR);
  if (fdResponse == -1) {
    perror("Opening response FIFO");
    sem_post(sem);
    sem_close(sem);
    exit(FIFO_ERROR_EXIT);
  }
  struct pollfd pfd;
  pfd.fd = fdResponse;
  pfd.events = POLLIN;
  if (poll(&pfd, 1, 10) == 1 && pfd.revents == POLLIN) {
    readFromServer(fdResponse, sem);
  }

  sem_post(sem);
  sem_close(sem);

  unlink(fifoName);
  free(fifoName);
}

sem_t *get_client_fifo_semaphore(int pid) {
  sem_t *sem;
  char *buffer = NULL;
  asprintf(&buffer, "/semAnswer%d", pid);
  if ((sem = sem_open(buffer, O_CREAT, 0666, 1)) == SEM_FAILED) {
    perror("Opening semaphore error");
    exit(SEMAPHORE_ERROR);
  }
  free(buffer);
  return sem;
}
