#include "server.h"

//#define LOCALHOST "127.0.0.1"
//#define MAX 516
//#define PORT 9333
#define SA struct sockaddr

void account_info(int sock_fd, int accNum){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = ACCOUNT_INFO;
    pthread_mutex_lock(&balances[accNum].lock);
    char *name = balances[accNum].name; // i'm curious how this will affect passing between sockets
    char *username = balances[accNum].username;
    time_t birthday = balances[accNum].birthday;
    pthread_mutex_unlock(&balances[accNum].lock);

    // write the message type first
    //msg_enum net_msg = htonl(msg);
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("account_info failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //write arguments for message type
    //write the username to client
    //htonl(&username);
    if((amt=write(sock_fd, username, sizeof(char)*MAX_STR)) < 1)
    {
        printf("account_info failed to write username\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the name to client
    if((amt=write(sock_fd, name, sizeof(char)*MAX_STR)) < 1)
    {
        printf("account_info failed to write name\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the birthday to client
    if((amt=write(sock_fd, &birthday, sizeof(time_t))) != sizeof(time_t))
    {
        printf("account_info failed to write birthday\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}


void cash(int sock_fd, float sentCash){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = CASH;

    // write the message type first
    //msg_enum net_msg = htonl(msg);
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("cash failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    
    //write the sent cash
    if((amt=write(sock_fd, &sentCash, sizeof(float))) != sizeof(float))
    {
        printf("cash failed to write sent cash\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void balance(int sock_fd, float balance, int acc_num){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = BALANCE;

    // write the message type first
    //msg_enum net_msg = htonl(msg);
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("balance failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //write the account number
    if((amt=write(sock_fd, &acc_num, sizeof(int))) != sizeof(int))
    {
        printf("balance failed to write account num\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //write the balance
    if((amt=write(sock_fd, &balance, sizeof(float))) != sizeof(float))
    {
        printf("balance failed to write balance\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void messageError(int sock_fd, msg_enum wrongMsg){ // this function is never called...
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = ERROR;

    // write the message
    //msg_enum net_msg = htonl(msg);
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("messageError failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //msg_enum net_wrong_msg = htonl(wrongMsg);
    if((amt=write(sock_fd, &wrongMsg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("messageError failed to write wrongMsg\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void* write_to_log_file(){
    //iterate over every account in balances[] to log account info to balances.csv
    //format: account number,balance,name,username,birthday  (”%d,%.2f,%s,%s,%ld\n”)
    printf("Log thread started. Waiting 5 seconds\n");
    sleep(5);
    char *balancesFile = "output/balances.csv";
    FILE *fp = fopen(balancesFile, "w");   //write to finalDir

    if(fp == NULL){
        printf("ERROR: failed to create %s\n", balancesFile);
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while(strcmp(balances[i].name,"unused") != 0){
        //printf("GI\n");
        char temp[1024];

        //lock
        pthread_mutex_lock(&balances[i].lock);

        sprintf(temp, "%d,%.2f,%s,%s,%ld\n", i, balances[i].balance, balances[i].name, balances[i].username, balances[i].birthday);
        
        //unlock
        pthread_mutex_unlock(&balances[i].lock);
        
        fputs(temp, fp);
        i++;
    }

    fclose(fp);
}

/*A worker thread will parse each query 
    received and reply with the appropriate response. If it modifies the global balance 
    datastructure, it should signal the log thread’s condition variable. This will continue 
    until it receives a TERMINATE query from the client. It will then close the 
    connection and return.*/
void* worker_thread(void* arg)
{
    printf("Worker thread started\n");
    int connfd = *(int *)arg;
    int amt, acc_num;
    float retBalance;
    int test = 1;
    while(test){
        msg_enum msg_type;
        if((amt=read(connfd, &msg_type, sizeof(msg_enum))) != sizeof(msg_enum))
        {
            printf("worker failed to read msg_type\n.");
            printf("It read %d bytes\n.", amt);
            int errnum = errno;
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error printed by perror");
            fprintf(stderr, "Error opening file: %s\n", strerror( errnum ));
            exit(1);
        }
        //printf("%d\n", (int)msg_type);
        switch (msg_type){
            case REGISTER : ;
                char username[MAX_STR];
                char name[MAX_STR];
                time_t birthday;
                if((amt=read(connfd, username, sizeof(char)*MAX_STR)) < 1)
                {
                    printf("worker failed to read username\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                printf("Server received username: %s\n", username);
                //read the name from client
                //char netNm[MAX_STR] = htonl(name);
                if((amt=read(connfd, name, sizeof(char)*MAX_STR)) < 1)
                {
                    printf("worker failed to read name\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                printf("Server received name: %s\n", name);
                //read the birthday from client
                //time_t netBirthday = htonl(birthDay);
                if((amt=read(connfd, &birthday, sizeof(time_t))) != sizeof(time_t))
                {
                    printf("worker failed to read birthday\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                //printf("S\n");
                int i = 0;
                while(strcmp(balances[i].name, "unused") != 0){
                    i++;
                }
                //printf("HI4\n");
                pthread_mutex_init(&balances[i].lock, NULL);
                pthread_mutex_lock(&balances[i].lock);
                strcpy(balances[i].username, username);
                strcpy(balances[i].name, name);
                balances[i].birthday = birthday;
                balances[i].balance = 0;
                pthread_mutex_unlock(&balances[i].lock);
                retBalance = 0;
                //printf("Server received: %s %s\n", username, name);
                printf("What was stored in balance array: %s %s %.2f\n", balances[i].username, balances[i].name, balances[i].balance);
                balance(connfd, retBalance, i);
                break;
            case GET_ACCOUNT_INFO : ;
                //int acc_num;
                // reads account number
                if((amt=read(connfd, &acc_num, sizeof(int))) != sizeof(int))
                {
                    printf("worker failed to read accout number\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                account_info(connfd, acc_num);
                break;
            case TRANSACT : ;
                //int acc_num;
                float transact_amt;
                // reads account number
                if((amt=read(connfd, &acc_num, sizeof(int))) != sizeof(int))
                {
                    printf("worker failed to read accout number\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                // reads transaction amount
                if((amt=read(connfd, &transact_amt, sizeof(float))) != sizeof(float))
                {
                    printf("worker failed to read transaction amount\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                if(balances[acc_num].balance > (-transact_amt)){
                    pthread_mutex_lock(&balances[acc_num].lock);
                    balances[acc_num].balance += transact_amt;
                    float retBalance = balances[acc_num].balance;
                    pthread_mutex_unlock(&balances[acc_num].lock);

                    balance(connfd, retBalance, acc_num);
                }
                break;
            case GET_BALANCE : ;
                //int acc_num;
                // reads account number
                if((amt=read(connfd, &acc_num, sizeof(int))) != sizeof(int))
                {
                    printf("worker failed to read accout number\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }

                pthread_mutex_lock(&balances[acc_num].lock);
                retBalance = balances[acc_num].balance;
                pthread_mutex_unlock(&balances[acc_num].lock);

                balance(connfd, retBalance, acc_num);
                break;
            case REQUEST_CASH : ;

                float amount;

                // get the cash amount
                if((amt=read(connfd, &amount, sizeof(float))) < 1)
                {
                    printf("worker failed to read cash amount\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                //float net_amount = ntohl(amount);   //amount to be sent back to client

                cash(connfd, amount);
                break;
            case ERROR : ;
                
                int message_type;

                if((amt=read(connfd, &message_type, sizeof(int))) < 1)
                {
                    printf("worker failed to read message type\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                //int message = ntohl(message_type);

                printf("No enumerated message for number: %d\n", message_type);
                break;
            case TERMINATE : ;
                close(connfd);
                test = 0;
                break;
        }
        //printf("E\n");
    }
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
    for(int i = 0; i < 1023; i++){
        strcpy(balances[i].name, "unused"); // this works
    }
    pthread_t tid;
    // start a log thread, wait 5 seconds, and write a log to a file
    pthread_create(&tid, NULL, write_to_log_file, NULL);


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



        
    /*For each incoming connection, the server will create a worker thread 
    which will handle the connection (pass it the connection’s file descriptor) 
    and return to listening on the socket. */
    
    while(1){
            // Function for chatting between client and server
        connfd = accept(sockfd, (SA *) &cli, &len); // blocks if doesn't have a connection
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } 
        printf("Server accepted connection\n");
        pthread_create(&tid, NULL, worker_thread, (void *)&connfd);
        sleep(0.1);
    }
    // Server never shut down
    close(connfd);
    close(sockfd);



    return 0; 
}

