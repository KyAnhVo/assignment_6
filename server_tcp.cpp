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
    struct sockaddr_in servaddr;

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("ERR: TCP: create socket failed\n");
        exit(1);
    }
    // printf("** TCP: create socket successfully **\n");

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
    // printf("** TCP: bind socket-port successfully **\n");
    
    // TODO: continue implementation

    // start listening
    if (listen(sockfd, 5) == -1)
    {
        printf("ERR: TCP: listen failed\n");
        exit(1);
    }
    // printf("** TCP: listen successfully **\n");

    struct ThreadArgs arg[5];
    for (;;)
    {   
        pthread_t * thread = new pthread_t;
        struct ThreadArgs * args = new struct ThreadArgs;
        args->clientAddress = new struct sockaddr_in;
        socklen_t len = sizeof(struct sockaddr_in);
        int s = accept(sockfd, (sockaddr *) args->clientAddress, &len);
        if (s <= 0)
        {
            printf("ERR: TCP: accept failed\n");
            delete args->clientAddress;
            delete args;
            delete thread;
            continue;
        }
        args->socket = s;
        if (pthread_create(thread, NULL, serverTcpConnection, (void*) args) != 0)
        {
            printf("ERR: TCP: pthread_create failed\n");
            close(s);
            delete args->clientAddress;
            delete args;
            delete thread;
            continue;
        }
        pthread_detach(*thread);
        delete thread;
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
    struct ThreadArgs * args = (struct ThreadArgs *) argv;
    int sockfd = args->socket, logsfd;
    sockaddr_in cli = *(args->clientAddress), logsaddr;
    // printf("** TCP: accepted call at socket %d **\n", sockfd);

    // setup log_s communication
    logsfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (logsfd == -1)
    {
        printf("ERR: TCP: create log_s socket failed\n");
        delete args->clientAddress;
        delete args;
        close(sockfd);
        close(logsfd);
        return NULL;
    }
    // printf("** TCP: create socket successfully for log_s **\n");

    // Enable SO_REUSEADDR
    int opt = 1;
    if (setsockopt(logsfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        printf("ERR: TCP: setsockopt failed\n");
        delete args->clientAddress;
        delete args;
        close(sockfd);
        close(logsfd);
        return NULL;
    }
    // printf("** TCP: set SO_REUSEADDR successfully for log_s **\n");

    // create log_s sockaddr_in with loopback and port 9999 (assume using same pc for simplicity)
    bzero(&logsaddr, sizeof(struct sockaddr_in));
    logsaddr.sin_port           = htons((short) 9999);
    logsaddr.sin_family         = AF_INET;
    logsaddr.sin_addr.s_addr    = inet_addr("127.0.0.1");

    char buffer[100];
    
    while (1)
    {
        bzero((void*) buffer, sizeof(buffer));
        int count = recv(sockfd, (void*) buffer, sizeof(buffer), 0);
        if (count == -1)
        {
            ("ERR: TCP: socket %d: recv() error\n", sockfd);
            break;
        }
        if (count == 0) continue;
        if (strcmp(buffer, "exit\n") == 0)
        {
            printf("** TCP: socket %d: exit **\n", sockfd);
            break;
        }
        for (int i = 0; i < count; i++)
        {
            if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
                break;
            }
        }
        printf("TCP: socket %d: %s\n", sockfd, buffer);

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
            break;
        }
        // printf("** TCP: sen msg to log_s successful: %s **\n", logBuffer);
    }

    delete args->clientAddress;
    delete args;
    close(sockfd);
    close(logsfd);
    return NULL;
}