#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "macros.h"
#include "server.h"

bool g_tickets_are_open = true;
pthread_mutex_t ticketOfficeMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t readRequestsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t updateOccupiedMutex = PTHREAD_MUTEX_INITIALIZER;
int nCharactersOfPreferredSeats;
int nDigitsOfTicketOfficeId;
int nDigitsNSeats;

void closeTicketOfficesSignalHandler(int signo) {
  pthread_mutex_lock(&ticketOfficeMutex);
  g_tickets_are_open = false;
  pthread_mutex_unlock(&ticketOfficeMutex);
  int fdServerFifo = open(SERVER_FIFO, O_WRONLY);
  if (fdServerFifo == -1) {
    perror("Error opening server FIFO.");
    exit(FIFO_ERROR_EXIT);
  }
  write(fdServerFifo, "*\n", 3);
}

void getNumberOfDigitsValues(int nSeats, int nTicketOffices) {
  nCharactersOfPreferredSeats = (WIDTH_SEAT + 1) * MAX_CLI_SEATS;
  nDigitsOfTicketOfficeId = getNumberOfDigits(nTicketOffices);
  nDigitsNSeats = getNumberOfDigits(MAX_CLI_SEATS);
}

void clearClientLog() {
  unlink("../Client/clog.txt");  // clear client log
  unlink("../Client/cbook.txt"); // clear client book
}

