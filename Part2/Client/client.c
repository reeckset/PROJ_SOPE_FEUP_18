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
#include "utilities.h"

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
  // TODO check open return
  int fdResquest = open(SERVER_FIFO, O_WRONLY);
  char *sstream = NULL;
  asprintf(&sstream, "%d %s %d %s\n", getpid(), numWantedSeats,
           numPreferredSeats, preferredSeats);

  write(fdResquest, sstream, strlen(sstream) + 1);
  free(sstream);
}

void readAndPrintReservedSeats(int fd, int fdLog, int fdCBook) {
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
      writeToLog(fdLog, "%0*d %0*d.%0*d %0*d\n", WIDTH_PID, getppid(),
                 WIDTH_XXNN / 2, i + 1, WIDTH_XXNN / 2, numReservedSeats,
                 WIDTH_SEAT, reservedSeat);
      writeToLog(fdCBook, "%0*d\n", WIDTH_SEAT, reservedSeat);
    }
  }
  printf("\n");
}

void processReturnCode(int code) { printf("Return code %d\n", code); }

void readFromServer(int fdResponse) {
  int returnCode;
  if (read(fdResponse, &returnCode, sizeof(int)) != sizeof(int)) {
    perror("Error reading server response");
    exit(READING_FIFO_ERROR);
  }
  int fdLog = open("./clog.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
  int fdCBook = open("./cbook.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);

  if (returnCode != 0) {
    processReturnCode(returnCode);
    char *errorCode = getErrorCode(returnCode);
    writeToLog(fdLog, "%0*d %s\n", WIDTH_PID, getppid(), errorCode);
    free(errorCode);
  } else {
    readAndPrintReservedSeats(fdResponse, fdLog, fdCBook);
  }
}

void processReponse(char *responseFifoName) {
  int fdResponse = open(responseFifoName, O_RDONLY);
  if (fdResponse == -1) {
    perror("Opening response FIFO");
    exit(FIFO_ERROR_EXIT);
  }
  readFromServer(fdResponse);
  unlink(responseFifoName);
  kill(getppid(), SIGKILL);
}

char *getErrorCode(int error) {
  char *result = (char *)malloc(3);
  switch (error) {
  case -1:
    sprintf(result, "MAX");
    break;
  case -2:
    sprintf(result, "NST");
    break;
  case -3:
    sprintf(result, "IID");
    break;
  case -4:
    sprintf(result, "ERR");
    break;
  case -5:
    sprintf(result, "NAV");
    break;
  case -6:
    sprintf(result, "FUL");
    break;
  }
  return result;
}
