// Israel Colin
// SCU COEN 146
//
//  This program implements the client side of stop & wait protocol
//  the client transfers a file to the server
//  gcc -o client client.c
//./client 8000 127.0.0.1
//  For the stop and wait protocolL, the assumptions are:
//       -- packet corruption, and packet loss

#include "tfv1.h"
#include <stdlib.h>
#include <arpa/inet.h>

// global variables
int state = 0; // only two states: 0 and 1
int sock;
struct sockaddr_in serverAddr;
socklen_t addr_size;

// list of functions
int main(int, char *[]);

// to reliably send a chunk of data
void my_send(char *, int);

// part of the my_send() function
void recv_ack(PACKET *, int);

// computes checksum using XOR of bytes
int calc_checksum(char *, int);

int main(int argc, char *argv[])
{
    char buff[SIZE];
    FILE *fp; // file to be sent to the server
    int r;

    if (argc != 5)
    {
        printf("usage: ./client port IP source_file dest_file\n");
        return 1;
    }

    // configure address
    inet_pton(AF_INET, argv[2], &serverAddr.sin_addr);
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_family = AF_INET;

    // create UDP socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        return 1;
    }
    printf("UDP socket has been created!\n");

    if ((fp = fopen(argv[3], "rb")) == NULL)
    {
        perror("file opening failed");
        return 1;
    }
    printf("Source file opened successfully!\n");

    printf("Sending file name...\n");

    my_send(argv[4], strlen(argv[4]));
    printf("Sent file name\n");

    printf("Now sending source file contents...");
    while ((r = fread(buff, sizeof(char), SIZE, fp)) > 0)
    {
        my_send(buff, r);
    }
    printf("Done!\n");

    // After transmitting the file, a packet with no data (len = 0) is sent to
    // notify the receiver that the file transfer has completed
    printf("Informing the server about the completion of file transmission...\n");
    my_send(buff, 0); // Don't really care what to send, as long as len = 0
    printf("Done!\n");

    fclose(fp);
    close(sock);

    return 0;
}

//***********************
// sends data (of size nbbytes) reliably to the sender
// **********************

void my_send(char *data, int nbytes)
{
    PACKET buf;
    int r;
    int cs;

    // Set the default checksum value to 0
    buf.header.checksum = 0;

    buf.header.len = nbytes;
    buf.header.seq_ack = state;
    // printf("Sequence number: %d\n",buf.header.seq_ack);

    // Add a null terminator to the end of the data string
    data[nbytes] = '\0';
    printf("Data: %s\n", data);
    strcpy(buf.data, data);

    // Simulate erroneous channel: corruption and loss
    // The probability of correct checksum is 80%
    r = rand() % 10;
    if (r <= 9)
        buf.header.checksum = calc_checksum((char *)&buf, sizeof(HEADER) + buf.header.len);
    else
        printf("\nPacket got corrupted on the way!\n");

    cs = state;

    printf("\tChecksum: %d\n", buf.header.checksum);
    printf("\tData: %s\n", buf.data);

    // The probability of no packet loss is 80%
    r = rand() % 10;
    if (r <= 9)
        sendto(sock, &buf, sizeof(HEADER) + nbytes, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    else
        printf("\nPacket got lost along the way!\n");

    // Wait for ACK
    recv_ack(&buf, nbytes);

    // If ACK not correctly received, retransmit data
    if (cs == state)
        my_send(data, nbytes);
    return;
}

//***********************
// wait for ack
// **********************
void recv_ack(PACKET *buf, int nbytes)
{
    HEADER receive_buf;
    int cs;
    int r;
    struct timeval tv;
    int rv;
    int counter = 0;

    fd_set readfds;

    fcntl(sock, F_SETFL, O_NONBLOCK);

    while (1)
    {
        while (1)
        {
            FD_ZERO(&readfds);
            FD_SET(sock, &readfds);

            tv.tv_sec = 1;
            tv.tv_usec = 0;

            rv = select(sock + 1, &readfds, NULL, NULL, &tv);

            if (rv == 1)
            {
                counter = 0;
                break;
            }
            else if (rv == 0)
            {
                printf("Timeout\n");
                counter++;

                if (counter >= 3)
                    return;
                continue;
            }
        }

        // Data to be read (ack received)
        recvfrom(sock, &receive_buf, sizeof(HEADER), 0, NULL, NULL);

        cs = receive_buf.checksum;
        receive_buf.checksum = 0;

        // Recalculate checksum of the received ack packet
        receive_buf.checksum = calc_checksum((char *)&receive_buf, sizeof(HEADER));

        printf("\tChecksum: %d\n", receive_buf.checksum);
        printf("\tSequence: %d\n", receive_buf.seq_ack);
        printf("\tCurrent state: %d\n", state);

        // A bad ack has been received
        if (cs != receive_buf.checksum || state != receive_buf.seq_ack)
        {
            printf("\nReceived bad packet\n");
            return;
        }

        // Resend packet (put into sending data)
        if (cs == receive_buf.checksum && state == receive_buf.seq_ack)
            state = !state;

        // Ack received, break the loop
        break;
    }
}

//***********************
// calculate checksum by using XOR
// **********************
int calc_checksum(char *buf, int tbytes)
{
    int i;
    char cs = 0;
    char *p;

    p = buf;
    for (i = 0; i < tbytes; ++i)
    {
        p = buf + i;
        cs ^= *p;
    }
    return (int)cs;
}