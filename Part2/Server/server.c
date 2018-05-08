#include <pthread.h>
#include <stdlib.h>

#include "server.h"

bool g_tickets_are_open = true;

void closeTicketOfficesSignalHandler() {
  g_tickets_are_open = false;
}

void initServer(Input inputs) {
  TicketOfficeArgs * ticketOfficeArgs = (TicketOfficeArgs *) malloc(sizeof(TicketOfficeArgs));

  ticketOfficeArgs->seatList = (Seat *) malloc(inputs.nSeats * sizeof(Seat));



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

  Seat* seatList = args->seatList;

  while(g_tickets_are_open) {
    processClientMsg();
  }

}
