#include "server.h"

#define LOCALHOST "127.0.0.1"
#define MAX 516
#define PORT 9333
#define NCLIENTS 5
#define SA struct sockaddr

void printSyntax(){
    printf("incorrect usage syntax! \n");
    printf("usage: $ ./server server_addr server_port num_workers\n");
}

//void write_to_log_file(){}

/*A worker thread will parse each query 
    received and reply with the appropriate response. If it modifies the global balance 
    datastructure, it should signal the log thread’s condition variable. This will continue 
    until it receives a TERMINATE query from the client. It will then close the 
    connection and return.*/
//void pass_to_worker(){}

int main(int argc, char *argv[]){
    // argument handling
    /*if(argc != 4)    commented out for interim submission
    {
        printSyntax();
        return 0;
    }*/
    
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;

    // TODO: complete the next line with socket()
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

    // TODO: fill out if-condition with bind()
    if ((bind(sockfd, (SA *) &servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    } 

    // TODO: fill out if-condition with listen()
    if ((listen(sockfd, 1)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    } else
        //printf("Listening...\n");
    len = sizeof(cli);


        // TODO: complete the next line with accept()
        connfd = accept(sockfd, (SA *) &cli, &len); // blocks if doesn't have a connection
        if (connfd < 0) {
            printf("Server accept failed...\n");
            exit(0);
        } 
        //printf("Server accepted connection\n");
    char msgid[MAX];
    while(1){
            // Function for chatting between client and server

        memset(msgid, 0, MAX);

        int size = read(connfd, msgid, sizeof(msgid));
        if (size < 0) {
            perror("read error");
            exit(1);
        }


   msg_enum msgEnum = atoi(msgid);
   char *strEnum = getMsgEnum(msgEnum);
   printf("%s : %d\n", strEnum, msgEnum);
   if(msgEnum == TERMINATE){
       break;
   }


        if (write(connfd, msgid, strlen(msgid)) < 0) {
            perror("write error");
            exit(1);
        }
      
    }
    if (write(connfd, msgid, strlen(msgid)) < 0) {
        perror("write error");
        exit(1);
    }
    // Server never shut down
    close(connfd);
    close(sockfd);

    // create empty output folder
    //bookeepingCode();

    // start a log thread, wait 5 seconds, and write a log to a file
    //write_to_log_file();

    // create a socket and begin listening on it

    /*For each incoming connection, the server will create a worker thread 
    which will handle the connection (pass it the connection’s file descriptor) */
    //pass_to_worker(fileDesc);

    //and return to listening on the socket.

    return 0; 
}

