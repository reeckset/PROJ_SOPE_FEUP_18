#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <semaphore.h>

#include "utilities.h"
typedef int Seat;

typedef struct {
  Seat *seatList;
  int nSeats;
  int nOcuppiedSeats;
  int fdServerFifo;
} TicketOfficeArgs;

typedef struct {
  int pid;
  int numWantedSeats;
  int numPreferredSeats;
  int *preferredSeats;
} Request;

void initServer(Input inputs);

void *initTicketOffice(void *ticketOfficeArgs);
int initRequestsFifo();

void processClientMsg(TicketOfficeArgs *args);
void activateSignalHandler();

int isSeatFree(Seat *seats, int seatNum);
int *getRequestedSeats(Seat *seatList, Seat *requestedSeats,
                       int nRequestedSeats, int minSeats, int pid);

void bookSeat(Seat *seats, int seatNum, int clientId);
void bookRequestedSeats(Seat *seats, int *requestedSeats, int nRequestSeats,
                        int clientId);

void freeSeat(Seat *seats, int seatNum);
void freeSeats(Seat *seats, Seat *requestResult, int sizeOfRequestResult);

void allocSeat(Seat *seatList, Seat seatToAlloc, int pid,
               int *reservedSeatsCounter, Seat *requestResult);

sem_t *get_seat_semaphore(int seat);

#endif
