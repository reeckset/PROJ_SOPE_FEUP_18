#ifndef SERVER_H
#define SERVER_H

#include "utilities.h"
typedef int Seat;

typedef struct {
    Seat * seatList;
    int nSeats;
    int nOcuppiedSeats;
} TicketOfficeArgs;

void initServer(Input inputs);

void * initTicketOffice(void * ticketOfficeArgs);

void processClientMsg(TicketOfficeArgs* args);
void activateSignalHandler();





int isSeatFree(Seat* seats, int seatNum);

void bookSeat(Seat* seats, int seatNum, int clientId);

void freeSeat(Seat* seats, int seatNum);

#endif
