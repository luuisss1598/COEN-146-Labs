
# Behnam Dezfouli
# CSEN, Santa Clara University

# This program implements a simple web server that serves html and jpg files

# Input arguments:
# argv[1]: Sever's port number


from socket import *  # Import socket module
import sys            # To terminate the program
import os             # to be able to execute a system call


# IP address and port number
HOST = "127.0.0.1"
# PORT = 8011


def readHTML(fileName):
    with open(fileName, "r") as file:
        html_code = file.read()
        
        return html_code;
    
def readFile(fileName):
    with open(fileName, "r") as file:
        text_file = file.read()
                
        return text_file;
    

if __name__ == "__main__":

    # check if port number is provided
    if len(sys.argv) != 2:
        print('\nUsage: python %s <PortNumber>' % (sys.argv[0]))
        sys.exit()
        
    # save the port argument from the user
    PORT = int(sys.argv[1])
    
    """
    1. Create the socket
    2. Bind the socket
    3. Listen for connection
    4. Accept request
    5. receive data from client
    6. Send the data
    7. 
    
    """
    # ask the user to choose an option
    
    option = int(input("1: System network configuration \n2: Regular HTTP server\n"))
    
    # create the socket
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    
    # bind to port 8080
    serverSocket.bind((HOST, PORT));
    
    #listen to connections
    serverSocket.listen(5);
        
    # # Server should be up and running and listening to the incoming connections
    while True:
        print('The server is ready to respond to incoming requests...')
        
        # accept the incoming connections
        connection_socket, address = serverSocket.accept()
        print(f"Connection from {address} has been established!\n\n")
        
        try:
            
            # receive the message from the user then decode the message
            message = connection_socket.recv(1024).decode()
            
            #split the message in order to just get the extension 
            file_extension = message.split()[1].split(".")

            # Here we print the extension obtained from the client's request
            print("file extension message:", file_extension)
            
            if(option == 1):
                
                # create the header information
                dataHeader =  "HTTP/1.1 200 OK\n"
                dataHeader += "Server: SCU COEN Web Server\n"
                dataHeader += "Content-length: "
                
                # get the ifconfig information stored into a variable
                # system_info = os.system.run(["ifconfig"], capture_output=True, text=True)
                system_info = os.system("ifconfig > system_info.txt")
                
                # extract the text file data and store into command output to show the output of the command
                command_output = readFile("system_info.txt")
                dataHeader += str(len(command_output)) + "\n\n"
                
                #here we need to attach the overall command output into the header in order to send it back to the client
                dataHeader += command_output
                
                # send the completed header
                connection_socket.send(dataHeader.encode());    #encode the message
    
                # close the connection
                connection_socket.shutdown(SHUT_WR);
                connection_socket.close()
            
            # if user choose the second option then execute the following code
            elif(option == 2):
                
                # add the necessary headers in order to send back the complete header
                if (str(file_extension[1]) == 'html'):
                    dataHeader = "HTTP/1.1 200 OK\r\n"
                    dataHeader += "Content-Type: text/html; charset=utf-8\r\n"
                    dataHeader += "\r\n"
                    dataHeader += readHTML("index.html")
                    connection_socket.sendall(dataHeader.encode())
                elif (str(file_extension[1]) == 'jpg'):     #create a different content type based on the image that we are going to send back
                    dataHeader = "HTTP/1.1 200 OK\r\n"
                    dataHeader += "Content-Type: image/jpeg\r\n"
                    dataHeader += "Connection: close\r\n"
                    dataHeader += "Content-Length: " + str(os.path.getsize('scu_logo.jpg')) + "\r\n\r\n"
                    
                    #open the logo image in order to send back the image into the client
                    with open('scu_logo.jpg', 'rb') as image:
                        outputData = image.read()

                    # send all the information back to the client, but also encode the message
                    connection_socket.sendall(dataHeader.encode() + outputData)
                
                else:
                    #in case they choose the wrong option, let user know that is incorrect
                    print('Invalid file type, we only support html and jpg!')
                    with open(f'{str(file_extension[1])}', 'rb') as image:
                        outputData = image.read()
            
                # close the socket for that client
                connection_socket.shutdown(SHUT_WR);
                connection_socket.close()
                
            else:
                print("Wrong option...\n\n")
            
        #Raised when an I/O operation (such as a print statement, the built-in open() function or a method of a file object) fails for an I/O-related reason, e.g., "file not found" or "disk full"
        except IOError:
            dataHeaderERROR = "HTTP/1.1 404 Not Found\r\n\r\n";
            dataHeaderERROR += "<html><head></head><body><h1>404 Not Found</h1></body></html>"
            connection_socket.sendall(dataHeaderERROR.encode())
            connection_socket.close()

    serverSocket.close()
