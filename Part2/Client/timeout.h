#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <semaphore.h>

void gracefulShutdownOnTimeout(int timeout, int pid);
sem_t *get_client_fifo_semaphore(int pid);


#endif /* end of include guard: TIMEOUT_H */
