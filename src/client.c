#include "client.h"
#include "sys/time.h"
#include "time.h"

#define SA struct sockaddr

int connfd;

void messageError(int sock_fd, msg_enum wrongMsg){
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

void getBalance(int acc_num){
    // integer to hold number of bytes read/written
    int amt = 0;

    msg_enum get_balance_msg = GET_BALANCE;
    if(amt = write(connfd, &get_balance_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write GET_BALANCE\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //sending the account number for GET_BALANCE
    if(amt = write(connfd, &acc_num, sizeof(int)) != sizeof(int)){
        printf("transact failed to write account number\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void requestCash(float val_requested){
    // integer to hold number of bytes read/written
    int amt = 0;
    float wired_cash;
    msg_enum rsp_type, net_response;
    
    msg_enum request_cash_msg = REQUEST_CASH;

    if(amt = write(connfd, &request_cash_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write REQUEST_CASH\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //sending the amount for REQUEST_CASH
    if(amt = write(connfd, &val_requested, sizeof(float)) != sizeof(float)){
        printf("transact failed to write val\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    // read in the response message type first (should be CASH)
    // as this tells us what fields we will need
    // to read afterwards and their types
    if((amt=read(connfd, &rsp_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("transact failed to read rsp_type\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // handle us getting the wrong value back
    if(rsp_type != CASH)
    {
        messageError(connfd, rsp_type);
        return;
    }
    // get the cash amount
    if((amt=read(connfd, &wired_cash, sizeof(float))) < 1)
    {
        printf("transact failed to read cash amountr\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    cash += wired_cash; // update the client cash balance (different than the account balance)
}

// logic for handling TRANSACT
void transact(int acc_num, float val)
{
    // integer to hold number of bytes read/written
    int amt = 0;

    msg_enum get_balance_msg = GET_BALANCE;
    msg_enum request_cash_msg = REQUEST_CASH;
    msg_enum transact_msg = TRANSACT;
    
    msg_enum rsp_type;
    int account_num;
    float balance;
    float val_requested = 20000;



    // send a GET_BALANCE message to the server to ensure
    // that the account will not go negative
    getBalance(acc_num);


    // read in the response message type first (should be BALANCE)
    // as this tells us what fields we will need
    // to read afterwards and their types
    if((amt=read(connfd, &rsp_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("transact failed to read rsp_type\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // handle us getting the wrong value back
    if(rsp_type != BALANCE)
    {
        messageError(connfd, rsp_type);
        return;
    }
    // get the account number
    if((amt=read(connfd, &account_num, sizeof(int))) < 1)
    {
        printf("transact failed to read account number\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // get the balance
    if((amt=read(connfd, &balance, sizeof(float))) < 1)
    {
        printf("transact failed to read balance\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    if(balance < (-val)){ // if the account doesn't have enough money for the desired withdrawl, ignore
        return;
    }
    //only request cash if there isn't enough in the local bank (this client thread)
    //only applicable for withdrawls
        //this is why val is negated, because a deposit will increase the balance
    while(cash < (-val)){
        // send REQUEST_CASH to server until the client cash balance 
        // (include/client.h) will not go negative
        requestCash(val_requested);
    }

    // send TRANSACTION message to the server
    if(amt = write(connfd, &transact_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write TRANSACT\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }


    //sending the account number for TRANSACT
    if(amt = write(connfd, &account_num, sizeof(int)) != sizeof(int)){
        printf("transact failed to write account number\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }


    //sending the amount for TRANSACT
    if(amt = write(connfd, &val, sizeof(float)) != sizeof(float)){
        printf("transact failed to write val\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
 

    // RECEIVE BALANCE RESPONSE
    if((amt=read(connfd, &rsp_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("transact failed to read rsp_type\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // handle us getting the wrong value back
    if(rsp_type != BALANCE)
    {
        messageError(connfd, rsp_type);
        return;
    }
    // get the account number
    if((amt=read(connfd, &account_num, sizeof(int))) < 1)
    {
        printf("transact failed to read account number\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // get the balance
    if((amt=read(connfd, &balance, sizeof(float))) < 1)
    {
        printf("transact failed to read balance\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
}

void registrate(int sock_fd, int acc_num, char userName[MAX_STR], char name[MAX_STR], time_t birthDay)
{
    // integer to hold number of bytes read/written
    int amt = 0;

    // variables to write to the socket
    msg_enum msg_type = REGISTER;

    // variables to write the read response into
    msg_enum rsp_type; // should be of type BALANCE
    int servAccNum;
    float balance;
    
    // using read and write aren't the best "real world" call
    // but will be the simplest for you to use
    //
    // write the message type first
    if((amt=write(sock_fd, &msg_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("registrate failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the username to client
    if((amt=write(sock_fd, userName, sizeof(char)*MAX_STR)) < 1)
    {
        printf("registrate failed to write userName\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the name to client
    if((amt=write(sock_fd, name, sizeof(char)*MAX_STR)) < 1)
    {
        printf("registrate failed to write name\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the birthday to client
    if((amt=write(sock_fd, &birthDay, sizeof(time_t))) != sizeof(time_t))
    {
        printf("registrate failed to write birthDay\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    
    // read in the response message type first
    // as this tells us what fields we will need
    // to read afterwards and their types
    if((amt=read(sock_fd, &rsp_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("registrate failed to read rsp_type\n."); //should be BALANCE
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // handle us getting the wrong value back
    if(rsp_type != BALANCE)
    {
        messageError(sock_fd, rsp_type);
        return;
    }
    // get the account number
    if((amt=read(sock_fd, &servAccNum, sizeof(int))) != sizeof(int))
    {
        printf("registrate failed to read servAccNum\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // get the balance
    if((amt=read(sock_fd, &balance, sizeof(float))) != sizeof(float))
    {
        printf("registrate failed to read balance\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
}


void get_account_info(int sock_fd)
{
    // integer to hold number of bytes read/written
    int amt = 0;

    // variables to write to the socket
    msg_enum msg_type = GET_ACCOUNT_INFO;
    int acc_num = 0;

    // variables to write the read response into
    msg_enum rsp_type;
    char name[MAX_STR];
    char username[MAX_STR];
    time_t birthday;
    
    // using read and write aren't the best "real world" call
    // but will be the simplest for you to use
    //
    // write the message type first
    if((amt=write(sock_fd, &msg_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("get_account_info failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    // write the arguments for the message type
    
    if((amt=write(sock_fd, &acc_num, sizeof(int))) != sizeof(int))
    {
        printf("get_account_info failed to write acc_num\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    
    // read in the response message type first
    // as this tells us what fields we will need
    // to read afterwards and their types
    if((amt=read(sock_fd, &rsp_type, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("get_account_info failed to read rsp_type\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // handle us getting the wrong value back
    if(rsp_type != ACCOUNT_INFO)
    {
        messageError(sock_fd, rsp_type);
        return;
    }
    // get the username
    if((amt=read(sock_fd, username, sizeof(char)*MAX_STR)) < 1)
    {
        printf("get_account_info failed to read username\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // get the name
    if((amt=read(sock_fd, name, sizeof(char)*MAX_STR)) < 1)
    {
        printf("get_account_info failed to read name\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    
    // get the birthday
    if((amt=read(sock_fd, &birthday, sizeof(time_t))) != sizeof(time_t))
    {
        printf("get_account_info failed to read birthday\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
}



void terminate(int sock_fd){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = TERMINATE;

    // write the message
    if((amt=write(sock_fd, &msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("messageError failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
}

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./client input_filename server_addr server_port\n");
}

int main(int argc, char *argv[]){
    // argument handling
    if(argc != 4)
    {
        printSyntax();
        return 0;
    }

    char* inFilename = argv[1];
    char inputFilename[MAX_STR];
    sprintf(inputFilename, "input/%s", inFilename);
    char* serv_addr = argv[2];
    int PORT = atoi(argv[3]);
    
    struct timeval start;
    struct timeval end;
    gettimeofday(&start, NULL);
    
    int sockfd, amt;
    struct sockaddr_in servaddr, cli;
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serv_addr);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } 

    
    connfd = sockfd;
    FILE *fp;
    char line[1024];
    fp = fopen(inputFilename, "r");
    if(fp == NULL){
        printf("ERROR: Failed to open input file.\n");
        exit(1);
    }

    while(fgets(line, 1024, fp)){
        if(sockfd == 0){
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd == -1) {
                printf("Socket recreation failed...\n");
                exit(0);
            } 

            if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
                printf("Reconnection with the server failed...\n");
                exit(0);
            } 
            
            connfd = sockfd;
        }
        //variables in one line of input file
        int input_msg_type; // technically a msg_enum
        int acc_num;
        char name[MAX_STR];
        char usrName[MAX_STR];
        long birthDay;
        float transact_amt;
        int num_transactions;
        //the dumb sscanf function doesn't know to stop reading the string with commas, so strtok_r it is
        char *tokTemp = line;
        char *token = strtok_r(tokTemp, ",", &tokTemp);
        input_msg_type = atoi(token);
        token = strtok_r(tokTemp, ",", &tokTemp);
        acc_num = atoi(token);
        token = strtok_r(tokTemp, ",", &tokTemp);
        strcpy(name, token);
        token = strtok_r(tokTemp, ",", &tokTemp);
        strcpy(usrName, token);
        token = strtok_r(tokTemp, ",", &tokTemp);
        birthDay = atol(token);
        token = strtok_r(tokTemp, ",", &tokTemp);
        transact_amt = (float)atof(token);
        token = strtok_r(tokTemp, ",", &tokTemp);
        num_transactions = atoi(token);
        //sscanf(line, "%d,%d,%s,%s,%ld,%f,%d\n", &input_msg_type, &acc_num, name, usrName, &birthDay, 
                //&transact_amt, &num_transactions);
        time_t temp = (time_t)birthDay;
        switch (input_msg_type){
            case REGISTER :
                registrate(sockfd, acc_num, usrName, name, temp);
                break;
            case GET_ACCOUNT_INFO :
                get_account_info(sockfd);
                break;
            case TRANSACT :
                transact(acc_num, transact_amt);
                break;
            case GET_BALANCE :
                getBalance(acc_num);
                // read in the response message type first (should be BALANCE)
                // as this tells us what fields we will need
                // to read afterwards and their types
                msg_enum rsp_type;
                int account_num;
                float balance;
                if((amt=read(connfd, &rsp_type, sizeof(msg_enum))) != sizeof(msg_enum))
                {
                    printf("transact failed to read rsp_type\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                
                // handle us getting the wrong value back
                if(rsp_type != BALANCE)
                {
                    messageError(connfd, rsp_type);
                    continue;
                }
                // get the account number
                if((amt=read(connfd, &account_num, sizeof(int))) < 1)
                {
                    printf("transact failed to read account number\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                
                // get the balance
                if((amt=read(connfd, &balance, sizeof(float))) < 1)
                {
                    printf("transact failed to read balance\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                break;
            case REQUEST_CASH :
                requestCash(transact_amt);
                break;
            case ERROR :
                messageError(sockfd, ERROR);
                break;
            case TERMINATE :
                terminate(sockfd);
                close(sockfd);
                sockfd = 0;
                connfd = sockfd;
                break;
        }
        
    }
    if(sockfd != 0){
        terminate(sockfd);
        close(sockfd);
        sockfd = 0;
        connfd = sockfd;
    }

    gettimeofday(&end, NULL);
    
    float elapTime = (end.tv_sec - start.tv_sec) + 1e-6*(end.tv_usec - start.tv_usec);
    printf("Elapsed Time: %.2f\n", elapTime);

    return 0; 
}
