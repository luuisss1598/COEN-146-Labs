
# Behnam Dezfouli
#  CSEN, Santa Clara University

# This program implements a simple smtp mail client to send an email to a local smtp server

# Note:
# Run a local smtp mail server using the following command before running this code:
# python -m smtpd -c DebuggingServer -n localhost:6000


from socket import *
import ssl
import sys


# Choose a mail server
mailserver = 'localhost'


# Create socket called clientSocket and establish a TCP connection with mailserver
# STUDENT WORK

# 6000
# PORT = int(sys.argv[1])
HOST = "localhost"

clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect(("", 6000))

recv1 = clientSocket.recv(1024).decode()
print(recv1)
if recv1[:3] != '220':
    print('220 reply not received from server.')


# Send HELO command along with the server address
HELOsend = "HELO localhost.com\r\n".encode()
clientSocket.send(HELOsend)
recv2 = clientSocket.recv(1024).decode()
print(recv2)
if recv2[:3] != '250':
    print('220 reply not received from server.')



# Send MAIL FROM command and print server response
MAIL_from = "MAIL FROM: <lherrera@scu.edu> \r\n".encode()
clientSocket.send(MAIL_from)
recv3 = clientSocket.recv(1024).decode()
print(recv3)
if recv3[:3] != '250':
    print('220 reply not received from server.')

# Send RCPT TO command and print server response
RCPT_to = "RCPT TO: <luisherrera722@gmail.com>".encode()
clientSocket.send(RCPT_to)
recv4 = clientSocket.recv(1024).decode()
print(recv4)
if recv4[:3] != '250':
    print('220 reply not received from server.')

# 
DATA_send = "DATA\r\n".encode()
print(DATA_send)
clientSocket.send(DATA_send)
recv8 = clientSocket.recv(1024).decode()
print(recv8)
if recv8[:3] != '250':
    print('220 reply not received from server.')



# Send DATA command and print server response
SUBJECT_send = "SUBJECT: Greeting to you!\r\n".encode()
body = "Hi, my name is Luis \n".encode()
dot = "\r\n.\r\n".encode()

print(SUBJECT_send)
print(DATA_send)
print(body)
print(dot)

clientSocket.send(SUBJECT_send)
clientSocket.send(body)
clientSocket.send(dot)

recv5 = clientSocket.recv(1024).decode()
print(recv5)
if recv5[:3] != '250':
    print('220 reply not received from server.')

# Send QUIT command and get server response
QUIT_msg = "QUIT\r\n".encode()
clientSocket.send(QUIT_msg)
recv10 = clientSocket.recv(1024).decode()
print(recv10)
if recv10[:3] != '221':
    print('220 reply not received from server.')
