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

typedef struct {
  int pid;
  int numWantedSeats;
  int numPreferredSeats;
  int *preferredSeats;
} Request;

void printfIntArray(int *array, int size) {
  int i;
  for (i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("\n");
}

void gracefulShutdownOnTimeout(int timeout) {
  int sleepTime = sleep(timeout);
  kill(0, SIGKILL);
  // TODO BEFORE KILLING CHILD, CHECK IF FIFO IS EMPTY TO AVOID INFORMATION LOSS
  // EITHER DO FSEEK
}

char *createResponseFifo() {
  char *fifoName = NULL;
  asprintf(&fifoName, "/tmp/ans%d", getpid());
  if (mkfifo(fifoName, 0660) == -1) {
    perror("Error making the client's fifo");
    exit(FIFO_ERROR_EXIT);
  }
  return fifoName;
}

void writeRequestToFifo(const char *numWantedSeats, int numPreferredSeats,
                        const char *preferredSeats) {
  int fdResquest = open(SERVER_FIFO, O_WRONLY);
  char *sstream = NULL;
  asprintf(&sstream, "%d %s %d %s\n", getpid(), numWantedSeats,
           numPreferredSeats, preferredSeats);

  write(fdResquest, sstream, strlen(sstream) + 1);
  free(sstream);
}

void readAndPrintReservedSeats(int fd) {
  int numReservedSeats, i, reservedSeat;
  if (read(fd, &numReservedSeats, sizeof(int)) != sizeof(int)) {
    perror("Error reading number of reserved seats");
    exit(READING_FIFO_ERROR);
  }
  for (i = 0; i < numReservedSeats; i++) {
    printf("Printing successfully reserved seats: ");
    if (read(fd, &reservedSeat, sizeof(int)) != sizeof(int)) {
      perror("Error reading reserved seats");
      exit(READING_FIFO_ERROR);
    } else {
      printf("%d ", reservedSeat);
    }
    printf("\n");
  }
}

void processReturnCode(int code) {}

void readFromServer(int fdResponse) {
  int returnCode;
  if (read(fdResponse, &returnCode, sizeof(int)) != sizeof(int)) {
    perror("Error reading server response");
    exit(READING_FIFO_ERROR);
  }
  if (returnCode != 0) {
    processReturnCode(returnCode);
  } else {
    readAndPrintReservedSeats(fdResponse);
  }
}

void sendRequest(const char **argv) {
  writeRequestToFifo(argv[2], getIntAmount(argv[3]), argv[3]);
}

void attemptSend(const char **argv, int timeout) {
  char *responseFifoName = createResponseFifo();
  sendRequest(argv);
  int pid = fork();
  if (pid < 0) {
    perror("");
    exit(FORK_ERROR_EXIT);
  }
  if (pid == 0) { // filho
    int fdResponse = open(responseFifoName, O_RDONLY);
    readFromServer(fdResponse);
  } else { // pai
    gracefulShutdownOnTimeout(timeout);
  }
}

void populateRequest(Request *request, const char **argv) {
  request->numWantedSeats =
      checkValidIntArgument(argv[2], "Number of wanted seats");

  request->preferredSeats =
      stringToIntArray(argv[3], "preferred seats", &request->numPreferredSeats);
  printfIntArray(request->preferredSeats, request->numPreferredSeats);
}

int main(int argc, char const *argv[]) {
  checkArgumentAmount(argc, 4,
                      "client <time_out> <num_wanted_seats> <pref_seat_list>");
  int timeout = checkValidIntArgument(argv[1], "Time Out");

  Request request;
  populateRequest(&request, argv);

  attemptSend(argv, timeout);

  printf("Time out: %d, Number of wanted seats: %d\n", timeout,
         request.numWantedSeats);

  free(request.preferredSeats);
  // TODO UNLINK FIFO
  exit(0);
}
