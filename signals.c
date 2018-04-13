#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "logger.h"

struct sigaction oldsb;
struct sigaction oldsc;

void sigint_handler(int signo) {
  kill(0, SIGTSTP);
  write_to_logger("Stoping all children");
  char dayum = 0;
  char c;
  do {
    printf("\nAre you sure you want to stop this search? (y/n)\n");
  } while(scanf("%c", &dayum)!= 1 && dayum != 'n' && dayum != 'y');

  while((c = getchar()) != '\n' && c != EOF);

  if (dayum == 'y') {
    write_to_logger("Killing all children");
    kill(0, SIGKILL);
  } else if( dayum == 'n'){
    write_to_logger("Resuming all children");
    kill(0, SIGCONT);
  }
  printf("Handler end \n");
  sleep(1);
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


  struct sigaction sb;
  sb.sa_handler = SIG_IGN;

  if (sigaction(SIGTSTP, &sb, &oldsb) < 0) {
    perror("SIGTSTP handler error");
    exit(1);
  }
}

void ignore_sig_int() {
  struct sigaction oldsa;
  struct sigaction sb;
  sb.sa_handler = SIG_IGN;

  if (sigaction(SIGINT, &sb, &oldsa) < 0) {
    perror("SIGTSTP handler error");
    exit(1);
  }


  if (sigaction(SIGTSTP, &oldsb, NULL) < 0) {
    perror("SIGTSTP handler error");
    exit(1);
  }
}
