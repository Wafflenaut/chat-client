from socket import *
import sys

#Parameters: A socket matched up with a client
#Function: Confirms valid connection with a client, indicates that data is correctly being communicated
#Output: True if expected client message was received, false if not
def handshake(clientSocket, serverHandle):
    serverConfirm = 'server-372'
    serverConfirm = serverConfirm.encode()

    clientConfirm = clientSocket.recv(1024)
    clientConfirm = clientConfirm.decode('utf8')

    if(clientConfirm == 'client-372'):
        clientSocket.send(serverConfirm)

        tempHandle = clientSocket.recv(256)
        tempHandle = tempHandle.decode('utf8')
        clientHandle = tempHandle

        tempHandle = serverHandle.encode()
        clientSocket.send(tempHandle)

        return clientHandle
    else:
        return ''

#Parameters: An outgoing message (string)
#Function: Verifies if a message is of the correct length (0 - 500)
#Output: True if within range, false if not
def verifyMessage(outgoingMessage):
    if(len(outgoingMessage) >= 0 and len(outgoingMessage) <= 500):
        return True
    else:
        return False

#Parameters: Socket with Client
#Function: Receives and formats message from client
#Output: Returns formatted message from client
def receiveMessage(clientSocket):
    incomingMessage = clientSocket.recv(1024)
    incomingMessage = incomingMessage.decode('utf8')

    return incomingMessage

#Parameters: Socket with client, server's handle
#Function: Reads Input from Client and sends
#Output: Returns True if valid message sent, returns false if /quit received
def sendMessage(clientSocket, serverHandle):
    validMessage = False
    while(not validMessage):
        outgoingMessage = input(serverHandle + "> ")
        if(outgoingMessage == '\quit'):
            outgoingMessage = outgoingMessage.encode()
            clientSocket.send(outgoingMessage)
            return False

        if(verifyMessage(outgoingMessage)):
            outgoingMessage = serverHandle + "> " + outgoingMessage
            outgoingMessage = outgoingMessage.encode()
            clientSocket.send(outgoingMessage)
            validMessage = True
            return True
        else:
            print('Error: Messages must be less than 500 characters')

#Parameters: socket with client, serverHandle
#Function: Alternate receiving/sending messages from/to a client
#Output: None
def establishChatSession(clientSocket, serverHandle, clientHandle):
    sessionActive = True
    waitingOnMessage = True

    print("Now chatting with " + clientHandle)

    while(sessionActive):
        #If waiting on message, receive a message
        if(waitingOnMessage):
            incomingMessage = receiveMessage(clientSocket)

            #If quit is received, end the session
            if(incomingMessage == '\quit'):
                sessionActive = False

                return True
            else:
                print(incomingMessage)
                waitingOnMessage = False
        #If not waiting, then send a message
        else:
            if(not sendMessage(clientSocket, serverHandle)):
                sessionActive = False
            else:
                waitingOnMessage = True


if(len(sys.argv) != 2):
    print('Server port must be passed in via argument, e.g. \'python3 chat-server.py 21012\'')
else:
    serverPort = int(sys.argv[1])
    serverSocket = socket(AF_INET, SOCK_STREAM)
    serverHandle = 'Calculon'
    clientHandle = ''

    serverSocket.bind(('', serverPort))
    serverSocket.listen(1)
    print('Server listening for connection on port: ' + str(serverPort))

    while True:
        clientSocket, address = serverSocket.accept()

        clientHandle = handshake(clientSocket, serverHandle)
        if(clientHandle != ''):
            establishChatSession(clientSocket, serverHandle, clientHandle)
            print("Ending chat with " + clientHandle)
        else:
            print("Invalid Client")

        clientSocket.close()
