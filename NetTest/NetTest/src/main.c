/*
* FILE:			main.c
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 16, 2015
*
* DESCRIPTION:	This program acts to benchmark TCP/IP or UDP/IP sending between two machines. A blocksize is inserted and
				a number of blocks to send is inserted to a server. The server waits 5 seconds for a connection before timing
				out. A client will connect on the appropriate protocol/port and then the sending begins.
*/


#include "../headers/allHeaders.h"

void printResults(struct TestResults results, int sockType, int role);
int netTest(int argc, char*argv[]);
int parseArgs(int* blockSize, int* sockType, int* role, int* testTime, char* serverIP, int argc, char* argv[]);
void printUseage(void);
int server(int blockSize, int testTime, int sockType, SOCKET socket);
int client(int blockSize, char* serverIP, int sockType, SOCKET clientSock);





int main(int argc, char* argv[])
{
	return netTest(argc, argv);
}





/*
FUNCTION:			int netTest

DESCRIPTION:		This is the main function of our testing benchmark and contains the protoc

PARAMETERS:			int argc - The count of command line arguments
					char*argv[] - The command line arguments passed when program is run

RETURNS:			int errors - indicates how the function completed, if there was any problems in running the program or not.
*/


int netTest(int argc, char*argv[])
{
	int blockSize = kDefaultBlockSize;
	int sockType = kDefaultSockType;
	int role = kDefaultRole;
	int numBlocks = kDefaultTestBlocksToSend;
	char serverIP[kIPV4Length] = "";
	int errors = kNoError;

#ifdef _WIN32
	//windows uses DWORD to determine length of time for sockopt timeout
	int timeout = 1000;
#else

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 500000; //equivalent to 500 miliseconds
#endif


							  //struct fd_set set;

	if (argc > 1)
	{
		errors = parseArgs(&blockSize, &sockType, &role, &numBlocks, serverIP, argc, argv);
	}

	if (errors == kNoError)
	{
		//call SockInit to initiate use of winsock dll if in win32
		if (sockInit() == 0)
		{
			//because we determined the sockType already (STREAM or DATAGRAM) the protocol will either be TCP or UDP
			SOCKET sock = socket(AF_INET, sockType, 0);

			if (sock == INVALID_SOCKET) //windows specific, will have to change
			{
				printf("Cannot Initialize Socket...quitting program...");
				sockQuit();
			}
			else
			{
#ifdef _WIN32
				//windows uses const char where as linux uses const void
				if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) == 0)
#else

				if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const void *)&timeout, sizeof(timeout)) == 0)
#endif
				{

					if (role == kServer)
					{
						//go into server mode
						errors = server(blockSize, numBlocks, sockType, sock);
					}
					else
					{
						//go into client mode
						errors = client(blockSize, serverIP, sockType, sock);
					}
					printf("\n_______________________________________________________\nTEST COMPLETED\n");

					//gracefully get rid of our sockets
					sockClose(sock);
					sockQuit();
				}
				else
				{
					printf("Could not set the sockOpt to timeout on a receive. Socket is unreliable...quitting...\n\n");
				}
			}

		}
		else
		{
			//error will only occur on windows machines
#ifdef _WIN32
			printf("ERROR: Failed to initialize WinSock...\nError Code:%d", WSAGetLastError());
#endif 
		}
	}
	else
	{
		printUseage();
	}

	return errors;
}