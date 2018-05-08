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

  ticketOfficeArgs->seatList = (Seat *) malloc(inputs.nSeats * sizeof(Seat));
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
  fscanf(args->fdServerFifo, "%d %d %d %[^\n]\n", &request.pid, &request.numWantedSeats, &request.numPreferredSeats, selectedSeats);

  //LIBERTAR O FIFO
  pthread_mutex_unlock(&readRequestsMutex);


  //parse string selectedSeats to seat array

  //se tiver, processar msg -> atualizando args (altera Seats)


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
