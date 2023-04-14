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
#define PORT 8080

int main(int argc, char *argv[])
{
    char message[20] = "Received!";
    int socket_fd = 0;
    int connection_fc = 0;
    struct sockaddr_in serverAddress;
    char net_buffer[1024];
    FILE *destination_file;

    memset(&serverAddress, '0', sizeof(serverAddress));
    memset(net_buffer, '0', sizeof(net_buffer));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(atoi(argv[1]));

    printf("Listining on Port: %d\n", atoi(argv[1]));

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) // I need to get the socket from the client side
    {
        printf("Server1 error:\n");
        return 1;
    }

    int bindPort = bind(socket_fd, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)); // I need to bind the socket into the Network address and port
    printf("Bind to port number: %d...\n", bindPort);

    //listen to the port
    int listenPort = listen(socket_fd, 10);
    printf("Server Listening: %d\n", listenPort);
    
    connection_fc = accept(socket_fd, (struct sockaddr *)NULL, NULL);
    printf("Server connected: %d\n", connection_fc);

    // read the outfile received from the client
    int readOutpuFile = read(connection_fc, net_buffer, 17);
    printf("Server received outputfile from client: |%s|\n", net_buffer);

    destination_file = fopen(net_buffer, "w");

    // respond back to the client stating that outfile was received
    write(connection_fc, &message, 20);
    printf("Server Message to client: %s\n", message);

    //erases the data in the n bytes
    bzero(net_buffer, 20);
    char empty[SIZE];

    while (read(connection_fc, empty, 1) > 0)
    {
        //write down the data from the source file into the destination file
        fwrite(empty, 1, 1, destination_file);
    }

    close(connection_fc);
    fclose(destination_file);
    printf("Closing the connection...\n\n");

    return 0;
}