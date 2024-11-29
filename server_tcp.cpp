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
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

#include <unordered_map>

/**
 * Used to multithread for TCP side
 * @param argv: address to port number (ushort)
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

    if (listen(sockfd, 5) == -1)
    {
        printf("ERR: TCP: listen failed\n");
        exit(1);
    }

    
    
    // close socket, return from thread
    close(sockfd);
    printf("** TCP: socket closed **\n");
    return NULL;
}