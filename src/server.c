#include "server.h"

#define SA struct sockaddr

sem_t sem_worker;
sem_t sem_main;

void account_info(int sock_fd, int accNum){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = ACCOUNT_INFO;
    pthread_mutex_lock(&balances[accNum].lock);
    char *name = balances[accNum].name; 
    char *username = balances[accNum].username;
    time_t birthday = balances[accNum].birthday;
    pthread_mutex_unlock(&balances[accNum].lock);

    // write the message type first
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("account_info failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //write arguments for message type
    //write the username to client
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
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("messageError failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    
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
    sleep(8);
    char *balancesFile = "output/balances.csv";
    FILE *fp = fopen(balancesFile, "w");   //write to output balances file

    if(fp == NULL){
        printf("ERROR: failed to create %s\n", balancesFile);
        exit(EXIT_FAILURE);
    }
    int i = 0;
    while(strcmp(balances[i].name,"unused") != 0){
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
    sem_wait(&sem_worker);
    int connfd = *(int *)arg;
    sem_post(&sem_main);
    int amt, acc_num;
    float retBalance;
    int test = 1;
    while(test){
        msg_enum msg_type;
        if((amt=read(connfd, &msg_type, sizeof(msg_enum))) != sizeof(msg_enum))
        {
            printf("worker failed to read msg_type\n.");
            printf("It read %d bytes\n.", amt);
            exit(1);
        }
        
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
                
                //read the name from client
                if((amt=read(connfd, name, sizeof(char)*MAX_STR)) < 1)
                {
                    printf("worker failed to read name\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                //read the birthday from client
                if((amt=read(connfd, &birthday, sizeof(time_t))) != sizeof(time_t))
                {
                    printf("worker failed to read birthday\n.");
                    printf("It wrote %d bytes\n.", amt);
                    exit(1);
                }
                // find next open account slot
                int i = 0;
                while(strcmp(balances[i].name, "unused") != 0){
                    i++;
                }

                pthread_mutex_init(&balances[i].lock, NULL);
                pthread_mutex_lock(&balances[i].lock);
                strcpy(balances[i].username, username);
                strcpy(balances[i].name, name);
                balances[i].birthday = birthday;
                balances[i].balance = 0;
                pthread_mutex_unlock(&balances[i].lock);
                retBalance = 0;
                
                balance(connfd, retBalance, i); // send message back
                break;
            case GET_ACCOUNT_INFO : ;
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
                pthread_mutex_lock(&balances[acc_num].lock);
                if(balances[acc_num].balance >= (-transact_amt)){

                    balances[acc_num].balance += transact_amt;
                    float retBalance = balances[acc_num].balance;

                    balance(connfd, retBalance, acc_num);
                }
                pthread_mutex_unlock(&balances[acc_num].lock);
                break;
            case GET_BALANCE : ;
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
                
                if(message_type != 8){
                    printf("No enumerated message for number: %d\n", message_type);
                }
                break;
            case TERMINATE : ;
                close(connfd);
                test = 0;
                break;
        }
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
        strcpy(balances[i].name, "unused"); // this is to give a concrete value for unused slots to check with
    }
    pthread_t tid;
    // start a log thread, wait 8 seconds, and write a log to a file
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
    } 
    
    len = sizeof(cli);


    /*For each incoming connection, the server will create a worker thread 
    which will handle the connection (pass it the connection’s file descriptor) 
    and return to listening on the socket. */
    int temp;
    sem_init(&sem_main, 0, 1);
    sem_init(&sem_worker, 0, 1);
    while(1){
        connfd = accept(sockfd, (SA *) &cli, &len);
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } 
        sem_wait(&sem_main);
        temp = connfd;
        sem_post(&sem_worker);
        // Launch worker thread to handle client connection
        pthread_create(&tid, NULL, worker_thread, (void*)&temp);
    }
    // Server never shut down

    return 0; 
}
