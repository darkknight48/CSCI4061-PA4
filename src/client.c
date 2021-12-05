#include "client.h"

#define LOCALHOST "127.0.0.1"
#define MAX 516
#define PORT 9333
#define NCLIENTS 5
#define SA struct sockaddr

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


    int sockfd, connfd;
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
    servaddr.sin_addr.s_addr = inet_addr(LOCALHOST);
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    // TODO: fill out if condition (connect)
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) != 0) {
        printf("Connection with the server failed...\n");
        exit(0);
    } else
        printf("Connected with server\n");

for(int i = REGISTER; i <= TERMINATE; i++){
    char msgID[MAX];
    memset(msgID, 0, MAX);
    sprintf(msgID, "%d", i);
    if (write(sockfd, msgID, strlen(msgID)) < 0) {
        perror("Cannot write");
        exit(1);
    }
}
while(1){

    char recv[MAX];
    memset(recv, 0, MAX);
    if (read(sockfd, recv, MAX) < 0) {
        perror("cannot read");
        exit(1);
    }

    int temp = atoi(recv);
    if(temp == 0){
        printf("REGISTER\n");
    }else if(temp == 1){
        printf("GET_ACCOUNT_INFO\n");
    }else if(temp == 2){
        printf("TRANSACT\n");
    }else if(temp == 3){
        printf("GET_BALANCE\n");
    }else if(temp == 4){
        printf("ACCOUNT_INFO\n");
    }else if(temp == 5){
        printf("BALANCE\n");
    }else if(temp == 6){
        printf("REQUEST_CASH\n");
    }else if(temp == 7){
        printf("CASH\n");
    }else if(temp == 8){
        printf("ERROR\n");
    }else if(temp == 9){
        printf("TERMINATE\n");
        break;
    }


}
    // close the socket
    close(sockfd);

    //msg_enum msg{};
    //printf("%d\n", msg);
        

    return 0; 
}

