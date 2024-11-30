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

#include "server_tcp.h"
#include "server_udp.h"

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

pthread_t tcpThread, udpThread;

/**
 * @param argv[0] port number
 */

int main(int argc, char** argv)
{   
    // check if has argument
    if (argc != 2)
    {
        printf("ERR: incorrect # arguments\n");
        exit(1);
    }

    // always first arg is port number
    char* endptr;
    int portTemp = (int) strtol(argv[1], &endptr, 10); // portTemp because short gurantees hit
    if (*endptr != '\0')
    {
        printf("ERR: port not a number\n");
        exit(1);
    }
    if (portTemp < 1 || portTemp > 65535)
    {
        printf("ERR: Port #%d does not exist\n", portTemp);
        exit(1);
    }
    ushort port = (ushort) portTemp;
    printf("** PORT RECOGNIZED: #%d **\n", port);

    // each thread handle udp/tcp on itself
    pthread_create(&tcpThread, NULL, serverTCP, (void *) &port);
    pthread_create(&udpThread, NULL, serverUDP, (void *) &port);

    pthread_join(tcpThread, NULL);
    pthread_join(udpThread, NULL);
}