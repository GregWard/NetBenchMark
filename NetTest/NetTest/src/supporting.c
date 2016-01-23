/*
* FILE:			main.c
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 16, 2015
*
* DESCRIPTION:	This file contains the supporting functions for our program such as the useage statement that will print out
				if the cmd line args aren't what we expected, the parsing of the cmd line args and the printing of the test
				results.
*/


#include "../headers/allHeaders.h"





/*
FUNCTION:		void printUseage
DESCRIPTION:	This function prints the useage statement command line arguments for the test time, the mode of transport
PARAMETERS:		none
RETURNS:		none
*/
void printUseage(void)
{
	printf("Usage: NetTest.exe [-c IPv4Addr] (indicates client session and IPv4 of server to connect)\n"
		"\t[-b Size] (where Size is 1000/2000/5000/10,000)\n"
		"\t[-u] (indicates use of udp, tcp is default)\n"
		"\t[-l numBlocks] (where numBlocks indicates the amount of blocks to send (must be a whole number\n and less than 1 million)\n");
}



/*
FUNCTION:		int parseArgs

DESCRIPTION:	This function parses command line arguments for any differences from the defaults.

PARAMETERS:		int* blockSize - the size of each block to be sent (a pointer to this variable)
int* sockType - if the protocol will be UDP, sockType is set to Dgram (a pointer to this variable)
int* role - If the program will be acting as a server or client (a pointer to this variable)
int* testlength - the number of blocks to send (a pointer to this variable)
char* serverIP - the address of the server to connect to
int argc - the numbero f command line args
char* argv[] - the command line arguments

RETURNS:		int errors - indicates whether or not there was an error in parsing the command line arguments
*/




int parseArgs(int* blockSize, int* sockType, int* role, int* testlength, char* serverIP, int argc, char* argv[])
{
	int errors = kNoError;
	int argOk = kFalse;

	int blockSizeSet = kFalse;
	int sockSet = kFalse;
	int roleSet = kFalse;
	int timeSet = kFalse;


	for (int i = 1; i < argc; ++i)
	{
		argOk = kFalse;

		//test to see if this is the client command
		if (strcmp(argv[i], "-c") == 0)
		{
			argOk = kTrue;

			if (roleSet == kFalse)
			{
				if (i + 1 < argc)
				{
					if (strlen(argv[i + 1]) <= 15)
					{
						strcpy(serverIP, argv[i + 1]);
						*role = kClient;
						roleSet = kTrue;
						i++;
					}
				}
				else
				{
					errors = kCmdLineArgInvalid;
					break;
				}
			}
			else
			{
				errors = kCmdLineArgInvalid;
				break;
			}
		}

		//test to see if this is the block size command
		if (strcmp(argv[i], "-b") == 0)
		{
			argOk = kTrue;

			if (blockSizeSet == kFalse && i + 1 < argc)
			{
				for (int j = strlen(argv[i + 1]) - 1; j > 0; --j)
				{
					if (argv[i + 1][j - 1] >= '0' && argv[i + 1][j - 1] <= '9')
					{
						continue;
					}
					else
					{
						errors = kCmdLineArgInvalid;
						break;
					}
				}

				if (errors == kNoError)
				{
					sscanf(argv[i + 1], "%d", blockSize);
					if (*blockSize == 1000 || *blockSize == 2000 || *blockSize == 5000 || *blockSize == 10000)
					{
						i++;
					}
					else
					{
						errors = kCmdLineArgInvalid;
						break;
					}
				}
				else
				{
					break;
				}

			}
			else
			{
				errors = kCmdLineArgInvalid;
				break;
			}
		}

		//test to see if this is the socket type command
		if (strcmp(argv[i], "-u") == 0)
		{
			argOk = kTrue;

			if (sockSet == kFalse)
			{
				*sockType = SOCK_DGRAM;
			}
			else
			{
				errors = kCmdLineArgInvalid;
				break;
			}
		}

		//test to see if this is num blocks to send command
		if (strcmp(argv[i], "-l") == 0)
		{
			argOk = kTrue;

			if (timeSet == kFalse && i + 1 < argc)
			{
				for (int j = strlen(argv[i + 1]) - 1; j > 0; --j)
				{
					if (argv[i + 1][j - 1] >= '0' && argv[i + 1][j - 1] <= '9')
					{
						continue;
					}
					else
					{
						errors = kCmdLineArgInvalid;
						break;
					}
				}

				if (errors == kNoError)
				{
					sscanf(argv[i + 1], "%d", testlength);
					if (*testlength >= 1 && *testlength <1000000)
					{
						++i;
					}
					else
					{
						errors = kCmdLineArgInvalid;
						break;
					}

				}
				else
				{
					break;
				}

			}
			else
			{
				errors = kCmdLineArgInvalid;
				break;
			}
		}

		if (argOk == kFalse)
		{
			errors = kCmdLineArgInvalid;
			break;
		}
	}


	return errors;
}


/*
FUNCTION:		void printResults

DESCRIPTION:	This functions will print the results of the test just completed.

PARAMETERS:		struct TestResults results	- The struct that holds the results of the recently completed test
int sockType				- The protocol we are using (TCP or UDP)

RETURNS:		none

*/

void printResults(struct TestResults results, int sockType, int role)
{

	int totalPacketsSentPerBlock = 0;
	int protocolsize = kIPSize;
	int additionalBytesPerBlock = 0;


	if (sockType == SOCK_DGRAM)
	{
		protocolsize += kUDPSize;
	}
	else
	{
		protocolsize += kTCPSize;
	}

	totalPacketsSentPerBlock = (results.blockSize / (kMTU - protocolsize));

	//account for rounding
	if ((results.blockSize % (kMTU - protocolsize)) != 0)
	{
		totalPacketsSentPerBlock++;
	}

	additionalBytesPerBlock =(totalPacketsSentPerBlock * (kEthernetHeaderFCS + kFramePreamble + kInterFrameGap + protocolsize));


	printf("____________________________________\nTest Results Are:\n");
	printf("Protocol:\t\t\t");

	if (sockType == SOCK_STREAM)
	{
		printf("TCP/IPv4");
	}
	else
	{
		printf("UDP/IPv4");
	}
	printf(" over Ethernet\n");

	printf("Size Per Block: \t\t%d\n", results.blockSize);
	printf("Time to Complete: \t\t%lf seconds\n", results.timeInSeconds);

	if (role == kClient)
	{
		printf("Blocks Received Total: \t\t%d\n", results.blocksRecv);
		printf("Blocks Missing Total: \t\t%d\n", results.blocksMissing);
		printf("Blocks Received Out Of Order:\t%d\n", results.blocksOutOfOrder);
	}
	else
	{
		printf("Blocks Sent Total: \t\t%d\n", results.blocksRecv);
		printf("Incomplete Blocks Sent Total: \t\t%d\n\n", results.blocksMissing);

		printf("Protocol Header Size(inc. IPv4): \t\t%d\n", protocolsize);
		printf("Num. Frames To Send Block: \t\t%d\n", totalPacketsSentPerBlock);
		printf("Additional bytes per block:\t\t%d\n\n", additionalBytesPerBlock);
	}
}