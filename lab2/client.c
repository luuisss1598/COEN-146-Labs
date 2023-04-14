

//
//  Luis Herrera & Ana Amurrio
//  CSEN, Santa Clara University
// This program implements a client that connects to a server and transfers the
// bytes read from a file over the established connection
//
// The input arguments are as follows:
// argv[1]: Sever's IP address
// argv[2]: Server's port number
// argv[3]: Source file
// argv[4]: Destination file at the server side which includes a copy of the source

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#define SIZE 10

int main (int argc, char *argv[]){
    int sockfd = 0; // socket descriptor
    char net_buffer[SIZE]; // to hold the bytes read from socket
    char file_buffer[SIZE]; // to hold the bytes read from source file
    struct sockaddr_in serverAddress; // server address structure
    int net_bytes_read; // numer of bytes received over socket
    char clientMSG[] = "I am client";

    char serverIP_Address = atoi(argv[1]); //argv[1] = --> "127.0.0.1"

    FILE *source_file, *destination_file; // pointer to the source file

    if (argc < 5){
        printf ("Usage: ./%s <server ip address> <server port number> <source file>  <destination file>\n", argv[0]);
        return 1;
    }

    //open the source file
    source_file = fopen(argv[3], "r"); //argv[3] = source file
    if (source_file == NULL){
        printf ("Error opening source file\n");
        return 1;
    }

    //open the destination file
    destination_file = fopen(argv[4], "w"); // argv[4] = destination file

    // Make sure intial values are zero
    memset (net_buffer, '\0', sizeof(net_buffer));

    // memset (file_buffer, '0', sizeof (file_buffer));
    memset (&serverAddress, '\0', sizeof(serverAddress));

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)    //I need to get the socket from the client side
    {
        printf("Server1 error:\n");
        return 1;
    }

    printf("Client Socket created successfuly...\n");

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(atoi(argv[2])); //atoi(argv[1]) = port number

    inet_pton(AF_INET, &serverIP_Address, &serverAddress.sin_addr);

    //connect to the server
    int connected = connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    printf("Client1 connected to server %d\n", connected);

    //pass the output file to the server
    int passFileToServer = write(sockfd, argv[4], 17); //sending the outfput file to the server

    // int writeVar = write(sockfd, clientMSG, 17);
    printf("Client writing executed: %s %d\n", argv[4], passFileToServer);

    //server got output file
    int outputFile_Received = read(sockfd, net_buffer, 50);
    printf("Client %d received message from server: | %s | \n", outputFile_Received, net_buffer);
    

    while(fread(&net_buffer, 1, 1, source_file) == 1) 
    {
        //pass the content of the f1.txt to the server so it writes it down to f2.txt
        write(sockfd, net_buffer, 1);
    }

    printf("Client sent data\n\n");

    printf("Client is closing connection \n\n");
    close(sockfd);
    fclose(source_file);

    return 0;
}
