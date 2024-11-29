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
 * @param argv: address to port number (ushort)
 */
void *serverUDP(void* argv)
{
    ushort port = *((ushort *) argv);

    // declarations
    int sockfd, connfd;
    uint len;
    struct sockaddr_in servaddr;

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
    servaddr.sin_addr.s_addr    = inet_addr("127.0.0.1");
    servaddr.sin_family         = AF_INET;

    // bind socket to port
    int status = bind(sockfd, (const SA *) &servaddr, sizeof(struct sockaddr_in));
    if (status != 0)
    {
        printf("ERR: UDP: bind socket-port failed\n");
        exit(1);
    }
    printf("** UDP: bind socket-port successfully **\n");

    // TODO: continue implementation

    // close socket, return from thread
    close(sockfd);
    printf("** UDP: socket closed **\n");
    return NULL;
}