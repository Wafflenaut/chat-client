Instructions for starting server:

Execute server by running:
	python3 chat-server.py <port number>
	e.g. python3 chat-server.py 21012

Instructions for starting client:

Compile client:
	gcc chat-client.c -o chat-client

Execute client by running:
	chat-client <server address> <port number>
	e.g. chat-client flip1.engr.oregonstate.edu 21012
	
Running/Controlling Server/Chat Client
	-Server must be started first
	-Client can then connect to server
	-Client will type their handle
	-After entering a valid  handle, client will send the first message to the server
	-Client/Server then alternate sending of messages
	-Either client/server user can quit using '\quit' command
	-When client/server quits, the client will exit and server will await a new connection

Notes:
-Server (aka Host A) Handle has been hard-coded as Calculon,
beloved star of the silicon screen as allowed per project specs.
-Initial testing conducted on localhost (of flip1/flip3),
	further testing conducted with chat client/server split on	
	flip1 and flip3

Works Cited/Referenced:
-Oregon State University CS372, Intro to Computer Networks,
	Lecture #15, Socket Programming Lecture/Slides, Paul Paulson
-Oregon State University CS344, Operating Systems I,
	Program 4, One Time Pad sample network program, client.c, Benjamin Brewster
-Oregon State University CS344, Operating Systems I,
	Program 4, One Time Pad completed project submission, John Moyer
-Python 3 Socket Library Documentation,
	https://docs.python.org/3/library/socket.html
-Computer Networking: A Top-Down Approach Seventh Edition,
	Kurose, James F., and Ross, Keith W.  Chapter 2.7 Socket Programming:
	Creating Network Applications

	

