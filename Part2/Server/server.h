#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include "utilities.h"
typedef int Seat;

typedef struct {
    Seat * seatList;
    int nSeats;
    int nOcuppiedSeats;
    FILE* fdServerFifo;
} TicketOfficeArgs;

typedef struct {
  int pid;
  int numWantedSeats;
  int numPreferredSeats;
  int *preferredSeats;
} Request;

void initServer(Input inputs);

void * initTicketOffice(void * ticketOfficeArgs);
FILE* initRequestsFifo();

void processClientMsg(TicketOfficeArgs* args);
void activateSignalHandler();



int isSeatFree(Seat* seats, int seatNum);
Seat* getRequestedSeats(Seat* seatList, Seat* requestedSeats, int nRequestedSeats, int minSeats, int pid);

void bookSeat(Seat* seats, int seatNum, int clientId);
void bookRequestedSeats(Seat* seats, Seat* requestedSeats, int nRequestSeats, int clientId);

void freeSeat(Seat* seats, int seatNum);

#endif
