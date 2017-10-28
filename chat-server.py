from socket import *
import sys

def verifyMessage(outgoingMessage):
    if(len(outgoingMessage) >= 0 and len(outgoingMessage <= 500)):
        return True
    else:
        return False

def handshake(clientSocket):
    serverConfirm = 'server-372\0'
    clientConfirm = clientSocket.recv(1024)

    if(clientConfirm == 'client-372'):
        clientSocket.send(serverConfirm)
        return True
    else:
        return False


if(len(sys.argv) != 2):
    print('Server port must be passed in via argument, e.g. \'python3 chat-server.py 21012\'')
else:
    serverAddress = 'localhost'
    serverPort = int(sys.argv[1])
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverHandle = 'Calculon'

    serverSocket.bind((serverAddress, serverPort))
    serverSocket.listen(1)
    print('Server listening for connection on port: ' + str(serverPort))

    while True:
		clientSocket, address = serverSocket.accept()

		message = clientSocket.recv(1024)
		message.decode('utf8')
		print(message)