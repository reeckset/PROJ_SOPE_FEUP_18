#ifndef CLIENT_H
#define CLIENT_H

#include <semaphore.h>

typedef struct {
  int pid;
  int numWantedSeats;
  int numPreferredSeats;
  int *preferredSeats;
} Request;

typedef struct {
  int returnCode;
  int nAllocatedSeats;
  int* seats;
} Response;

char *createResponseFifo();

void writeRequestToFifo(const char *numWantedSeats, int numPreferredSeats,
                        const char *preferredSeats);

void readAndPrintReservedSeats(int fd, int fdLog, int fdCBook);

void processReturnCode(int code);

void readFromServer(int fdResponse, sem_t *sem);

void processReponse(char *responseFifoName);

char *getErrorCode(int error);

#endif /* end of include guard: CLIENT_H */
