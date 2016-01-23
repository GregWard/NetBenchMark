/*
* FILE:			clientFunctions.c
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 16, 2015
*
* DESCRIPTION:	This program file contains the client/receiver specific functions. The client/receiver reaches out to the
				potential sender/receiver by either connecting to them over TCP connect/accept first and then sending a cryout
				handshake to ensure connection, or just sending the connection string before beginning the test.
*/


#include "../headers/allHeaders.h"
#include <time.h>

int benchmarkReceive(SOCKET sock, int flags, struct TestResults *testInfo);
void updateStatus(int* Rc, int* Rp, int* OO, int* Rm, int* Rh, int* Rl);





/*
FUNCTION:		int client

DESCRIPTION:	This function runs the receiving client for our test purposes

PARAMETERS:		int blockSize		- The size of the block we will be sending back and forth
				int testTime		- The length of time we will run our
				char* serverIP		- The stringified version of the sender we wish to contact
				int sockType		- The protocol that we are using for this test
				SOCKET clientSock	- The clients socket to send/receive on

RETURNS:		int errors - Indicates whether or not the function completed with errors or not
*/





int client(int blockSize, char* serverIP, int sockType, SOCKET clientSock)
{
	struct sockaddr_in server;
	int errors = kNoError;
	int flags = 0;

	server.sin_family = AF_INET;
	server.sin_port = htons(kDefaultPort);

	//FOR TCP SETUP
	if (sockType == SOCK_STREAM)
	{
		server.sin_addr.s_addr = inet_addr(serverIP);
		flags = MSG_WAITALL;
	}
	else// for UDP setup
	{
#ifdef _WIN32

		server.sin_addr.S_un.S_addr = inet_addr(serverIP);
#else
		if (inet_pton(AF_INET, serverIP, &server.sin_addr) != 1)
		{
			printf("Failure to convert IP! Exiting program...\n\n");
			errors = kIPConversionError;
		}
#endif
	}

	//connect the socket so it knows who to receive from
	if (connect(clientSock, (struct sockaddr*)&server, sizeof(server)) == 0)
	{
		printf("\nSuccessfully connected to server socket\n\n");
	}
	else
	{
		errors = kConnectionError;
	}

	//send the handshake asking to begin the test. If there is no response, the recv will timeout as per the sockOpt we
	//set at the start of the program
	if (errors == kNoError)
	{
		if (sendto(clientSock, kClientCryOut, sizeof(kClientCryOut), 0, (struct sockaddr*) &server, sizeof(server)) == -1)
		{
			errors = kSendBlockError;
		}
		else
		{
			//receive a response to the cryout from the server
			char recvBuf[10] = "";
			memset(recvBuf, '\0', 10);

			if (recvfrom(clientSock, recvBuf, sizeof(recvBuf), 0, NULL, NULL) == -1)
			{
				printf("Receiving Error...quitting now");
				errors = kReceiveError;
			}
			else
			{
				if (strcmp(kServerAcceptance, recvBuf) == 0)
				{
					//if we are in the clear, receive the request and continue as per normal
					struct TestResults testInfo;
					testInfo.blockSize = blockSize;

					errors = benchmarkReceive(clientSock, flags, &testInfo);
					printResults(testInfo, sockType, kClient);

				}
				else
				{
					errors = kReceiveError;
				}
			}
		}
	}


	return errors;
}





