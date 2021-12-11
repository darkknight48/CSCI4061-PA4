#include "client.h"
#include "time.h"

//#define LOCALHOST "127.0.0.1"
#define MAX 516
#define NCLIENTS 5
#define SA struct sockaddr

int connfd;

void messageError(int sock_fd, msg_enum wrongMsg){
    //integer to hold bytes written and read
    int amt = 0;

    //variables to write
    msg_enum msg = ERROR;

    // write the message
    msg_enum net_msg = htonl(msg);
    if((amt=write(sock_fd, &net_msg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("messageError failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    msg_enum net_wrong_msg = htonl(wrongMsg);
    if((amt=write(sock_fd, &net_wrong_msg, sizeof(msg_enum))) != sizeof(msg_enum))
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
    msg_enum net_get_balance = htonl(get_balance_msg);
    if(amt = write(connfd, &net_get_balance, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write GET_BALANCE\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //sending the account number for GET_BALANCE
    int net_acc_num = htonl(acc_num);
    if(amt = write(connfd, &net_acc_num, sizeof(int)) != sizeof(int)){
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

    msg_enum net_request_cash_msg = htonl(request_cash_msg);
    if(amt = write(connfd, &net_request_cash_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write REQUEST_CASH\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //sending the amount for REQUEST_CASH
    //float net_val_requested = htonl(val_requested);
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
    net_response = ntohl(rsp_type);

    // handle us getting the wrong value back
    if(net_response != CASH)
    {
        messageError(connfd, net_response);
        return;
    }
    // get the cash amount
    if((amt=read(connfd, &wired_cash, sizeof(float))) < 1)
    {
        printf("transact failed to read cash amountr\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    //float net_cash = ntohl(wired_cash);

    cash += wired_cash;
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
    float val_requested = 5000;



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
    msg_enum net_response = ntohl(rsp_type);

    // handle us getting the wrong value back
    if(net_response != BALANCE)
    {
        messageError(connfd, net_response);
        return;
    }
    // get the account number
    if((amt=read(connfd, &account_num, sizeof(int))) < 1)
    {
        printf("transact failed to read account number\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    int net_account_num = ntohl(account_num);

    // get the balance
    if((amt=read(connfd, &balance, sizeof(float))) < 1)
    {
        printf("transact failed to read balance\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    //float net_balance = ntohl(balance);
    if(balance < (-val)){ // if the account doesn't have enough money for the desired withdrawl, ignore
        exit(0);
    }
    //only request cash if there isn't enough in the local bank (this client thread)
    //only applicable for withdrawls
        //this is why val is negated, because a deposit will increase the balance
    while(cash < (-val)){
        // send REQUEST_CASH to server until the cash variable 
        // (include/client.h) will not go negative
        requestCash(val_requested);
    }

    // send TRANSACTION message to the server
    msg_enum net_transact_msg = htonl(transact_msg);
    if(amt = write(connfd, &net_transact_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write TRANSACT\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    // add the value of the transaction to the cash
    // variable (deposites increase, withdrawls decrease)

    //sending the account number for TRANSACT
    net_account_num = htonl(acc_num);
    if(amt = write(connfd, &net_account_num, sizeof(int)) != sizeof(int)){
        printf("transact failed to write account number\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //sending the amount for TRANSACT
    //float net_val = htonl(val);
    if(amt = write(connfd, &val, sizeof(float)) != sizeof(float)){
        printf("transact failed to write val\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
 
    // NOTE: make sure to use proper error handling
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
    msg_enum networkMsg = htonl(msg_type);
    if((amt=write(sock_fd, &networkMsg, sizeof(networkMsg))) != sizeof(networkMsg))
    {
        printf("registrate failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the username to client
    //char netUsrNm[MAX_STR] = htonl(userName);
    if((amt=write(sock_fd, &userName, sizeof(char)*MAX_STR)) < 1)
    {
        printf("registrate failed to write userName\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the name to client
    //char netNm[MAX_STR] = htonl(name);
    if((amt=write(sock_fd, &name, sizeof(char)*MAX_STR)) < 1)
    {
        printf("registrate failed to write name\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    //write the birthday to client
    //time_t netBirthday = htonl(birthDay);
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
    msg_enum hostRsp = ntohl(rsp_type);

    // handle us getting the wrong value back
    if(hostRsp != BALANCE)
    {
        messageError(sock_fd, hostRsp);
        return;
    }
    // get the account number
    if((amt=read(sock_fd, &servAccNum, sizeof(int))) != sizeof(int))
    {
        printf("registrate failed to read servAccNum\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    int hostAcNum = ntohl(servAccNum);
    // get the balance
    if((amt=read(sock_fd, &balance, sizeof(float))) != sizeof(float))
    {
        printf("registrate failed to read balance\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    //float hostBalance = ntohl(balance);


}


// dummy example of writing a GET_ACCOUNT_INFO message 
// and then reading a server's responding message
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
    msg_enum networkMsg = htonl(msg_type);
    if((amt=write(sock_fd, &networkMsg, sizeof(msg_enum))) != sizeof(msg_enum))
    {
        printf("get_account_info failed to write msg_type\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }
    // write the arguments for the message type
    int net_acc_num = htonl(acc_num);
    if((amt=write(sock_fd, &net_acc_num, sizeof(int))) != sizeof(int))
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
    else if(rsp_type != ACCOUNT_INFO)
    {
        messageError(sock_fd, rsp_type);
        return;
    }
    // get the username
    if((amt=read(sock_fd, &username, sizeof(char)*MAX_STR)) < 1)
    {
        printf("get_account_info failed to read username\n.");
        printf("It read %d bytes\n.", amt);
        exit(1);
    }
    // get the name
    if((amt=read(sock_fd, &name, sizeof(char)*MAX_STR)) < 1)
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
    char* inputFilename;
    sprintf(inputFilename, "input/%s", inFilename);
    char* serv_addr = argv[2];
    int PORT = atoi(argv[3]);
    
    time_t t1 = time(NULL);
    
    int sockfd, amt;
    struct sockaddr_in servaddr, cli;
    // socket create and verification
    // TODO: complete next line (socket)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } else
        printf("Socket created...\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serv_addr);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    // TODO: fill out if condition (connect)
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } else
        printf("Connected with server\n");
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
            } else
            printf("Socket recreated...\n");    
            if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
                printf("Reconnection with the server failed...\n");
                exit(0);
            } else
                printf("Reconnected with server\n");
            connfd = sockfd;
        }

        int input_msg_type; // technically a msg_enum
        int acc_num;
        char name[MAX_STR];
        char usrName[MAX_STR];
        long birthDay;
        float transact_amt;
        int num_transactions;
        sscanf(line, "%d,%d,%s,%s,%ld,%f,%d\n", &input_msg_type, &acc_num, name, usrName, &birthDay, 
                &transact_amt, &num_transactions);
        time_t temp = (time_t)birthDay;
        switch (input_msg_type){
            case REGISTER :
                registrate(sockfd, acc_num, usrName, name, temp);
            case GET_ACCOUNT_INFO :
                get_account_info(sockfd);
            case TRANSACT :
                transact(acc_num, transact_amt);
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
                msg_enum net_response = ntohl(rsp_type);

                // handle us getting the wrong value back
                if(net_response != BALANCE)
                {
                    messageError(connfd, net_response);
                    continue;
                }
                // get the account number
                if((amt=read(connfd, &account_num, sizeof(int))) < 1)
                {
                    printf("transact failed to read account number\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                int net_account_num = ntohl(account_num);

                // get the balance
                if((amt=read(connfd, &balance, sizeof(float))) < 1)
                {
                    printf("transact failed to read balance\n.");
                    printf("It read %d bytes\n.", amt);
                    exit(1);
                }
                //float net_balance = ntohl(balance);
            case REQUEST_CASH :
                requestCash(transact_amt);
            case ERROR :
                messageError(sockfd, ERROR);
            case TERMINATE :
                terminate(sockfd);
                close(sockfd);
                sockfd = 0;
                connfd = sockfd;
        }
        
    }
    terminate(sockfd);
    close(sockfd);
    sockfd = 0;
    connfd = sockfd;

    time_t t2 = time(NULL);
    time_t elapTime = t2 - t1;
    double printedTime = (double)elapTime;
    printf("Elapsed Time: %.2f\n", printedTime);

    return 0; 
}
