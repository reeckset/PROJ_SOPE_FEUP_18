#ifndef CLIENT_H
#define CLIENT_H

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

void readAndPrintReservedSeats(int fd);

void processReturnCode(int code);

void readFromServer(int fdResponse);

void processReponse(char *responseFifoName);

#endif /* end of include guard: CLIENT_H */