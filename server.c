// gcc -o Server server.c -pthread

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

/* shared variable */
int is_convert_year_finished = 0;
int christ_year = -1;

void *new_socket(void *args)
{
    int *arg = (int *)args;
    int port = *arg;

    int sd, newSd, cliLen, valread;
    char buffer[MAX_LEN] = {0};
    struct sockaddr_in cliAddr, servAddr;
    cliLen = sizeof(cliAddr);

    /* create socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);

    /* bind server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    bind(sd, (struct sockaddr *)&servAddr, sizeof(servAddr));

    listen(sd, 5);

    while (1)
    {
        /* accept connection */
        newSd = accept(sd, (struct sockaddr *)&cliAddr, &cliLen);

        /* handle connection */
        if (port >= 20000 && port <= 29999)
        {

            while (1)
            {
                if (is_convert_year_finished)
                {

                    /* send response */
                    char response[5];
                    sprintf(response, "%d", christ_year);
                    send(newSd, response, strlen(response), 0);

                    /* read message */
                    memset(buffer, 0x0, MAX_LEN); // reset
                    valread = read(newSd, buffer, MAX_LEN - 1);
                    printf("[PORT %d] Received: %s\n", port, buffer);

                    is_convert_year_finished = 0; // reset flag
                    break;
                }
            }
        }
        else if (port >= 10000 && port <= 19999)
        {

            /* read message */
            memset(buffer, 0x0, MAX_LEN); // reset
            valread = read(newSd, buffer, MAX_LEN - 1);

            /* slice string */
            int delim_index = strchr(buffer, ',') - buffer;

            char result_year[5];
            strncpy(result_year, buffer + delim_index + 5, 4);
            int year;
            sscanf(result_year, "%d", &year);

            char result_name[200];
            strncpy(result_name, buffer, delim_index);
            printf("[PORT %d] Received: %s\n", port, result_name);

            /* send response */
            char *response = "Server received !";
            send(newSd, response, strlen(response), 0);

            /* convert year & update shared variables */
            christ_year = year - 543;
            is_convert_year_finished = 1; // set flag to true
        }
        else
        {
            printf("port number out of range !");
        }
    }
}

int main(int argc, char *argv[])
{
    int port1 = 12502;
    pthread_t thread_id1;
    pthread_create(&thread_id1, NULL, new_socket, (void *)&port1);

    int port2 = 22502;
    pthread_t thread_id2;
    pthread_create(&thread_id2, NULL, new_socket, (void *)&port2);

    pthread_exit(NULL);
}