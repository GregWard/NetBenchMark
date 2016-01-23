/*
* FILE:			serverFunctions.c
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 16, 2015
*
* DESCRIPTION:	This program file contains the server/sender specific functions. The server/sender will wait for a connection
				from a client/receiver program and then execute the test when they connect. Otherwise the server will timeout
				after 5 seconds of no activity. After each test, a summary is printed and the server/sender will wait for
				another connection in case multiple PC's were being tested.
*/



#include "../headers/allHeaders.h"
#include <time.h>


int benchmarkSend(int blockSize, int testTime, SOCKET sock, struct sockaddr_in client, struct TestResults* test);
void setBlockHeader(char* block, int* packetsSent);
char* createTestBlock(int blockSize);





/*
FUNCTION:		int server

DESCRIPTION:	This function is the main part of the sender or server of the test. The server will wait until it receives
				notification from a client/receiver that they want to connect and test with them. The server will only wait
				for five seconds for a connection, after which time it will shut down gracefully. select is used to see
				if there is activity on the socket for both UDP and TCP. If using TCP, the program will then accept connection.
				The benchmark test is run, upon completion, the server will go back and wait again for a client.

PARAMETERS:		int blockSize		- The size of the blocks to be sent for the test
				int numBlocks		- The number of blocks to be sent for the test
				int sockType		- The protocol type we are using
				SOCKET serverSock	- The socket the server/sender will be using

RETURNS:		int errors			- Indicates whether or not server completed gracefully or not
*/





int server(int blockSize, int numBlocks, int sockType, SOCKET serverSock)
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	SOCKET clientSock = 0;
	int headerSize = 0;
	int errors = kNoError;
	struct timeval timeout;
	fd_set socketsToRead;

	//for select, to setup the file descriptors
	FD_ZERO(&socketsToRead);
	FD_SET(serverSock, &socketsToRead);

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(kDefaultPort);

	//set timeout to be 5 seconds
	timeout.tv_sec = 5;
	timeout.tv_usec = 0;

	int timedOut = 1;

	if ((bind(serverSock, (struct sockaddr*)&server, sizeof(server))) == 0)
	{
		while (timedOut > 0)
		{
			if (sockType == SOCK_STREAM)
			{
				//listen for one connection
				if (listen(serverSock, 1) == 0)
				{
					headerSize = sizeof(struct sockaddr_in);

					printf("Listening for new clients...will timeout in five seconds\n\n");
					if ((timedOut = select(serverSock + 1, &socketsToRead, NULL, NULL, &timeout)) > 0)
					{
						clientSock = accept(serverSock, (struct sockaddr*)&client, &headerSize);

						if (clientSock != INVALID_SOCKET)
						{
							printf("Server Accepted Connect\n");
						}
						else
						{
							errors = kAcceptError;
						}
					}
					else
					{
						if (timedOut == 0)
						{
							printf("Select Timed Out\n\n");
							errors = kTimeoutError;
						}
						else
						{
							printf("Error with Select...\n\n");
							errors = kGeneralError;
						}
					}
				}
				else
				{
					errors = kListeningError;
				}

			}
			else //we are using UDP
			{
				clientSock = serverSock;
				headerSize = sizeof(client);
				if ((timedOut = select(serverSock + 1, &socketsToRead, NULL, NULL, &timeout)) <= 0)
				{
					printf("No longer listening for new clients...please restart program.");
					errors = kGeneralError;
				}
			}
			if (errors == kNoError) //ensure there were no errors to this point
			{
				char recvBuf[100];
				memset(recvBuf, '\0', sizeof(recvBuf));

				if ((recvfrom(clientSock, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&client, &headerSize)) > 0)
				{
					if (strcmp(recvBuf, kClientCryOut) == 0)
					{
						if (sendto(clientSock, kServerAcceptance, sizeof(kServerAcceptance), 0, (struct sockaddr*)&client, sizeof(client)) > 0)
						{
							struct TestResults results;
							results.blockSize = blockSize;
							//let's go to packet sending method
							errors = benchmarkSend(blockSize, numBlocks, clientSock, client, &results);
							printResults(results, sockType, kServer);
						}
						else
						{
							errors = kSendBlockError;
						}
					}
					else
					{
						errors = kReceiveError;
					}
				}
				else
				{
					//couldn't send initial response, quit.
					errors = kTimeoutError;
					timedOut = 0;
				}
			}
			else
			{
				if (sockType == SOCK_STREAM)
				{
					printf("Error accepting connections via TCP...qutting...\n");
				}
				else
				{
					printf("No connections to UDP socket...quitting...\n");
				}
				
			}
		}
	}
	else
	{
		errors = kBindingError;
	}

	return errors;
}