/*
FUNCTION:		int benchmarkReceive 

DESCRIPTION:	This function receives blocks continually until either an error occurs or the receive times out.
				After this is completed the results of the test completed are gathered into a struct and we return
				from the function.

PARAMETERS:		SOCKET sock						- The socket that will be received on.
				int flags						- Any flags for the rcv function. We are looking to WAIT_ALL for an entire block to be received
				struct TestResults* testInfo	- The struct that will hold the test results.

RETURNS:		int errors - Indicates whether or not the function completed with errors or not
*/
int benchmarkReceive(SOCKET sock, int flags, struct TestResults* testInfo)
{
	int errors = kNoError;

	int badReceives = 0;

	//used to track out of order/missing blocks
	int currentPacketNum = 0;
	int	previouslyRecvPackNum = 0;
	int	outOfOrderBlocks = 0;
	int blocksMissing = 0;
	int highestpacketNumRecv = 0;
	int lowestblocknumInSequence = 0;

	int blocksReceived = 0;
	int receiveStatus = 1;
	int complete = 1;

	char* recvBuf = (char*)malloc(testInfo->blockSize * sizeof(char));

	if (recvBuf != NULL)
	{
		printf("\n\nTESTING BEGIN\n______________________________________________________\n");
		printf("Status:Receiving Blocks\n");
		time_t testStart = time(NULL);

		while (receiveStatus > 0)
		{
			//ensure we receive the right blocksize
			//flags will be equal to WAIT_ALL on tcp socket to ensure no partial receives
			//DGRAM will always receive a full amount
			receiveStatus = recv(sock, recvBuf, testInfo->blockSize, flags);

			if (receiveStatus == testInfo->blockSize)
			{
				//get the current packet num
				sscanf(recvBuf, "%d|", &currentPacketNum);

				//calculate if it is out of order, missing or on track
				updateStatus(&currentPacketNum, &previouslyRecvPackNum,
					&outOfOrderBlocks, &blocksMissing, &highestpacketNumRecv, &lowestblocknumInSequence);

				blocksReceived++;
			}
			else if (receiveStatus < testInfo->blockSize && testInfo->blockSize > 0)
			{
				badReceives++;
			}

		}

		testInfo->timeInSeconds = difftime(time(NULL), testStart);
		testInfo->blocksMissing = blocksMissing;
		testInfo->blocksOutOfOrder = outOfOrderBlocks;
		testInfo->blocksRecv = blocksReceived;

		free(recvBuf);
	}
	else
	{
		//memory allocation error
	}

	if (receiveStatus < 0)
	{
		printf("UPDATE:Receiving Ended With Errors...\n\n");
		testInfo->timeInSeconds -= 1;
		errors = kReceiveError;
	}
	else
	{
		printf("UPDATE:Receiving Ended With No Errors...\n\n");
	}

	return errors;
}




/*
FUNCTION:			void updateStatus

DESCRIPTION:		This function determines whether a block is out of order or a missing block that is now coming in.
					It updates the numbers accordingly to ensure that precise tracking of these numbers is enabled.

PARAMETERS:			int Rc - currently received packet #;
					int Rp - previousl received packet #;
					int OO - out of order packet count;
					int Rm - missing packet count;
					int Rh - the highest packet # recv'd so far;
					int Rl - the last number in the completed sequence to this point

RETURNS:			None

*/
void updateStatus(int* Rc, int* Rp, int* OO, int* Rm, int* Rh, int* Rl)
{
	//this all assumes no two packets will share the same header, they will all be uniquely identifiable
	if (*Rc == ((*Rp) + 1)) //if the current received packet is in sequence to the one just received, this is happy path material
	{
		//the packet is in correct order
		if (*Rc == ((*Rh) + 1))
		{
			//happy path
			*Rh = *Rc;
			*Rl = *Rc;
		}
		else
		{
			//if the packet is not adjacent to the highest one but still in happy path, we are presumably already missing on
			//therefore, reduce missing packets by 1
			(*Rm)--;
		}
	}
	else if (*Rc > *Rh) //if currently received is greater than the highest received so far, this packet is out of order
	{
		(*OO)++; //increase out of order by 1
		*Rm += (*Rc - (*Rh + 1)); //increase packets missing by the amount between the just received and last highest
		*Rh = *Rc; //the current now represents the highest
	}
	else //we are somewehre inbetween the highest and the last full sequence
	{
		(*OO)++; //increase out of order by one since it wan't in order
		(*Rm)--; // reduce the # of missing packets by one since it is no longer missing, just out of order

		if (*Rc == ((*Rl) + 1)) //if this is next one in the sequence, then this is the new head of the sequence
		{
			*Rl = *Rc;
		}
	}

	*Rp = *Rc; //always do this, because we are done working wit this the previous packet is now the one we currently worked with
}


