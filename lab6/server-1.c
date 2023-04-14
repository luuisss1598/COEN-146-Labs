#include "tfv1.h"

int state = 0; // we only have two states: 0 and 1
int sock;
struct sockaddr_in serverAddr;

// list of functions
int main(int, char *[]);
int my_receive(PACKET *);
int calc_checksum(char *, int);

// list of functions
int main(int argc, char *argv[])
{
    FILE *fp;
    int n;
    PACKET buf;
    socklen_t addr_size = sizeof(serverAddr);
    if (argc != 2)
    {
        printf("Need port number\n");
        return 1;
    }

    // STUDENT WORK
    memset(&serverAddr, 0, sizeof(serverAddr)); // initialize struct to 0
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(atoi(argv[1]));
    serverAddr.sin_family = AF_INET;

    printf("%d \n", atoi(argv[1]));

    // create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("socket error\n");
        return 1;
    }

    // bind
    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
    {
        printf("bind error\n");
        return 1;
    }

    memset(buf.data, 0, sizeof(buf.data)); // initialize buffer to 0
    if ((n = my_receive(&buf)) <= 0)
    {
        printf("could not get the name of the file\n");
        return 1;
    }
    printf("File name has been received!\n");

    // Open file with received name
    if ((fp = fopen(buf.data, "wb")) == NULL)
    {
        printf("error fopen\n");
        return 1;
    }

    printf("Receiving file %s ... \n", buf.data);
    while ((n = my_receive(&buf)) > 0)
    {
        printf("writing to file... n = %d\n", n);
        // STUDENT WORK
        fwrite(buf.data, sizeof(char), n, fp);
    }
    close(sock);
    fclose(fp);

    return 0;
}

int my_receive(PACKET *recv_pck)
{
    int cs_in;
    int cs_calc;
    int d;
    int r;
    int nbytes;
    HEADER ack_packet;
    struct sockaddr ret_addr;
    socklen_t addr_size = sizeof(ret_addr);

    while (1)
    {
        printf("Trying to receive\n");

        // clear out the buffer before receiving
        memset(recv_pck->data, '\0', sizeof(recv_pck->data));

        // receive the packet
        if ((nbytes = recvfrom(sock, recv_pck, sizeof(PACKET), 0, &ret_addr, &addr_size)) < 0)
        {
            printf("Failed to receive packet\n");
            return 0;
        }

        // clear out any garbage data in the buffer
        memset(recv_pck->data + recv_pck->header.len, '\0', sizeof(recv_pck->data) - recv_pck->header.len);

        printf("Received a UDP packet!\n");
        printf("\tchecksum: %d\n", recv_pck->header.checksum);
        printf("\tdata: %s\n", recv_pck->data);

        cs_in = recv_pck->header.checksum;
        recv_pck->header.checksum = 0;

        // recalculate checksum
        cs_calc = calc_checksum((char *)recv_pck, sizeof(HEADER) + recv_pck->header.len);

        // Check good checksum and good ack
        if (cs_in == cs_calc && recv_pck->header.seq_ack == state)
        {
            printf("Checksum passed! Sequence number matched!\n");

            // create an acknowledgement packet
            ack_packet.checksum = 0;
            ack_packet.seq_ack = state;
            ack_packet.len = 0;
            ack_packet.checksum = calc_checksum((char *)&ack_packet, sizeof(HEADER) + ack_packet.len);

            // simulating erroneous channel...corruption and loss
            r = rand() % 10;
            if (r > 9)
            {
                ack_packet.checksum = 0;
                printf("\nPacket got corrupted on the way\n");
            }

            printf("\tChecksum: %d\n", ack_packet.checksum);
            printf("\tSequence: %d\n", ack_packet.seq_ack);

            r = rand() % 10;
            if (r <= 9)
            {
                // send the acknowledgement packet
                sendto(sock, &ack_packet, sizeof(HEADER), 0, (struct sockaddr *)&ret_addr, sizeof(ret_addr));
            }
            else
            {
                printf("\n\n\n\nPacket got lost along the way\n\n\n\n");
            }

            // now we are expecting the next packet
            state = !state;
            return recv_pck->header.len;
        }
        else
        {
            printf("Checksum/sequence number check failed!\n");

            // create an acknowledgement packet for the other sequence number
            ack_packet.checksum = 0;
            ack_packet.seq_ack = !state;
            ack_packet.len = 0;
            ack_packet.checksum = calc_checksum((char *)&ack_packet, sizeof(HEADER) + ack_packet.len);

            printf("Resending ack for sequence number: %d...\n", ack_packet.seq_ack);

            // simulating erroneous channel...corruption and loss
            r = rand() % 10;
            if (r > 9)
            {
                ack_packet.checksum = 0;
                printf("\n\n\n\nPacket got corrupted along the way\n\n\n\n");
            }

            printf("\tChecksum: %d\n", ack_packet.checksum);
            printf("\tSequence %d\n", ack_packet.seq_ack);
            r = rand() % 10;
            if (r <= 9)
                sendto(sock, &ack_packet, sizeof(HEADER), 0, &ret_addr, sizeof(ret_addr));
            else
                printf("\n\n\n\nPacket got corrupted along the way\n\n\n\n");
        }
    }

    return -1;
}
int calc_checksum(char *buf, int tbytes)
{
    int i;
    char cs = 0;
    char *p;

    for (i = 0; i < tbytes; i++)
    {
        cs ^= buf[i];
    }

    return cs;
}
