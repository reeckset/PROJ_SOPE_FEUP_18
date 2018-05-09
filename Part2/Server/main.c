#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "utilities.h"
#include "macros.h"
#include "server.h"
#include "timeCounter.h"

/*
argv
[1] - nSeats
[2] - nTicketOffices
[3] - openTime (s)
*/

int main(int argc, char const *argv[]) {

  Input inputs = verifiyInput(argc, argv);

  int pid = fork();

  if(pid < 0) {
    perror("Error creating child process");
    exit(FORK_ERROR_EXIT);
  } else if(pid == 0) { // child
    initServer(inputs);
  } else { // parent
    initTimer(inputs);
  }

  return 0;
}
