// gcc -o Client client.c -pthread

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_LEN 1024

void *new_socket(void *args)
{
    int *arg = (int *)args;
    int port = *arg;

    int sd, rc, i, valread;
    char buffer[MAX_LEN] = {0};
    struct sockaddr_in servAddr;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr);

    /* create socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);

    /* connect to server */
    rc = connect(sd, (struct sockaddr *)&servAddr, sizeof(servAddr));

    /* handle connection */
    if (port >= 20000 && port <= 29999)
    {

        memset(buffer, 0x0, MAX_LEN); // reset
        valread = read(sd, buffer, MAX_LEN - 1);

        printf("[PORT %d] Received: %s\n", port, buffer);

        /* send response */
        char *response = "Bye ~";
        send(sd, response, strlen(response), 0);
    }
    else if (port >= 10000 && port <= 19999)
    {

        /* send message */
        char firstname[100], lastname[100], date_of_birth[10];
        printf("[PORT %d] Input Firstname: ", port);
        scanf("%s", firstname);
        printf("[PORT %d] Input Lastname: ", port);
        scanf("%s", lastname);
        printf("[PORT %d] Input Date of Birth: ", port);
        scanf("%s", date_of_birth);

        char data[MAX_LEN];
        strcpy(data, firstname);
        strcat(data, " ");
        strcat(data, lastname);
        strcat(data, ",");
        strcat(data, date_of_birth);

        send(sd, data, strlen(data), 0);

        /* receive message */
        valread = read(sd, buffer, MAX_LEN - 1);
        printf("[PORT %d] Response: %s\n", port, buffer);
    }
    else
    {
        printf("port number out of range !");
    }
}

int main(int argc, char *argv[])
{
    int port2 = 22502;
    pthread_t thread_id2;
    pthread_create(&thread_id2, NULL, new_socket, (void *)&port2);

    int port1 = 12502;
    pthread_t thread_id1;
    pthread_create(&thread_id1, NULL, new_socket, (void *)&port1);

    pthread_exit(NULL);
}