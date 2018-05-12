#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <semaphore.h>

#include "utilities.h"
typedef int Seat;

typedef struct {
  Seat *seatList;
  int nSeats;
  int nOccupiedSeats;
  int fdServerFifo;
  int fdLog;
  int id;
} TicketOfficeArgs;

typedef struct {
  int pid;
  int numWantedSeats;
  int numPreferredSeats;
  int *preferredSeats;
  int error;
} Request;

typedef struct {
  int returnCode;
  int nAllocatedSeats;
  int* seats;
} Response;

void initServer(Input inputs);

void *initTicketOffice(void *ticketOfficeArgs);
int initRequestsFifo();

void processClientMsg(TicketOfficeArgs *args);
void activateSignalHandler();

void handleRequest(TicketOfficeArgs *args, Request request,
                   int preferredSeatsSize);

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
sem_t *get_client_fifo_semaphore(int pid);

void verifyRequestErrors(Request* request, TicketOfficeArgs* args, int preferredSeatsSize);

void sendResponse(Response response, int pid, TicketOfficeArgs *args);

void writeToLog(int fd, char *format, ...);

char *getErrorCode(int error);

TicketOfficeArgs *createTicketOfficeArgs(int ticketOfficeNum, int nSeats,
                                         Seat *seatList, int fdLog,
                                         int fdServerFifo);

#endif
