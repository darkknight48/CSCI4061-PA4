#ifndef UTILS_H
#define UTILS_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_STR 64
#define MAX_ACC 1023
#define CASH_AMOUNT 100000
#define START_CASH 10000
#define LOGGER_SLEEP 5

/*  DO NOT MODIFY */
typedef enum
{
	REGISTER,         // 0
	GET_ACCOUNT_INFO, // 1
	TRANSACT,         // 2
	GET_BALANCE,      // 3 
	ACCOUNT_INFO,     // 4
	BALANCE,          // 5
	REQUEST_CASH,     // 6
	CASH,             // 7
	ERROR,            // 8
	TERMINATE,        // 9
	/* extra credit messages */
	REQUEST_HISTORY,  // 10
	HISTORY,          // 11
}msg_enum;

struct message
{
    msg_enum msg_type;
    msg_enum err_type;
    char name[MAX_STR];
    char username[MAX_STR];
    time_t birthday;
    int acc_num;
    float cash;
    int num_tx;
    float *history;
};

void writeMessage(int, struct message);
bool readMessage(int, struct message *);

void bookeepingCode();

#endif

