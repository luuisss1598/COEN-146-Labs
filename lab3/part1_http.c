//  Created by Behnam Dezfouli
//  CSEN, Santa Clara University
//

// This program implements a web server
//
// The input arguments are as follows:
// argv[1]: Sever's port number
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/stat.h> // to get file size

#define COMMAND_BUFF 15000	// Size of the buffer used to store the result of command execution
#define SOCK_READ_BUFF 4096 // Size of the buffer used to store the bytes read over socket
#define REPLY_BUFF 20000	// Size of the buffer used to store the message sent to client
#define FILE_READ 10

#define HTML_FILE "index.html"

// Socket descriptors
int socket_fd = 0;	   // socket descriptor
int connection_fd = 0; // new connection descriptor

void INThandler(int sig)
{
	char input;

	signal(sig, SIG_IGN);
	printf("Did you hit Ctrl-C?\n Do you want to quit? [y/n] ");

	input = getchar();
	input = getchar();

	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
}

// main function ---------------
int main(int argc, char *argv[])
{
	// Register a function to handle SIGINT ( SIGNINT is interrupt the process )
	signal(SIGINT, INThandler);

	int net_bytes_read;			   // numer of bytes received over socket
	struct sockaddr_in serv_addr;  // Address format structure
	char net_buff[SOCK_READ_BUFF]; // buffer to hold characters read from socket
	char message_buff[REPLY_BUFF]; // buffer to hold the message to be sent to the client

	char file_buff[FILE_READ]; // to hold the bytes read from source file
	FILE *source_file;		   // pointer to the source file

	// pointer to the file that will include the received bytes over socket
	FILE *dest_file;

	if (argc < 2)
	{ // Note: the name of the program is counted as an argument
		printf("Port number not specified!\n");
		return 1;
	}

	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	serv_addr.sin_family = AF_INET;				   // same as in the socket_fd
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // designate IP address of localhost
	serv_addr.sin_port = htons(atoi(argv[1]));	   // designate port from host to network

	if ((bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))) < 0)
	{
		perror("Failure to bind server address to the endpoint socket");
		exit(1);
	}

	// Create socket
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error: Could not create socket! \n");
		return 1;
	}

	// To prevent "Address in use" error
	// The SO_REUSEADDR socket option, which explicitly allows a process to bind to a port which remains in TIME_WAIT
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe

	// bind it to all interfaces, on the specified port number
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe
	// STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe STUDENT WORK ImplementMe WORK ImplementMe

	// Accept up to 1 connections
	if (listen(socket_fd, 1) < 0)
	{
		perror("Listen failed!");
		exit(EXIT_FAILURE);
	}

	printf("Listening to incoming connections... \n");

	unsigned int option = 0; // Variable to hold user option
	printf("1: System network configuration \n2: Regular HTTP server\n");
	scanf("%u", &option);

	// The web server returns current processor and memory utilization
	if (option == 1)
	{
		printf("System network configuration (using ifconfig)... \n");
	}
	// The requested file is returned
	else if (option == 2)
	{
		printf("Regular server (only serving html files)... \n\n");
	}
	else
		printf("Invalid Option! \n\n");

	while (1)
	{
		printf("Inside option 1 while loop...\n");
		// Accept incoming connection request from client
		if ((connection_fd = accept(socket_fd, (struct sockaddr *)NULL, NULL)) < 0)
		{
			perror("In accept failed!");
			exit(EXIT_FAILURE);
		}

		printf("Incoming connection: Accepted! \n");

		memset(net_buff, '\0', sizeof(net_buff));

		// Return system utilization info
		if (option == 1)
		{
			printf("inside\n");
			// run a command -- we run ifconfig here (you can try other commands as well)
			FILE *system_info = popen("ifconfig", "r");
			char tmp_buff[COMMAND_BUFF];
			char command_output[COMMAND_BUFF];
			memset(command_output, '\0', sizeof(command_output));
			while (fgets(tmp_buff, sizeof(tmp_buff), system_info) != NULL)
			{															   // get outpout into tmp+buff
				strcpy(command_output + strlen(command_output), tmp_buff); // copy tmp_buff into command_output
			}
			pclose(system_info);

			strncpy(message_buff, "HTTP/1.1 200 OK\n", strlen("HTTP/1.1 200 OK\n"));
			strncpy(message_buff + strlen(message_buff), "Server: SCU COEN Web Server\n", strlen("Server: SCU COEN Web Server\n"));
			strncpy(message_buff + strlen(message_buff), "Content-length: ", strlen("Content-length: "));

			char content_length[10] = {'0'};
			printf(content_length, "%d\n", strlen(command_output));

			strncpy(message_buff + strlen(message_buff), content_length, strlen(content_length));
			strncpy(message_buff + strlen(message_buff), "Content-type: text/plain\n", strlen("Content-type: text/plain\n"));
			strncpy(message_buff + strlen(message_buff), "\r\n", strlen("\r\n"));

			strncpy(message_buff + strlen(message_buff), command_output, strlen(command_output));

			printf("Message prepared: \n%s\n", message_buff);

			// Reply to client
			write(connection_fd, message_buff, strlen(message_buff));
			printf("Reply sent to client!\n");
			memset(message_buff, '\0', sizeof(message_buff));
		}

		else if (option == 2)
		{
		}
		shutdown(connection_fd, SHUT_RDWR);
		close(connection_fd);
	}

	close(socket_fd);
}