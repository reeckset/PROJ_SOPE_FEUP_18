#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "client.h"
#include "macros.h"

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
  printf("Printing successfully reserved seats: ");
  for (i = 0; i < numReservedSeats; i++) {
    if (read(fd, &reservedSeat, sizeof(int)) != sizeof(int)) {
      perror("Error reading reserved seats");
      exit(READING_FIFO_ERROR);
    } else {
      printf("%d ", reservedSeat);
    }
  }
  printf("\n");

}

void processReturnCode(int code) {
  printf("Return code %d\n", code);
}

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

void processReponse(char *responseFifoName) {
  int fdResponse = open(responseFifoName, O_RDONLY);
  if(fdResponse == -1) {
    perror("Opening response FIFO");
    exit(FIFO_ERROR_EXIT);
  }
  readFromServer(fdResponse);
  kill(getppid(),SIGKILL);
}
