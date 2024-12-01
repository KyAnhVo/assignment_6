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
    if (strcmp(argv[1], "-u") == 0)
    {
        protocol = UDP;
    }
    else if (strcmp(argv[1], "-t") == 0)
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
    int portTemp = (int) strtol(argv[3], &endptr, 10); // int portTemp because ushort gurantees "wacky" conversion
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

/**
 * used to 
 */

void tcpCli(char* ip, ushort port)
{
    // definitions
    int sockfd;
    sockaddr_in servaddr;
    char buffer[100];

    // for debug
    printf("%s - %d\n", ip, port);

    // create and connect socket to server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("ERR: socket creation failed\n");
        exit(1);
    }
    bzero((void*) &servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);
    if (connect(sockfd, (const sockaddr *) &servaddr, sizeof(struct sockaddr_in)) != 0)
    {
        printf("ERR: connection failed\n");
        exit(1);
    }

    // sending infos to server

    while (true)
    {
        fgets(buffer, sizeof(buffer), stdin);
        bool emptyBuffer = false;

        // remove newline char
        for (int i = 0; i < sizeof(buffer); i++)
        {
            if (buffer[i] == '\n')
            {
                buffer[i] = '\0';
                if (i == 0) emptyBuffer = true;
                break;
            }
        }
        if (emptyBuffer) continue;

        // send to server
        int status = send(sockfd, (const void *) buffer, sizeof(buffer), 0);
        if (status == -1)
        {
            printf("ERR: send() failed\n");
            exit(1);
        }

        // done if typed exit
        if (strcmp(buffer, "exit") == 0)
        {
            return;
        }
    }

}

void udpCli(char* ip, ushort port)
{
    
}