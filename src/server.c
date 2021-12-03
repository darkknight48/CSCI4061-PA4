#include "server.h"

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

void write_to_log_file(){}

/*A worker thread will parse each query 
    received and reply with the appropriate response. If it modifies the global balance 
    datastructure, it should signal the log thread’s condition variable. This will continue 
    until it receives a TERMINATE query from the client. It will then close the 
    connection and return.*/
void pass_to_worker(){}

//msg_enum e = REGISTER;

const char* getMsgEnum(enum msg_enum msg) 
{
   switch (msg) 
   {
       case REGISTER: return "REGISTER";
       case GET_ACCOUNT_INFO: return "GET_ACCOUNT_INFO";
       case TRANSACT: return "TRANSACT";
       case GET_BALANCE: return "GET_BALNCE";
       case ACCOUNT_INFO: return "ACCOUNT_INFO";
       case BALANCE: return "BALANCE";
       case REQUEST_CASH: return "";
       case CASH: return "CASH";
       case ERROR: return "ERROR";
       case TERMINATE: return "TERMINATE";
       case REQUEST_HISTORY: return "REQUEST_HISTORY";
       case HISTORY: return "HISTORY";
   }
}


int main(int argc, char *argv[]){
    // argument handling
    if(argc != 4)
    {
        printSyntax();
        return 0;
    }

    // create empty output folder
    bookeepingCode();

    // start a log thread, wait 5 seconds, and write a log to a file
    write_to_log_file();

    // create a socket and begin listening on it

    /*For each incoming connection, the server will create a worker thread 
    which will handle the connection (pass it the connection’s file descriptor) */
    pass_to_worker(fileDesc);

    //and return to listening on the socket.

    return 0; 
}

