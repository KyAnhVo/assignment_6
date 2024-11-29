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
    servaddr.sin_addr.s_addr    = inet_addr("127.0.0.1");
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

    for (int i = 0; i < 5; i++)
    {
        socklen_t len = sizeof(cli[i]);
        int s = accept(sockfd, (sockaddr *) (cli + i), &len);
        pthread_create(threads + i, NULL, serverTcpConnection, (void*) &s);
    }

    for (int i = 0; i < 5; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    // close socket, return from thread
    close(sockfd);
    printf("** TCP: socket closed **\n");
    return NULL;
}

/**
 * Used to handle each tcp accepted communication in a seperate thread
 * @param argv: {void*} address to file descriptor of communicating socket
 */
void *serverTcpConnection(void* argv)
{
    int sockfd = *((int*) argv);
    printf("** TCP: accepted call at socket %d **\n", sockfd);

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
        printf("TCP: socket %d: received: %s\n", sockfd, buffer);

        // TODO: send to LOG
    }
}