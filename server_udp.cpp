#include "server_udp.h"

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
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#include <unordered_map>

/**
 * Used to multithread for UDP side
 * @param argv: {ushort *} port number address
 */
void *serverUDP(void* argv)
{
    ushort port = *((ushort *) argv);

    // declarations
    int sockfd, logsfd;
    struct sockaddr_in servaddr, logsaddr, cli;
    socklen_t len = sizeof(struct sockaddr_in);

    // create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        printf("ERR: UDP: create socket failed\n");
        exit(1);
    }
    printf("** UDP: create socket successfully **\n");

    // change servaddr to have correct port and ip
    // NOTE: change inet_addr after testing
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_port           = htons(port);
    servaddr.sin_addr.s_addr    = INADDR_ANY;
    servaddr.sin_family         = AF_INET;

    // Enable SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        printf("ERR: TCP: setsockopt failed\n");
        exit(1);    
    }
    printf("** TCP: set SO_REUSEADDR successfully **\n");

    // bind socket to port
    int status = bind(sockfd, (const SA *) &servaddr, sizeof(struct sockaddr_in));
    if (status != 0)
    {
        printf("ERR: UDP: bind socket-port failed\n");
        exit(1);
    }
    printf("** UDP: bind socket-port successfully **\n");

    // setup log_s communication

    logsfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (logsfd == -1)
    {
        printf("ERR: UDP: create log_s socket failed\n");
        exit(1);
    }
    printf("** UDP: create socket successfully for log_s **\n");

    // Enable SO_REUSEADDR for log_s socket
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
    logsaddr.sin_addr.s_addr    = htonl(inet_addr("127.0.0.1"));
    if (bind(logsfd, (const SA *) &logsaddr, sizeof(struct sockaddr_in)) != 0)
    {
        printf("ERR: UDP: bind socket-port failed for log_s\n");
        exit(1);
    }
    printf("** UDP: bind socket-port successfully for log_s **\n");

    char buffer[100];
    while (1)
    {
        // receive and print quote, unless error or 0-length or "exit" is sent
        bzero((void*) buffer, sizeof(buffer));
        int count = recvfrom(sockfd, (void*) buffer, sizeof(buffer), 0, (SA*) &cli, &len);
        if (count == -1)
        {
            printf("ERR: UDP: recvfrom() error\n");
            exit(1);
        }
        if (count == 0) continue;
        if (count == 1) continue;
        if (strcmp(buffer, "exit\n") == 0)
        {
            printf("** UDP: socket %d: exit **\n", sockfd);
            return NULL;
        }
        // delete the \n, change it into null char
        buffer[count - 1] = '\0';
        printf("UDP: socket %d: received: %s\n", sockfd, buffer);

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
            printf("ERR: UDP: sendto() err to log_s\n");
            exit(1);
        }
        printf("** UDP: sen msg to log_s successful: %s **\n", logBuffer);
    }



    // close socket, return from thread
    close(sockfd);
    return NULL;
}