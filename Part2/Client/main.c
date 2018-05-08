#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
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

void countTimeout(int timeout) {
  int sleepTime = sleep(timeout);
  if (sleepTime != 0) {
    kill(0, SIGKILL);
  }
}

char *createClientFifo(Request *request) {
  char *fifoName = NULL;
  request->pid = getpid();
  asprintf(&fifoName, "/tmp/ans%d", request->pid);
  if (mkfifo(fifoName, 0660) == -1) {
    perror("Error making the client's fifo");
    exit(FIFO_ERROR_EXIT);
  }
  return fifoName;
}

void writeRequestToFifo(Request *request, int fileDescriptor) {
  // TODO SECCAO CRITICA

  write(fileDescriptor, &(request->pid), sizeof(int));
  write(fileDescriptor, &(request->numWantedSeats), sizeof(int));
  write(fileDescriptor, &(request->numPreferredSeats), sizeof(int));
  int i;
  for (i = 0; i < request->numPreferredSeats; i++) {
    write(fileDescriptor, request->preferredSeats + i, sizeof(int));
  }

  // TODO FIM DA SECCAO CRITICA
}

void sendRequest(Request request) {
  char *clientFifoName = createClientFifo(&request);

  int fdServer = open(SERVER_FIFO, O_WRONLY);
  int fdClient = open(clientFifoName, O_RDONLY);

  writeRequestToFifo(&request, fdClient);

  //  readFromServer();
}

void attemptSend(Request request, int timeout) {
  int pid = fork();
  if (pid < 0) {
    perror("");
    exit(FORK_ERROR_EXIT);
  }
  if (pid == 0) { // filho
    sendRequest(request);
  } else { // pai
    countTimeout(timeout);
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

  attemptSend(request, timeout);

  printf("Time out: %d, Number of wanted seats: %d\n", timeout,
         request.numWantedSeats);

  free(request.preferredSeats);
  //TODO UNLINK FIFO
  exit(0);
}
