#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "macros.h"
#include "server.h"

bool g_tickets_are_open = true;
pthread_mutex_t readRequestsMutex = PTHREAD_MUTEX_INITIALIZER;

void closeTicketOfficesSignalHandler(int signo) { g_tickets_are_open = false; }

void initServer(Input inputs) {

  int fdServerFifo = initRequestsFifo();

  TicketOfficeArgs *ticketOfficeArgs =
      (TicketOfficeArgs *)malloc(sizeof(TicketOfficeArgs));

  Seat *seatList = (Seat *)calloc(inputs.nSeats, sizeof(Seat));

  ticketOfficeArgs->seatList = seatList;
  ticketOfficeArgs->nSeats = inputs.nSeats;
  ticketOfficeArgs->nOcuppiedSeats = 0;
  ticketOfficeArgs->fdServerFifo = fdServerFifo;

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
    pthread_create(&tids[i], NULL, initTicketOffice, (void *)ticketOfficeArgs);
    printf("Thread %lu\n", tids[i]);
  }

  void *retVal;
  for (int i = 0; i < inputs.nTicketOffices; i++) {
    pthread_join(tids[i], &retVal);
    printf("Thread joined %lu\n", tids[i]);
  }
  free(seatList);
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
}

void *initTicketOffice(void *ticketOfficeArgs) {
  TicketOfficeArgs *args = (TicketOfficeArgs *)ticketOfficeArgs;

  while (g_tickets_are_open) {
    processClientMsg(args);
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

void processClientMsg(TicketOfficeArgs *args) {

  pthread_mutex_lock(&readRequestsMutex);

  Request request;
  char selectedSeats[MAX_SEATS_STRING_SIZE];
  printf("%lu waiting\n", pthread_self());
  /*
  int ret = fscanf(args->fdServerFifo, "%d %d %d %[^\n]", &request.pid,
             &request.numWantedSeats, &request.numPreferredSeats,
             selectedSeats);

  if (ret != 4 ) {
    //printf("%d\n", ret);
    pthread_mutex_unlock(&readRequestsMutex);
    return;
  } else if (ret == 4){
    printf("%d\n", ret);
  } else if (ret == 0){
    printf("%d\n", ret);
  }*/

  int n = 0;
  char c;
  char *oldBuffer = NULL;
  char *buffer = NULL;

  while ((n = read(args->fdServerFifo, &c, sizeof(c))) == 1 && c != '\n') {

    oldBuffer = buffer;
    buffer = NULL;
    if(oldBuffer == NULL) {
      asprintf(&buffer, "%c", c);
    } else {
      asprintf(&buffer, "%s%c", oldBuffer, c);
    }

    free(oldBuffer);

  }


  if (n == 0 || n == -1) {
    pthread_mutex_unlock(&readRequestsMutex);
    return;
  }

  int ret = sscanf(buffer, "%d %d %d %[^\n]", &request.pid,
             &request.numWantedSeats, &request.numPreferredSeats,
             selectedSeats);


  free(buffer);

  printf("Received %lu: %d %d %d %s\n", pthread_self(), request.pid,
         request.numWantedSeats, request.numPreferredSeats, selectedSeats);
  pthread_mutex_unlock(&readRequestsMutex);

  // parse string selectedSeats to seat array
  int preferredSeatsSize;
  request.preferredSeats = stringToIntArray(
      selectedSeats, "Parsing Desired Seat List", &preferredSeatsSize);

  // ver disponibilidade de lugares
  // se disponivel, alocar
  // int *requestedSeatsResult = getRequestedSeats(
  //     args->seatList, request.preferredSeats, preferredSeatsSize,
  //     request.numWantedSeats, request.pid);
  //
  // if (requestedSeatsResult != NULL) { // Seats are available
  //   args->nOcuppiedSeats += request.numWantedSeats;
  //
  //   // TODO server response
  //   free(requestedSeatsResult);
  // } else {
  //   // TODO server response
  // }
  // free(request.preferredSeats);
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

  FILE *fServerFifo = fdopen(fdServerFifo, "r");

  return fdServerFifo;
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
}

sem_t *get_seat_semaphore(int seat) {
  sem_t *sem;
  char *buffer = NULL;
  asprintf(&buffer, "/sem%d", seat);
  if ((sem = sem_open(buffer, O_CREAT, S_IWOTH, 1)) == SEM_FAILED) {
    perror("Opennig semaphore error");
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
