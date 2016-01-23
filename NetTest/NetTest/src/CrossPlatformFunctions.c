/*
* FILE:			CrossPlatformFunctions.C
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Gregory Ward
*
* DATE:			January 7, 2015
*
* DESCRIPTION:	This source file contains the definitions for the Cross Platorm Utilities that are used for
*				performing the network benchmark tests.
*
*/



#include "../headers/allHeaders.h"





/*
FUNCTION:		sockInit

DESCRIPTION:	Depending on the platform it is compiled for, it will initialize the WSADATA struct for socket use on
				Windows. If it is not a windows machine, then 0 is returned.

PARAMETERS:		void

RETURNS:		Windows: A pointer to a WSADATA structure intialized by the WSAStartup function
				Other: 0

CODE ACKNOWLEDGMENT: Stack Overflow - GoBusto - http://stackoverflow.com/questions/28027937/cross-platform-sockets
*/




int sockInit(void)
{
#ifdef _WIN32
	WSADATA wsa_data;
	return WSAStartup(MAKEWORD(1, 1), &wsa_data);
#else
	return 0;
#endif
}





/*
FUNCTION:		sockQuit

DESCRIPTION:	Depending on the platform it is compiled for, it will terminate use of the winsock dll

PARAMETERS:		void

RETURNS:		No return. On Windows compilation the WSACleanup function is run.

CODE ACKNOWLEDGMENT: Stack Overflow - GoBusto - http://stackoverflow.com/questions/28027937/cross-platform-sockets
*/




int sockQuit(void)
{
#ifdef _WIN32
	return WSACleanup();
#else
	return 0;
#endif
}




/*
FUNCTION:		sockClose

DESCRIPTION:	An existing socket is closed and shutdown.

PARAMETERS:		SOCKET sock ( SOCKET is defined as an int on non windows machines)

RETURNS:		int status - The status of the shutdown and close of the socket.

CODE ACKNOWLEDGMENT: Stack Overflow - GoBusto - http://stackoverflow.com/questions/28027937/cross-platform-sockets
*/




int sockClose(SOCKET sock)
{

	int status = 0;

#ifdef _WIN32
	status = shutdown(sock, SD_BOTH);

	if (status == 0) 
	{ 
		status = closesocket(sock); 
	}
#else
	status = shutdown(sock, SHUT_RDWR);

	if (status == 0) 
	{ 
		status = close(sock); 
	}
#endif

	return status;

}