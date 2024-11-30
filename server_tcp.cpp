#include "server_tcp.h"

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#include <unordered_map>

// used for threads of each connection
pthread_t threads[5];

struct ThreadArgs
{
    int socket;
    sockaddr_in *clientAddress;
};

/**
 * Used to multithread for TCP side
 * @param argv: {ushort *} port number address
 */
void *serverTCP(void* argv)
{
    ushort port = *((ushort *) argv);

    // declarations
    int sockfd, connfd, status;
    uint len;
    struct sockaddr_in servaddr, cli[5];

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("ERR: TCP: create socket failed\n");
        exit(1);
    }
    printf("** TCP: create socket successfully **\n");

    // change servaddr to have correct port and ip
    // NOTE: change inet_addr after testing
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_port           = htons(port);
    servaddr.sin_addr.s_addr    = INADDR_ANY;
    servaddr.sin_family         = AF_INET;

    // bind socket to port
    status = bind(sockfd, (const SA *) &servaddr, sizeof(struct sockaddr_in));
    if (status != 0)
    {
        printf("ERR: TCP: bind socket-port failed\n");
        exit(1);
    }
    printf("** TCP: bind socket-port successfully **\n");
    
    // TODO: continue implementation

    // start listening
    if (listen(sockfd, 5) == -1)
    {
        printf("ERR: TCP: listen failed\n");
        exit(1);
    }
    printf("** TCP: listen successfully **\n");

    struct ThreadArgs arg[5];
    for (int i = 0; i < 5; i++)
    {
        socklen_t len = sizeof(cli[i]);
        int s = accept(sockfd, (sockaddr *) (cli + i), &len);
        
        arg[i].socket = s;
        arg[i].clientAddress = cli + i;
        pthread_create(threads + i, NULL, serverTcpConnection, (void*) (arg + i));
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    // close socket, return from thread
    close(sockfd);
    return NULL;
}

/**
 * Used to handle each tcp accepted communication in a seperate thread
 * @param argv: {void*} address to file descriptor of communicating socket
 */
void *serverTcpConnection(void* argv)
{
    struct ThreadArgs args = *((struct ThreadArgs *) argv);
    int sockfd = args.socket, logsfd;
    sockaddr_in cli = *(args.clientAddress), logsaddr;
    printf("** TCP: accepted call at socket %d **\n", sockfd);

    // setup log_s communication
    logsfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (logsfd == -1)
    {
        printf("ERR: TCP: create log_s socket failed\n");
        exit(1);
    }
    printf("** TCP: create socket successfully for log_s **\n");

    // Enable SO_REUSEADDR
    int opt = 1;
    if (setsockopt(logsfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        printf("ERR: TCP: setsockopt failed\n");
        exit(1);    
    }
    printf("** TCP: set SO_REUSEADDR successfully for log_s **\n");

    // bind log_s to 9999 port
    bzero(&logsaddr, sizeof(struct sockaddr_in));
    logsaddr.sin_port           = htons((short) 9999);
    logsaddr.sin_family         = AF_INET;
    logsaddr.sin_addr.s_addr    = inet_addr("127.0.0.1");
    if (bind(logsfd, (const SA *) &logsaddr, sizeof(struct sockaddr_in)) != 0)
    {
        printf("ERR: TCP: bind socket-port failed for log_s\n");
        exit(1);
    }
    printf("** TCP: bind socket-port successfully for log_s **\n");

    char buffer[100];
    
    while (1)
    {
        bzero((void*) buffer, sizeof(buffer));
        int count = recv(sockfd, (void*) buffer, sizeof(buffer), 0);
        if (count == -1)
        {
            ("ERR: TCP: socket %d: recv() error\n", sockfd);
        }
        if (count == 0) continue;
        if (strcmp(buffer, "exit\n") == 0)
        {
            printf("** TCP: socket %d: exit **\n", sockfd);
            return NULL;
        }
        buffer[count - 1] = '\0';
        printf("TCP: socket %d: received: %s\n", sockfd, buffer);

        // send to log_s
        
        // get ip address in string format
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(cli.sin_addr), ip, INET_ADDRSTRLEN);

        // get the {"<msg>" was received from <IP>} line
        char logBuffer[150];
        bzero((void*) logBuffer, sizeof(logBuffer));
        strcpy(logBuffer, "\"");
        strcat(logBuffer, buffer);
        strcat(logBuffer, "\"");
        strcat(logBuffer, " was received from ");
        strcat(logBuffer, ip);
        
        // send message to log_s
        int countlogs = sendto(logsfd, (const void*) logBuffer, sizeof(logBuffer), 0, (const sockaddr*) &logsaddr, sizeof(struct sockaddr_in));
        if (countlogs == -1)
        {
            printf("ERR: TCP: sendto() err to log_s\n");
            exit(1);
        }
        printf("** TCP: sen msg to log_s successful: %s **\n", logBuffer);
    }
}