#include "utils.h"

void _removeOutputDir()
{
    pid_t pid = fork();
    if(pid == 0)
    {
        char *argv[] = {"rm", "-rf", "output", NULL};
        if (execvp(*argv, argv) < 0)
        {
            printf("ERROR: exec failed\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    } else{
        wait(NULL);
    }
}

void _createOutputDir()
{
    mkdir("output", ACCESSPERMS);
}

void bookeepingCode()
{
    _removeOutputDir();
    sleep(1);
    _createOutputDir();
}

char* getMsgEnum(msg_enum msg) 
{
   switch (msg) 
   {
       case REGISTER: return "REGISTER";
       case GET_ACCOUNT_INFO: return "GET_ACCOUNT_INFO";
       case TRANSACT: return "TRANSACT";
       case GET_BALANCE: return "GET_BALNCE";
       case ACCOUNT_INFO: return "ACCOUNT_INFO";
       case BALANCE: return "BALANCE";
       case REQUEST_CASH: return "REQUEST_CASH";
       case CASH: return "CASH";
       case ERROR: return "ERROR";
       case TERMINATE: return "TERMINATE";
       case REQUEST_HISTORY: return "REQUEST_HISTORY";
       case HISTORY: return "HISTORY";
   }
}

//void writeMessage(int, struct message){}
//bool readMessage(int, struct message *){}