void initServer(Input inputs) {

  getNumberOfDigitsValues(inputs.nSeats, inputs.nTicketOffices);

  int fdServerFifo = initRequestsFifo();

  int fdLog = open("./slog.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

  clearClientLog();

  Seat *seatList = (Seat *)calloc(inputs.nSeats + 1, sizeof(Seat));

  activateSignalHandler();
  // TODO ERASE
  for (int i = 0; i < inputs.nSeats; i++) {
    char *buffer = NULL;
    asprintf(&buffer, "/sem%d", i);
    if (sem_unlink(buffer) != 0) {
      if (errno != ENOENT) {
        perror(buffer);
        exit(SEMAPHORE_ERROR);
      }
    }
    free(buffer);
  }

  pthread_t *tids =
      (pthread_t *)malloc(inputs.nTicketOffices * sizeof(pthread_t));

  int i;
  for (i = 0; i < inputs.nTicketOffices; i++) {
    printf("Created %d\n", i);
    TicketOfficeArgs *ticketOfficeArgs =
        createTicketOfficeArgs(i, inputs.nSeats, seatList, fdLog, fdServerFifo);
    pthread_create(&tids[i], NULL, initTicketOffice, (void *)ticketOfficeArgs);
    writeToLog(fdLog, "%0*d-OPEN\n", nDigitsOfTicketOfficeId, i);
    printf("Thread %lu\n", tids[i]);
  }

  void *retVal;
  for (int i = 0; i < inputs.nTicketOffices; i++) {
    pthread_join(tids[i], &retVal);
    writeToLog(fdLog, "%0*d-CLOSED\n", nDigitsOfTicketOfficeId, i);
    printf("Thread joined %lu\n", tids[i]);
  }
  writeToLog(fdLog, "SERVER CLOSED\n");
  close(fdLog);
  writeSBook(seatList, inputs.nSeats);
  if (close(fdServerFifo) != 0) {
    perror("Close fifo error");
    exit(FIFO_ERROR_EXIT);
  } else {
    unlink(SERVER_FIFO);
  }

  for (int i = 0; i < inputs.nSeats; i++) {
    char *buffer = NULL;
    asprintf(&buffer, "/sem%d", i);
    if (sem_unlink(buffer) != 0) {
      if (errno != ENOENT) {
        perror(buffer);
        exit(SEMAPHORE_ERROR);
      }
    }
    free(buffer);
  }

  free(seatList);
  free(tids);
}

void *initTicketOffice(void *ticketOfficeArgs) {
  TicketOfficeArgs *args = (TicketOfficeArgs *)ticketOfficeArgs;

  while (true) {
    pthread_mutex_lock(&ticketOfficeMutex);

    if (g_tickets_are_open) {
      pthread_mutex_unlock(&ticketOfficeMutex);

      processClientMsg(args);
    } else {
      pthread_mutex_unlock(&ticketOfficeMutex);

      break;
    }
  }

  return NULL;
}

void activateSignalHandler() {
  struct sigaction oldsa;
  struct sigaction sa;
  sa.sa_handler = &closeTicketOfficesSignalHandler;

  if (sigaction(SIGUSR1, &sa, &oldsa) < 0) {
    perror("SIGUSR1 handler error");
    exit(1);
  }
}

char *getRequest(int fdServerFifo) {
  int n = 0;
  char c;
  char *oldBuffer = NULL;
  char *buffer = NULL;
  while ((n = read(fdServerFifo, &c, sizeof(c))) == 1 && c != '\n') {

    oldBuffer = buffer;
    buffer = NULL;
    if (oldBuffer == NULL) {
      asprintf(&buffer, "%c", c);
    } else {
      asprintf(&buffer, "%s%c", oldBuffer, c);
    }

    free(oldBuffer);
  }

  return buffer;
}

int openResponseFifo(int pid) {
  char *fifoName = NULL;
  asprintf(&fifoName, "/tmp/ans%d", pid);
  int fdResponse = open(fifoName, O_WRONLY);
  // if (fdResponse == -1) {
  //   perror("Opening response FIFO");
  // }
  return fdResponse;
}

void sendResponse(Response response, int pid, TicketOfficeArgs *args,
                  Request request) {
  sem_t *sem = get_client_fifo_semaphore(pid);
  sem_wait(sem);

  int fdResponse = openResponseFifo(pid);
  if (fdResponse == -1) {
    if (response.returnCode == 0)
      freeSeats(args->seatList, response.seats, response.nAllocatedSeats);
    return;
  }
  write(fdResponse, &response.returnCode, sizeof(int));
  if (response.returnCode == 0) {
    write(fdResponse, &response.nAllocatedSeats, sizeof(int));
    for (int i = 0; i < response.nAllocatedSeats; i++) {
      write(fdResponse, &response.seats[i], sizeof(int));
    }
  }

  char *preferredSeats = intArrayToString(
      request.preferredSeats, request.numPreferredSeats, WIDTH_SEAT);
  printf("%s\n", preferredSeats);
  if (response.returnCode == 0) {
    char *reservedSeats =
        intArrayToString(response.seats, response.nAllocatedSeats, WIDTH_SEAT);
    writeToLog(args->fdLog, "%0*d-%0*d-%0*d: %-*s- %s\n",
               nDigitsOfTicketOfficeId, args->id, WIDTH_PID, request.pid,
               nDigitsNSeats, request.numWantedSeats,
               nCharactersOfPreferredSeats, preferredSeats, reservedSeats);
    free(reservedSeats);
  } else {
    char *errorCode = getErrorCode(request.error);
    writeToLog(args->fdLog, "%0*d-%0*d-%0*d: %-*s- %s\n",
               nDigitsOfTicketOfficeId, args->id, WIDTH_PID, request.pid,
               nDigitsNSeats, request.numWantedSeats,
               nCharactersOfPreferredSeats, preferredSeats, errorCode);
    free(preferredSeats);
  }
  sem_post(sem);
  sem_close(sem);
}

void handleRequest(TicketOfficeArgs *args, Request request,
                   int preferredSeatsSize) {

  Response response;
  if (request.error) {
    response.returnCode = request.error;
    sendResponse(response, request.pid, args, request);
    return;
  }

  int *requestedSeatsResult = getRequestedSeats(
      args->seatList, request.preferredSeats, preferredSeatsSize,
      request.numWantedSeats, request.pid);

  if (requestedSeatsResult == NULL) {
    request.error = NAV;
  } else { // Seats are available
    pthread_mutex_lock(&updateOccupiedMutex);
    args->nOccupiedSeats += request.numWantedSeats;
    pthread_mutex_unlock(&updateOccupiedMutex);
  }

  response.returnCode = request.error;
  response.nAllocatedSeats = request.numWantedSeats;
  response.seats = requestedSeatsResult;
  sendResponse(response, request.pid, args, request);

  if (!request.error) {
    free(requestedSeatsResult);
  }
}

void processClientMsg(TicketOfficeArgs *args) {

  pthread_mutex_lock(&readRequestsMutex);
  pthread_mutex_lock(&ticketOfficeMutex);
  if (!g_tickets_are_open) {
    pthread_mutex_unlock(&ticketOfficeMutex);
    pthread_mutex_unlock(&readRequestsMutex);
    return;
  }
  pthread_mutex_unlock(&ticketOfficeMutex);

  Request request;
  request.error = AYE;
  char selectedSeats[MAX_SEATS_STRING_SIZE];

  char *requestMsg = getRequest(args->fdServerFifo);

  if (requestMsg == NULL || strcmp(requestMsg, "*") == 0) {
    pthread_mutex_unlock(&readRequestsMutex);
    return;
  }
  pthread_mutex_unlock(&readRequestsMutex);

  int ret = sscanf(requestMsg, "%d %d %d %[^\n]", &request.pid,
                   &request.numWantedSeats, &request.numPreferredSeats,
                   selectedSeats);
  if (ret != 4) {
    perror("Read message error");
    free(requestMsg);
    request.error = ERR;
    handleRequest(args, request, 0);
    return;
  }

  free(requestMsg);

  printf("Received %lu: %d %d %d %s\n", pthread_self(), request.pid,
         request.numWantedSeats, request.numPreferredSeats, selectedSeats);
  int preferredSeatsSize;

  // TODO dentro da stringtointarray verificar identificadores negativos
  request.preferredSeats =
      stringToIntArray(selectedSeats, "Parsing Desired Seat List",
                       &preferredSeatsSize, &request.error);

  verifyRequestErrors(&request, args, preferredSeatsSize);

  handleRequest(args, request, preferredSeatsSize);
  free(request.preferredSeats);
  free(args);
  printf("Finished Request\n");
}

int initRequestsFifo() {
  if (mkfifo(SERVER_FIFO, 0660) == -1) {
    perror("Error making the server's fifo");
    exit(FIFO_ERROR_EXIT);
  }

  int fdServerFifo = open(SERVER_FIFO, O_RDWR);
  if (fdServerFifo == -1) {
    perror("Error opening server FIFO.");
    exit(FIFO_ERROR_EXIT);
  }

  return fdServerFifo;
}

void verifyRequestErrors(Request *request, TicketOfficeArgs *args,
                         int preferredSeatsSize) {
  if (request->numWantedSeats > MAX_CLI_SEATS) {
    request->error = MAX;
    return;
  }
  if (preferredSeatsSize < request->numWantedSeats) {
    request->error = NST;
    return;
  }
  pthread_mutex_lock(&updateOccupiedMutex);
  if ((args->nSeats - args->nOccupiedSeats) < request->numWantedSeats) {
    request->error = FUL;
    pthread_mutex_unlock(&updateOccupiedMutex);
    return;
  }
  pthread_mutex_unlock(&updateOccupiedMutex);
}

int isSeatFree(Seat *seats, int seatNum) { return (seats[seatNum] == 0); }

int *getRequestedSeats(Seat *seatList, int *requestedSeats, int nRequestedSeats,
                       int minSeats, int pid) {
  int *requestResult = (int *)malloc(minSeats * sizeof(int));
  int reservedSeatsCounter = 0;
  sem_t *sem;

  int i;
  for (i = 0; i < nRequestedSeats; i++) {

    if (reservedSeatsCounter == minSeats) {
      break;
    }

    sem = get_seat_semaphore(requestedSeats[i]);
    sem_wait(sem);
    allocSeat(seatList, requestedSeats[i], pid, &reservedSeatsCounter,
              requestResult);
    sem_post(sem);
    sem_close(sem);
  }

  if (reservedSeatsCounter < minSeats) {
    freeSeats(seatList, requestResult, reservedSeatsCounter);
    return NULL;
  } else {
    return requestResult;
  }
}

void bookSeat(Seat *seats, int seatNum, int clientId) {
  seats[seatNum] = clientId;
}

void allocSeat(Seat *seatList, Seat seatToAlloc, int pid,
               int *reservedSeatsCounter, int *requestResult) {
  if (isSeatFree(seatList, seatToAlloc)) {
    requestResult[*reservedSeatsCounter] = seatToAlloc;
    bookSeat(seatList, seatToAlloc, pid);
    (*reservedSeatsCounter)++;
  }
  DELAY();
}

sem_t *get_seat_semaphore(int seat) {
  sem_t *sem;
  char *buffer = NULL;
  asprintf(&buffer, "/sem%d", seat);
  if ((sem = sem_open(buffer, O_CREAT, 0666, 1)) == SEM_FAILED) {
    perror("Opening semaphore error");
    exit(SEMAPHORE_ERROR);
  }
  free(buffer);
  return sem;
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

void freeSeat(Seat *seats, int seatNum) { seats[seatNum] = 0; }

void freeSeats(Seat *seats, Seat *requestResult, int sizeOfRequestResult) {
  for (int i = 0; i < sizeOfRequestResult; i++) {
    sem_t *sem = get_seat_semaphore(requestResult[i]);
    sem_wait(sem);
    freeSeat(seats, requestResult[i]);
    sem_post(sem);
    sem_close(sem);
  }
}

TicketOfficeArgs *createTicketOfficeArgs(int ticketOfficeNum, int nSeats,
                                         Seat *seatList, int fdLog,
                                         int fdServerFifo) {
  TicketOfficeArgs *ticketOfficeArgs =
      (TicketOfficeArgs *)malloc(sizeof(TicketOfficeArgs));

  ticketOfficeArgs->seatList = seatList;
  ticketOfficeArgs->nSeats = nSeats;
  ticketOfficeArgs->nOccupiedSeats = 0;
  ticketOfficeArgs->fdServerFifo = fdServerFifo;
  ticketOfficeArgs->fdLog = fdLog;
  ticketOfficeArgs->id = ticketOfficeNum;

  return ticketOfficeArgs;
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

void writeSBook(Seat *seatList, int nSeats) {
  int i;
  int fdSBook = open("./sbook.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);
  for (i = 0; i < nSeats; i++) {
    if (!isSeatFree(seatList, i)) {
      writeToLog(fdSBook, "%0*d\n", WIDTH_SEAT, i);
    }
  }
  close(fdSBook);
}
