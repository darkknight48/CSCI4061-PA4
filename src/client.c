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

