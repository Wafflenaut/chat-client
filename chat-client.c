#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

typedef int bool;
#define true 1
#define false 0
#define smallBuffer 256
#define messageBuffer 1024

//Parameters: Error Message string
//Function:  Prints error message
//Output: Prints error message
void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

//Parameters: Socket, message to send
//Function:  Sends a message to the server
//Output:  Message is sent to the server
void sendMessage(int serverFD, char* message){
	int charsWritten = 0;
	// Send message to server
	charsWritten = send(serverFD, message, strlen(message), 0); // Write to the server
	if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
	if (charsWritten < strlen(message)) printf("CLIENT: WARNING: Not all data written to socket!\n");	
}

//Parameters: Socket, pointer to string for received message
//Function:  Gets message from the server
//Output:  Receives and stores message from the server
void getMessage(int serverFD, char* message){
	int charsRead = 0;
	// Get return message from server
	memset(message, '\0', sizeof(message)); // Clear out the buffer again for reuse
	charsRead = recv(serverFD, message, messageBuffer - 1, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 0) error("CLIENT: ERROR reading from socket");
	
}

//Parameters: Socket and client's handle
//Function:  Confirms connection, receives server's handle, sends client's handle
//	This servers as the 'initial message' prior to user input message
//Output:  Sends client's handle
int handshake(int serverFD, char* clientHandle, char* serverHandle){
	char message[messageBuffer];
	
	//sends client-message to server
	memset(message, '\0', sizeof(message));
	strcpy(message, "client-372");
	sendMessage(serverFD, message);
	
	//receives server-message
	memset(message, '\0', sizeof(message));
	getMessage(serverFD, message);
	
	if(strcmp(message, "server-372") == 0){
		memset(message, '\0', sizeof(message));
		strcpy(message, clientHandle);
		sendMessage(serverFD, message);
		
		memset(message, '\0', sizeof(message));
		getMessage(serverFD, message);
		strcpy(serverHandle, message);
		return true;
	}
	else{
		return false;
	}

}

//Parameters: socket and client-handle
//Function: Initiates a chat session of alternating client/server chat messages
//Output:  Outputs received chat messages
int initiateChatSession(int serverFD, char* clientHandle, char* serverHandle){
	int sessionActive = true;
	int waitingOnMessage = false;
	int validMessage = false;
	char message[messageBuffer];
	char tempMessage[messageBuffer];
	
	memset(message, '\0', sizeof(message));
	
	printf("Starting the chat session with %s\n", serverHandle);
	
	while(sessionActive){
		if(waitingOnMessage){
			memset(message, '\0', sizeof(message));
			getMessage(serverFD, message);
			if(strcmp(message, "\\quit") == 0){
				sessionActive = false;
				return true;
			}
			else{
				printf("%s\n", message);
				waitingOnMessage = false;
			}
		}
		else{
			//add the while loop to validate input
			//check length
			validMessage = false;
			while(!validMessage){
				memset(message, '\0', sizeof(message)); // Clear out the buffer array
				printf("%s> ", clientHandle);
				fgets(message, sizeof(message) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0
				message[strcspn(message, "\n")] = '\0'; // Remove the trailing \n that fgets adds
				
				if(strcmp(message, "\\quit") == 0){
					sendMessage(serverFD, message);
					validMessage = true;
					sessionActive = false;
					return true;
				}
				
				if(strlen(message) > 500 || strlen(message) < 0){
					printf("Messages must be from 0 to 500 characters.  Please enter a valid message.\n");
				}
				else{
					//Construct the outgoing message with client handle
					memset(tempMessage, '\0', sizeof(tempMessage)); // Clear out the buffer array
					strcpy(tempMessage, clientHandle);
					strcat(tempMessage, "> ");
					strcat(tempMessage, message);
					sendMessage(serverFD, tempMessage);

					waitingOnMessage = true;
					validMessage = true;
				}
			}
		}

	}
}

//Parameters: Address for handle string
//Function: Gets and validates the client's handle
//Output:  Stores client's handle
void getHandle(char* handle){
	bool validHandle = false;
	char* handleInput = NULL;
	char *token;
	int numChars = 0;
	size_t bufferSize = 0;
	memset(&handleInput, '\0', sizeof(handleInput));
	
	while(validHandle == false){
		printf("Enter your chat handle (1-10 characters): ");
		
		numChars = getline(&handleInput, &bufferSize, stdin);
		if(numChars > 10){
			
			printf("Your handle must be less than 10 characters\n");
			memset(&handleInput, '\0', sizeof(handleInput));
		}
		else if(strlen(handleInput) == 0){
			printf("Your handle must be at least 1 character\n");
			memset(&handleInput, '\0', sizeof(handleInput));
		}
		else if(numChars < 0){
			printf("Input Error: Please re-enter\n");
			clearerr(stdin);
		}
		else{
			validHandle = true;
		}
	}

	//tokenize the string prior to the newline character
    token = strtok(handleInput, "\n");

    //only copies if token is not null (string wasn't empty)
    //strcopying a null value causes a segmentation fault
    if (token != NULL) {
        strcpy(handle, token);
    }

}



int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	int validHandshake;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char buffer[messageBuffer];
	char clientHandle[smallBuffer];
	char serverHandle[smallBuffer];
	
	memset(&clientHandle, '\0', sizeof(clientHandle));
	memset(&serverHandle, '\0', sizeof(serverHandle));
    
	if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args
	
	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");
		
	getHandle(clientHandle);
		
	validHandshake = handshake(socketFD, clientHandle, serverHandle);
	if(validHandshake == false){
		printf("Handshake Failed - Invalid Server\n");
		exit(1);
	}
	
	initiateChatSession(socketFD, clientHandle, serverHandle);

	printf("Ending chat with %s and exiting.\n", serverHandle);
	close(socketFD); // Close the socket

	return 0;
}
