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

struct threadArg
{
    char buffer[100];
    int length;
};

pthread_mutex_t fileMutex;

void * writeToFile(void*);

int main()
{
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero((void*) &servaddr, sizeof(struct sockaddr_in));
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(9999);

    int binder = bind(sockfd, (const sockaddr *) &servaddr, sizeof(struct sockaddr_in));
    if (binder == -1)
    {
        printf("ERR: bind err\n");
        close(sockfd);
        exit(1);
    }

    pthread_mutex_init(&fileMutex, NULL);

    for (;;)
    {
        pthread_t thread;
        struct threadArg * arg = new threadArg;

        // recv data
        arg->length = recvfrom(sockfd, (void*) arg->buffer, sizeof(arg->buffer), 0, NULL, NULL);
        if (arg->length == -1)
        {
            printf("ERR: recv error\n");
            delete arg;
            continue;
        }

        // thread to handle log entry
        if (pthread_create(&thread, NULL, writeToFile, (void*) arg) != 0)
        {
            printf("ERR: thread err\n");
            delete arg;
            continue;
        }
        pthread_detach(thread);
    }

    close(sockfd);
    return 0;
}

/**
 * argv->buffer has the form {"<msg>" was received from <IP>}
 */

void* writeToFile(void* argv)
{
    struct threadArg *arg = (struct threadArg *) argv;
    char fileInput[150];
    
    // get curr time in cstr format
    char timeStr[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);

    strcpy(fileInput, timeStr);
    strcat(fileInput, "\t");
    strcat(fileInput, arg->buffer);

    pthread_mutex_lock(&fileMutex);

    FILE *file = fopen("echo.log", "a");
    if (!file) {
        printf("ERR: opening/creating file echo.log error\n");
        pthread_mutex_unlock(&fileMutex); // Unlock the mutex if file open fails
        return NULL;
    }

    printf("%s\n", fileInput);
    fprintf(file, "%s\n", fileInput);
    fclose(file);

    pthread_mutex_unlock(&fileMutex);

    delete arg;
    return NULL;
}