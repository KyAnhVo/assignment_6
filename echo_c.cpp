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

#define UDP false
#define TCP true

void udpCli(char*, ushort);
void tcpCli(char*, ushort);

/**
 * @param argv[1]: protocol ("-u" = udp, "-t" = tcp)
 * @param argv[2]: ip address
 * @param argv[3]: port
 */
int main(int argc, char** argv)
{   
    bool protocol;
    ushort port;

    // check arg count
    if (argc != 4)
    {
        printf("ERR: wrong # arguments\n");
        exit(1);
    }

    // check if argv[1] is valid protocol option
    // set protocol
    if (strcmp(argv[2], "-u") == 0)
    {
        protocol = UDP;
    }
    else if (strcmp(argv[2], "-t") == 0)
    {
        protocol = TCP;
    }
    else
    {
        printf("ERR: 1st arg must be -t = tcp or -u = udp\n");
        exit(1);
    }

    // check if argv[2] is valid ip address
    struct sockaddr_in temp;
    int ipCheck = inet_pton(AF_INET, argv[2], &(temp.sin_addr));
    if (ipCheck == 0)
    {
        printf("ERR: invalid IP address\n");
        exit(1);
    }

    // check if argv[3] is valid port
    char* endptr;
    int portTemp = (int) strtol(argv[1], &endptr, 10); // int portTemp because ushort gurantees "wacky" conversion
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
    port = (ushort) portTemp;

    if (protocol == TCP)
        tcpCli(argv[2], port);
    else
        udpCli(argv[2], port);

}

void tcpCli(char* ip, ushort port)
{
    // definitions
    int sockfd;
    sockaddr_in servaddr;

    // create and connect socket to server
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        printf("ERR: socket creation failed\n");
        exit(1);
    }
    bzero((void*) &servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(inet_addr(ip));
    if (connect(sockfd, (const sockaddr *) &servaddr, sizeof(struct sockaddr_in) != 0)
    {
        printf("ERR: connection failed\n");
        exit(1);
    }

}

void udpCli(char* ip, ushort port)
{
    
}