/*
FUNCTION:		int benchmarkSend

DESCRIPTION:	This performs the actual testing of the block sending to the client

PARAMETERS:		int blockSize				-	The size of each block we will be sending
				int numBlocks				-	The total number of blocks to be sent
				SOCKET sock					-	The socket we will be sending each block on
				struct sockaddr_in client	-	The information we have about the client
				struct TestResults* test	-	A struct that will hold all the test results upon completion

RETURNS:		int errors - Indicates whether or not the test completed with no errors
*/



int benchmarkSend(int blockSize, int numBlocks, SOCKET sock, struct sockaddr_in client, struct TestResults* test)
{
	int errors = kNoError;
	double seconds = 0;
	int blocksSent = 0;
	int sendStatus = 0;
	int incompleteSends = 0;

	//each number in this array represent a multiple of 10
	//int[0] = value * 100000, int[1] = value * 10000 and so on...
	int packetsSent[6] = { 0,0,0,0,0,1 };

	//get block ready

	char* block = createTestBlock(blockSize);

	if (block != NULL)
	{

		//capture current time

		printf("________________________________________________\nTEST HAS BEGUN\nBlocks To Send: %d\n\nWill update once completed\n", numBlocks);
		time_t testStart = time(NULL);

		while (((blocksSent < numBlocks) && sendStatus >= 0)) // //FOR ACTUAL PROGRAM
		{
			//send blocks
			sendStatus = sendto(sock, block, blockSize, 0, (struct sockaddr*)&client, sizeof(client));
			if (sendStatus > 0)
			{
				setBlockHeader(block, packetsSent); //optimized for speed
				blocksSent++;
			}
			else if (sendStatus < 0)
			{
				incompleteSends++;
			}
		}
		seconds = difftime(time(NULL), testStart);
		printf("\n________________________________________________\n\nTEST COMPLETED ");



		if (sendStatus >= 0)
		{
			//if we didn't exit because of a send error, capture test metrues
			printf("WITHOUT ERRORS\n");
		}
		else
		{
			printf("WITH ERRORS\n\nREASON:\t");

			if (sendStatus == 0)
			{
				printf("SOCKET SHUTDOWN GRACEFULLY BY RECEIVER\n\n");
				errors = kNoError;
			}
			else
			{
				printf("THERE WAS AN ERROR SENDING ON THE SOCKET\n\n");
				errors = kSendBlockError;
			}
		}
		if (seconds == 0)
		{
			seconds++;
		}

		test->timeInSeconds = seconds;
		test->blocksRecv = blocksSent;
		test->blocksMissing = incompleteSends;

		free(block);
	}
	else
	{
		errors = kMallocError;
	}

	return errors;
}





/*
FUNCTION:		char* createTestBlock

DESCRIPTION:	This creates the block that will be sent to the user repeatedly.

PARAMETERS:		int blockSize	- The size of the block to be createds

RETURNS:		A pointer to the newly created block allocated or NULL if it did not succeed
*/
char* createTestBlock(int blockSize)
{
	char* block = NULL;

	block = (char*)malloc(blockSize);

	if (block != NULL)
	{
		memset(block, '0', blockSize);

		char* ptr = block;

		//place a bar at the header end
		ptr += kHeaderLength;
		*ptr = '|';
		ptr--;
		*ptr = '1';


	}

	return block;
}



/*
FUNCTION:		void setBlockHeader

DESCRIPTION:	This function places a header on the first 6 digits of each block which indicates the number of block
				it is

PARAMETERS:		char* block			-	A pointer to the block we will 'brand'
				int* packetsSent	-	The array holding the integers with the values for the branding

RETURNS:		None

*/


void setBlockHeader(char* block, int* packetsSent)
{
	char* editPoint = block;
	int i = kHeaderLength - 1; //acts as a counter
	int done = kFalse;

	//this fancy little while loop will increment each int IF IT IS MEANT TO BE INCREMENTED
	//each int represents a placeholder...this is faster than sprintf
	while (done == kFalse && i >= 0)
	{
		packetsSent[i]++;
		//if the value is greater than 10, we need to move to the next piece of the array
		if (packetsSent[i] >= 10)
		{
			packetsSent[i] = 0;
			i--;
		}
		else
		{
			done = kTrue;
		}
	}

	//convert each one to a char and make the header a bit better it to the header
	for (int i = 0; i < kHeaderLength; ++i)
	{
		//convert each int to a char for our custom header
		*editPoint = (char)(packetsSent[i] + '0');
		++editPoint;
	}
}