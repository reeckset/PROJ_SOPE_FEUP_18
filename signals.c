#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void sigint_handler(int signo) {

  fflush(STDIN_FILENO);
  printf("Are you sure you want to stop this search? (y/n)\n");
  kill(-getpid(), SIGKILL);

  char dayum = ' ';
  while (dayum != 'n' && dayum != 'y') {
    printf("Are you sure you want to stop this search? (y/n)\n");
    scanf("%c", &dayum);
  }
  if (dayum == 'y') {
    kill(-getpid(), SIGKILL);
  } else {
    kill(-getpid(), SIGCONT);
  }
}

void swapSigintHandler() {

  printf("ACTIVATING HANDLER\n");

  struct sigaction oldsa;
  struct sigaction sa;
  sa.sa_handler = &sigint_handler;

  if (sigaction(SIGINT, &sa, &oldsa) < 0) {
    perror("SIGINT handler error");
    exit(1);
  }

  struct sigaction oldsb;
  struct sigaction sb;
  sb.sa_handler = SIG_IGN;

  if (sigaction(SIGHUP, &sb, &oldsb) < 0) {
    perror("SIGSTOP handler error");
    exit(1);
  }
}
