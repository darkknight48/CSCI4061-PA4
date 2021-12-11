#include "server.h"

//#define LOCALHOST "127.0.0.1"
#define MAX 516
//#define PORT 9333
#define SA struct sockaddr


void write_to_log_file(){
    //iterate over every account in balances[] to log account info to balances.csv
    //format: account number,balance,name,username,birthday  (”%d,%.2f,%s,%s,%ld\n”)
    
    char *balancesFile = "output/balances.csv";
    FILE *fp = fopen(balancesFile, "w");   //write to finalDir

    if(fp == NULL){
        printf("ERROR: failed to create %s\n", balancesFile);
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < MAX_ACC; i++){
        char temp[1024];

        //lock
        pthread_mutex_lock(&balances[i].lock);

        sprintf(temp, ”%d,%.2f,%s,%s,%ld\n”, i, balances[i].balance, balances[i].name, balances[i].username, balances[i].birthday);
        
        //unlock
        pthread_mutex_unlock(&balances[i].lock);
        
        fputs(temp, fp);
    }

    fclose(fp);
}

/*A worker thread will parse each query 
    received and reply with the appropriate response. If it modifies the global balance 
    datastructure, it should signal the log thread’s condition variable. This will continue 
    until it receives a TERMINATE query from the client. It will then close the 
    connection and return.*/
void worker_thread(void* arg)
{
    int connfd = *(int *)arg;
    int amt;
    while(1){
        msg_enum msg_type;
        if((amt=read(connfd, &msg_type, sizeof(msg_enum))) != sizeof(msg_enum))
        {
            printf("worker failed to read msg_type\n.");
            printf("It read %d bytes\n.", amt);
            exit(1);
        }
        switch (msg_type){
            case REGISTER :
                char username[MAX_STR];
                char name[MAX_STR];
                time_t birthday;
                if((amt=read(connfd, &username, sizeof(char)*MAX_STR)) < 1)
                {
                    printf("worker failed to read username\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                //write the name to client
                char netNm[MAX_STR] = htonl(name);
                if((amt=write(sock_fd, &netNm, sizeof(char)*MAX_STR)) < 1)
                {
                    printf("registrate failed to write name\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                //write the birthday to client
                time_t netBirthday = htonl(birthDay);
                if((amt=write(sock_fd, &birthDay, sizeof(time_t))) != sizeof(time_t))
                {
                    printf("registrate failed to write birthDay\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
            case GET_ACCOUNT_INFO :
                get_account_info(sockfd);
            case TRANSACT :
                transact(acc_num, transact_amt);
            case GET_BALANCE :
                
            case REQUEST_CASH :
                request_cash(transact_amt);
            case ERROR :
                messageError(sockfd);
            case TERMINATE :
                terminate(sockfd);
                close(sockfd);
                sockfd = 0;
                connfd = sockfd;

    }
    pthread_mutex_lock(&lock);
    //sumthin
    pthread_mutex_unlock(&lock);
}

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
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
    //write_to_log_file();


    char *serv_addr = argv[1];
    int PORT = atoi(argv[2]);
    int numWorkers = atoi(argv[3]);
    
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // create a socket and begin listening on it
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } 
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    
    if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } 

    if ((listen(sockfd, 1)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        //printf("Listening...\n");
    len = sizeof(cli);



        //printf("Server accepted connection\n");
    /*For each incoming connection, the server will create a worker thread 
    which will handle the connection (pass it the connection’s file descriptor) 
    and return to listening on the socket. */
    pthread_t tid;
    while(1){
            // Function for chatting between client and server
        connfd = accept(sockfd, (SA *) &cli, &len); // blocks if doesn't have a connection
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } 

        pthread_create(&tid, NULL, worker_thread, (void *)&connfd);
        sleep(0.1);
    }
    // Server never shut down
    close(connfd);
    close(sockfd);



    return 0; 
}

void account_info(int sock_fd, int accNum){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = ACCOUNT_INFO;
    char name[MAX_STR] = balances[accNum].name;
    char username[MAX_STR] = balances[accNum].username;
    time_t birthday = balances[accNum].birthday;

    // write the message type first
    if((amt=write(sock_fd, htonl(&msg), sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("account_info failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //write arguments for message type
    //write the username to client
    htonl(&username);
    if((amt=write(sock_fd, htonl(&username), sizeof(char)*MAX_STR)) < 1)
    {
        printf("account_info failed to write username\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the name to client
    if((amt=write(sock_fd, htonl(&name), sizeof(char)*MAX_STR)) < 1)
    {
        printf("account_info failed to write name\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the birthday to client
    if((amt=write(sock_fd, htonl(&birthday), sizeof(time_t))) != sizeof(time_t))
    {
        printf("account_info failed to write birthday\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}


void cash(int sock_fd, int sentCash){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = CASH;

    // write the message type first
    if((amt=write(sock_fd, htonl(&msg), sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("cash failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    
    //write the sent cash
    if((amt=write(sock_fd, htonl(&sentCash), sizeof(int))) != sizeof(int))
    {
        printf("cash failed to write sent cash\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void balance(int sock_fd, float balance){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = BALANCE;

    // write the message type first
    if((amt=write(sock_fd, htonl(&msg), sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("balance failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //write the balance
    if((amt=write(sock_fd, htonl(&balance), sizeof(float))) != sizeof(float))
    {
        printf("balance failed to write balance\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void messageError(int sock_fd){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = ERROR;

    // write the message
    if((amt=write(sock_fd, htonl(&msg), sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("messageError failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}