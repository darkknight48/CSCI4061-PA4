#include "client.h"

#define LOCALHOST "127.0.0.1"
#define MAX 516
#define PORT 9333
#define NCLIENTS 5
#define SA struct sockaddr

int connfd;

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
    float cash;


    // send a GET_BALANCE message to the server to ensure
    // that the account will not go negative
    if(amt = write(&connfd, &get_balance_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write GET_BALANCE\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //sending the account number for GET_BALANCE
    if(amt = write(&connfd, &acc_num, sizeof(int)) != sizeof(int)){
        printf("transact failed to write account number\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }


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
    else if(rsp_type != BALANCE)
    {
        printf("transact recieved wrong rsp_type\n");
        exit(1);
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

    //only request cash if there isn't enough in the account balance
    //only applicable for withdrawls
        //this is why val is negated, because a deposit will increase the balance
    while(balance < (-val)){
        // send REQUEST_CASH to server until the cash variable 
        // (include/client.h) will not go negative
        if(amt = write(&connfd, &request_cash_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
            printf("transact failed to write REQUEST_CASH\n.");
            printf("It wrote %d bytes\n.", amt);
            exit(1);
        }

        //sending the amount for REQUEST_CASH
        if(amt = write(&connfd, &val_requested, sizeof(float)) != sizeof(float)){
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
        else if(rsp_type != CASH)
        {
            printf("transact recieved wrong rsp_type\n");
            exit(1);
        }
        // get the cash amount
        if((amt=read(connfd, &cash, sizeof(float))) < 1)
        {
            printf("transact failed to read cash amountr\n.");
            printf("It read %d bytes\n.", amt);
            exit(1);
        }

        balance += cash;
    }

    // send TRANSACTION message to the server
    if(amt = write(&connfd, &transact_msg, sizeof(msg_enum)) != sizeof(msg_enum)){
        printf("transact failed to write TRANSACT\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }



    // add the value of the transaction to the cash
    // variable (deposites increase, withdrawls decrease)

    //sending the account number for TRANSACT
    if(amt = write(&connfd, &acc_num, sizeof(int)) != sizeof(int)){
        printf("transact failed to write account number\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    //sending the amount for TRANSACT
    if(amt = write(&connfd, &val, sizeof(float)) != sizeof(float)){
        printf("transact failed to write val\n.");
        printf("It wrote %d bytes\n.", amt);
        exit(1);
    }

    

    // NOTE: make sure to use proper error handling
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
    else if(rsp_type != ACCOUNT_INFO)
    {
        printf("get_account_info recieved wrong rsp_type\n");
        exit(1);
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

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./client input_filename server_addr server_port\n");
}

int main(int argc, char *argv[]){
    // argument handling
    /*if(argc != 4)  commented out for interim submission
    {
        printSyntax();
        return 0;
    }*/


    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
    // socket create and verification
    // TODO: complete next line (socket)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("Socket creation failed...\n");
        exit(0);
    } //else
        //printf("Socket created...\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    // TODO: fill out if condition (connect)
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } //else
        //printf("Connected with server\n");

int i = REGISTER;
while(1){
    
    char msgID[MAX];
    memset(msgID, 0, MAX);
    sprintf(msgID, "%d", i);
    if (write(sockfd, msgID, strlen(msgID)) < 0) {
        perror("Cannot write");
        exit(1);
    }

    char recv[MAX];
    memset(recv, 0, MAX);
    if (read(sockfd, recv, MAX) < 0) {
        perror("cannot read");
        exit(1);
    }

    msg_enum msgEnum = atoi(recv);
    char *strEnum = getMsgEnum(msgEnum);
    printf("%s : %d\n", strEnum, msgEnum);
    if(msgEnum == TERMINATE){
        break;
    }

    i++;
}
    // close the socket
    close(sockfd);

    //msg_enum msg{};
    //printf("%d\n", msg);
        

    return 0; 
}
