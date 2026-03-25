/* Common file for server & client */

#include "tftp.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

#define DATA_BYTES 512

void send_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, int mode_flag)
{
    int fd;
    int block = 1;
    int bytes;
    int size;
    tftp_packet pkt;
    tftp_packet ack;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        printf("File open failed\n");
        return;
    }

    if (mode_flag == 2)
        size = 1;          /* octet */
    else
        size = DATA_BYTES; /* default & netascii */

    while (1)
    {
        bytes = read(fd, pkt.body.data_packet.data, size);

        pkt.opcode = htons(DATA);
        pkt.body.data_packet.block_number = htons(block);

        sendto(sockfd, &pkt, bytes + 4, 0, (struct sockaddr *)&client_addr, client_len);

        printf("Sent packet %d with %d bytes\n", block, bytes);

        if (bytes < size)
        {
            recvfrom(sockfd, &ack, BUFFER_SIZE, 0, NULL, NULL);
            printf("ACK received for packet %d\n", block);
            printf("End of file sent\n");
            break;
        }

        recvfrom(sockfd, &ack, BUFFER_SIZE, 0, NULL, NULL);
        printf("ACK received for packet %d\n", block);

        block++;
    }

    close(fd);
}

void receive_file(int sockfd, struct sockaddr_in client_addr, socklen_t client_len, char *filename, int mode_flag)
{
    int fd;
    int bytes;
    int block = 1;
    int i;

    tftp_packet pkt;
    tftp_packet ack;

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        printf("File create failed\n");
        return;
    }

    while (1)
    {
        int n;

        n = recvfrom(sockfd, &pkt, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_len);

        bytes = n - 4;   // data size
        printf("Received packet %d data size %d\n", block, bytes);

        /* write data first */
        if (mode_flag == 3)
        {
            /* NetASCII */
            for (i = 0; i < bytes; i++)
            {
                if (pkt.body.data_packet.data[i] == '\n')
                {
                    write(fd, "\r\n", 2);
                }
                else
                {
                    write(fd, &pkt.body.data_packet.data[i], 1);
                }
            }
        }
        else
        {
            write(fd, pkt.body.data_packet.data, bytes);
        }

        /* send ACK */
        ack.opcode = htons(ACK);
        ack.body.ack_packet.block_number = htons(block);

        sendto(sockfd, &ack, 4, 0, (struct sockaddr *)&client_addr, client_len);

        printf("ACK sent for packet %d\n", block);

        if (bytes < 512)
        {
            printf("End of file received\n");
            break;
        }

        block++;
    }

    close(fd);
}