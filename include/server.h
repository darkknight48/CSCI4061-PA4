#ifndef SERVER_H
#define SERVER_H

#include "utils.h"
#include <pthread.h>
#include <semaphore.h>

// skeleton of a struct to hold account information
// this should contain information passed with each 
// new account that is registered (see REGISTER in 
// the writeup...)
// 
// add any other fields you feel are necessary to 
// for implementing the extra credit history or
// for maintaining synchronization among threads
struct account
{
    char username[MAX_STR];
    char name[MAX_STR];
    time_t birthday;
    float balance;
    pthread_mutex_t lock; // PTHREAD_MUTEX_INITIALIZER;
};

// array of structs to store account information
struct account balances[MAX_ACC];
// ALT: you may declare it as a pointer and malloc
// struct account * balances;
extern sem_t sem_worker;
extern sem_t sem_main;


void printSyntax();

#endif

