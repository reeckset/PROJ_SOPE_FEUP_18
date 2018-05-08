#ifndef MACROS_H
#define MACROS_H
#endif

#define SUCCESSFULL_EXIT 0
#define ILLEGAL_ARG_EXIT 1
#define INVALID_STR2INT_ARRAY_EXIT 2
#define FORK_ERROR_EXIT 3
#define FIFO_ERROR_EXIT 4
#define FORK_WAIT_ERROR 5

#define SERVER_FIFO "/tmp/requests"

#define MAX_ROOM_SEATS 9999
#define MAX_CLI_SEATS 99
#define MAX_OPEN_TIME 300
#define MAX_TICKET_OFFICES 8
#define WIDTH_PID 5
#define WIDTH_XXNN 5
#define WIDTH_SEAT 4

#define min(a,b) ((a<b) ? a : b)
#define MAX_SEATS_STRING_SIZE 4 * MAX_CLI_SEATS + MAX_CLI_SEATS - 1 //4 chars * 99 + 98 spaces
