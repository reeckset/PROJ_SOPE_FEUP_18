#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "macros.h"
#include "utilities.h"
#include "timeout.h"
#include "client.h"



void attemptSend(const char **argv, int timeout) {
  char *responseFifoName = createResponseFifo();
  writeRequestToFifo(argv[2], getIntAmount(argv[3]), argv[3]);
  int pid = fork();
  if (pid < 0) {
    perror("");
    exit(FORK_ERROR_EXIT);
  }
  if (pid == 0) { // filho
    processReponse(responseFifoName);
  } else { // pai
    gracefulShutdownOnTimeout(timeout);
  }
}

void populateRequest(Request *request, const char **argv) {
  request->numWantedSeats =
      checkValidIntArgument(argv[2], "Number of wanted seats");

  request->preferredSeats =
      stringToIntArray(argv[3], "preferred seats", &request->numPreferredSeats);
}


int main(int argc, char const *argv[]) {
  checkArgumentAmount(argc, 4,
                      "client <time_out> <num_wanted_seats> <pref_seat_list>");
  int timeout = checkValidIntArgument(argv[1], "Time Out");

  Request request;
  populateRequest(&request, argv);

  attemptSend(argv, timeout);

  free(request.preferredSeats);
  // TODO UNLINK FIFO
  exit(0);
}
