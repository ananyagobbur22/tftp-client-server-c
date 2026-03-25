#include "tftp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet);

int main() 
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    tftp_packet packet;


    // Create UDP socket

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    {
        perror("Socket creation failed");
        return 1;
    }


    // Set socket timeout option
    //TODO Use setsockopt() to set timeout option

    
    // Set up server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
     if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) 
    {
        perror("Bind failed");
        close(sockfd);
        return 1;
    }

    printf("TFTP Server listening on port %d...\n", PORT);

    // Main loop to handle incoming requests
    while (1) 
    {
        int n = recvfrom(sockfd, &packet, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);
        

         handle_client(sockfd, client_addr, client_len, &packet);
    }

    close(sockfd);
    return 0;
}

void handle_client(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, tftp_packet *packet) 
{
    // Extract the TFTP operation (read or write) from the received packet
    // and call send_file or receive_file accordingly
    int opcode;
    int mode_flag;

    opcode = ntohs(packet->opcode);

    /* read mode sent by client */
    mode_flag = packet->body.request.mode[0] - '0';

    if (mode_flag < 1 || mode_flag > 3)
    {
        mode_flag = 1;   /* default mode */
    }

    /* PUT operation */
    if (opcode == WRQ)
    {
        printf("WRQ received for file %s (mode %d)\n", packet->body.request.filename, mode_flag);

        receive_file(sockfd,client_addr, client_len, packet->body.request.filename, mode_flag);
    }
    /* GET operation */
    else if (opcode == RRQ)
    {
        printf("RRQ received for file %s (mode %d)\n", packet->body.request.filename, mode_flag);

        send_file(sockfd, client_addr, client_len, packet->body.request.filename, mode_flag);
    }
    else
    {
        printf("Unknown request received\n");
    }
}