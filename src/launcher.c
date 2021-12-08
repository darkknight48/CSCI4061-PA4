#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    // fork and exec the server
    pid_t server_pid;
    if ((server_pid=fork()) == 0)
    {
        execl("server", "server", argv[1], argv[2], argv[3], NULL);
    }
    else if (server_pid < 0)
    {
        printf("Failed to launch server.\n");
        exit(EXIT_FAILURE);
    }
    
    // sleep to let the server start
    sleep(4);

    unsigned char c = 4;
    char name[10];
    pid_t client_pid;
    // fork and exec for each input file
    while (c < argc)
    { 
        if ((client_pid=fork()) == 0)
        {
            sprintf(name, "client%d", c);
            execl("client", name, argv[c], argv[1], argv[2], NULL);
        }
        else if (server_pid < 0)
        {
            printf("Failed to launch server.\n");
            exit(EXIT_FAILURE);
        }
        // sleep to allow the registration messages to be delivered
        // to prevent a race condition with the transactions
        if (c == 4)
        {
            sleep(3);
        }
        c++;
    }

    // wait on the clients but not the server
    for (int i = 4; i < argc; i++)
    {
        if (wait(NULL)==-1)
        {
            printf("Failed to wait on client.\n");
            exit(EXIT_FAILURE);
        }
    }

    sleep(6);

    // send terminate signal to the server
    if((kill(server_pid, SIGTERM))== -1)
    {
        printf("Failed to SIGTERM server.\n");
        exit(EXIT_FAILURE);
    }
    return 0; 
}

