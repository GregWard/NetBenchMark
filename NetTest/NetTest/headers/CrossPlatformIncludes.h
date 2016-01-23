/*
* FILE:			CrossPlatformIncludes.h
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 7, 2015
*
* DESCRIPTION:	This file contains the platform specific compilation library includes and prototypes for the cross platform
				functions used in creating and closing the sockets.
*
*/


#ifndef CROSSPLATINCLUDE
#define CROSSPLATINCLUDE
//Only include the header file once

//common header files
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifdef _WIN32
/* See http://stackoverflow.com/questions/12765743/getaddrinfo-on-win32 */
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  /* Windows XP. */
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#pragma warning(disable:4996)
#include <winsock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#define MSG_WAITALL 0x8
#pragma comment(lib, "Ws2_32.lib")
#else
/* Assume that any non-Windows platform uses POSIX-style sockets instead. */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  /* Needed for getaddrinfo() and freeaddrinfo() */
#include <unistd.h> /* Needed for close() */
#include <sys/time.h>

typedef int SOCKET;
#define INVALID_SOCKET -1

#endif



//PROTO TYPES
int sockInit(void);
int sockQuit(void);
int sockClose(SOCKET sock);



#endif
