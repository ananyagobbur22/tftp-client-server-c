#include "tftp.h"
#include "tftp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

/* Global mode flag */
int current_mode_flag = 1;

int main()
{
    char command[256];
    tftp_client_t client;

    /* Initialize client structure */
    memset(&client, 0, sizeof(client));
    client.mode_flag = 1;   /* default mode */

    /* Main loop for command-line interface */
    while (1)
    {
        printf("\n1. Connect\n");
        printf("2. Put\n");
        printf("3. Get\n");
        printf("4. Mode\n");
        printf("5. Exit\n");
        printf("Enter choice: ");

        fgets(command, sizeof(command), stdin);

        /* CONNECT OPERATION */
        if (command[0] == '1')
        {
            char ip[20];
            int port;

            printf("Enter IP address: ");
            scanf("%s", ip);

            printf("Enter port number: ");
            scanf("%d", &port);

            connect_to_server(&client, ip, port);
        }

        /* PUT */
        else if (command[0] == '2')
        {
            char filename[256];

            printf("Enter filename to PUT: ");
            scanf("%s", filename);
            getchar();

            put_file(&client, filename);
        }

        /* GET */
        else if (command[0] == '3')
        {
            char filename[256];

            printf("Enter filename to GET: ");
            scanf("%s", filename);
            getchar();

            get_file(&client, filename);
        }

        /* MODE */
        else if (command[0] == '4')
        {
            printf("\nSelect Mode:\n");
            printf("1. Default (512 bytes)\n");
            printf("2. Octet (1 byte)\n");
            printf("3. NetASCII\n");
            printf("Enter mode: ");

            scanf("%d", &client.mode_flag);
            //getchar();

            if (client.mode_flag < 1 || client.mode_flag > 3)
            {
                printf("Invalid mode\n");
            }
            else
            {
                /* update global mode flag */
                current_mode_flag = client.mode_flag;
            }
        }

        /* EXIT */
        else if (command[0] == '5')
        {
            disconnect(&client);
            printf("Client exiting...\n");
            break;
        }

        else
        {
            printf("Invalid option\n");
        }
    }

    return 0;
}

/* Initialize UDP socket and server address */
void connect_to_server(tftp_client_t *client, char *ip, int port)
{
    client->sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    client->server_addr.sin_family = AF_INET;
    client->server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &client->server_addr.sin_addr);

    client->server_len = sizeof(client->server_addr);

    printf("Connected to server %s:%d\n", ip, port);
}

/* PUT operation */
void put_file(tftp_client_t *client, char *filename)
{
    /* send write request */
    send_request(client->sockfd, client->server_addr, filename, WRQ);

    /* send actual file data */
    send_file(client->sockfd,  client->server_addr,  client->server_len,  filename, client->mode_flag);
}

/* GET operation */
void get_file(tftp_client_t *client, char *filename)
{
    /* send read request */
    send_request(client->sockfd,   client->server_addr,   filename,   RRQ);

    /* receive file data */
    receive_file(client->sockfd, client->server_addr, client->server_len, filename,client->mode_flag);
}

/* Disconnect client */
void disconnect(tftp_client_t *client)
{
    close(client->sockfd);
}

/* Send RRQ / WRQ packet */
void send_request(int sockfd, struct sockaddr_in server_addr, char *filename, int opcode)
{
    tftp_packet packet;

    packet.opcode = htons(opcode);
    strcpy(packet.body.request.filename, filename);

    /* set mode using global flag */
    if (current_mode_flag == 1)
        strcpy(packet.body.request.mode, "1");
    else if (current_mode_flag == 2)
        strcpy(packet.body.request.mode, "2");
    else if (current_mode_flag == 3)
        strcpy(packet.body.request.mode, "3");

    sendto(sockfd, &packet, sizeof(packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

    printf("Request sent for file %s\n", filename);
}

/* Not used on client side (kept for skeleton completeness) */
void receive_request(int sockfd, struct sockaddr_in server_addr, char *filename,  int opcode)
{
    
}
