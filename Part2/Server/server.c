#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "server.h"
#include "macros.h"

bool g_tickets_are_open = true;
pthread_mutex_t readRequestsMutex = PTHREAD_MUTEX_INITIALIZER;

void closeTicketOfficesSignalHandler(int signo) {
  g_tickets_are_open = false;
}

void initServer(Input inputs) {

  FILE* fdServerFifo = initRequestsFifo();

  TicketOfficeArgs * ticketOfficeArgs = (TicketOfficeArgs *) malloc(sizeof(TicketOfficeArgs));

  Seat seatList[inputs.nSeats] = {0};

  ticketOfficeArgs->seatList = seatList;
  ticketOfficeArgs->nSeats = inputs.nSeats;
  ticketOfficeArgs->nOcuppiedSeats = 0;
  ticketOfficeArgs->fdServerFifo = fdServerFifo;

  activateSignalHandler();

  pthread_t * tids = (pthread_t*) malloc(inputs.nTicketOffices * sizeof(pthread_t));

  int i;
  for(i = 0; i < inputs.nTicketOffices; i++) {
    pthread_create(&tids[i], NULL, initTicketOffice, (void *) ticketOfficeArgs);
  }

  void * retVal;
  pthread_exit(retVal);
}


void * initTicketOffice(void * ticketOfficeArgs) {
  TicketOfficeArgs* args = (TicketOfficeArgs*) ticketOfficeArgs;



  while(g_tickets_are_open) {
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

void processClientMsg(TicketOfficeArgs* args) {
  //BLOQUEAR A LEITURA ANTES
  pthread_mutex_lock(&readRequestsMutex);


  //TODO LER MSG (VER ESTRUTURA DA MSG (PROTOCOLO DE COMUNICAÇAO))
  Request request;
  char selectedSeats[MAX_SEATS_STRING_SIZE];
  //tentar ler o fifo requests (ver se tem cenas para ler)
  //se tiver, processar msg -> atualizando args (altera Seats)
  fscanf(args->fdServerFifo, "%d %d %d %[^\n]\n", &request.pid, &request.numWantedSeats, &request.numPreferredSeats, selectedSeats);

  //LIBERTAR O FIFO
  pthread_mutex_unlock(&readRequestsMutex);

  //parse string selectedSeats to seat array
  int preferredSeatsSize;
  request.preferredSeats = stringToIntArray(selectedSeats, "Parsing Desired Seat List", &preferredSeatsSize);

  //LOCK PARA VER DISPONIBILDADE (ALLOCATION MUTEX)

  //ver disponibilidade de lugares
  //se disponivel, alocar
  Seat* requestedSeatsResult = getRequestedSeats(args->seatList, request.preferredSeats, preferredSeatsSize, request.numWantedSeats);
  if(requestedSeatsResult != NULL) { // Seats are available
    bookRequestedSeats(args->seatList, requestedSeatsResult, request.numWantedSeats, request.pid);
  } else {
    //se nao, UNLOCK DE ALLOCATION MUTEX
  }


}

FILE* initRequestsFifo() {
  if (mkfifo(SERVER_FIFO, 0660) == -1) {
    perror("Error making the server's fifo");
    exit(FIFO_ERROR_EXIT);
  }

  int fdServerFifo = open(SERVER_FIFO, O_RDONLY);
  if(fdServerFifo == -1) {
    perror("Error opening server FIFO.");
    exit(FIFO_ERROR_EXIT);
  }

  FILE* fServerFifo = fdopen(fdServerFifo, "r");

  return fServerFifo;
}


int isSeatFree(Seat* seats, int seatNum) {
  return (seats[seatNum] == 0);
}

Seat* getRequestedSeats(Seat* seatList, Seat* requestedSeats, int nRequestedSeats, int minSeats) {
  Seat* requestResult = (Seat*) malloc(minSeats * sizeof(Seat));
  int reservedSeatsCounter = 0;

  int i;
  for(i = 0; i < nRequestedSeats; i++ ) {

    if(reservedSeatsCounter == minSeats) {
      break;
    }

    if(isSeatFree(seatList, requestedSeats[i]) ) {
      requestResult[reservedSeatsCounter] = requestedSeats[i];
      reservedSeatsCounter++;
    }
  }

  if(reservedSeatsCounter < minSeats) {
    return NULL;
  } else {
    return requestResult;
  }


}

void bookSeat(Seat* seats, int seatNum, int clientId) {
  seats[seatNum] = clientId;
}

void bookRequestedSeats(Seat* seats, Seat* requestedSeats, int nRequestSeats, int clientId) {
  int i;
  for(i = 0; i < nRequestSeats; i++) {
    bookSeat(seats, requestedSeats[i], clientId);
  }
}
