#include <pthread.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

#include "server.h"

bool g_tickets_are_open = true;

void closeTicketOfficesSignalHandler(int signo) {
  g_tickets_are_open = false;
}

void initServer(Input inputs) {
  TicketOfficeArgs * ticketOfficeArgs = (TicketOfficeArgs *) malloc(sizeof(TicketOfficeArgs));

  ticketOfficeArgs->seatList = (Seat *) malloc(inputs.nSeats * sizeof(Seat));
  ticketOfficeArgs->nSeats = inputs.nSeats;
  ticketOfficeArgs->nOcuppiedSeats = 0;

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
  //tentar ler o fifo requests (ver se tem cenas para ler)
  if (mkfifo(SERVER_FIFO, 0660) == -1) {
    perror("Error making the server's fifo");
    exit(FIFO_ERROR_EXIT);
  }
  open(SERVER_FIFO, O_RDONLY)
  //se tiver, processar msg -> atualizando args (altera Seats)
  //LIBERTAR O FIFO


}
