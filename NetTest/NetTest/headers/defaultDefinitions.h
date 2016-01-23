/*
* FILE:			defaultDefinitions.h
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 9, 2015
*
* DESCRIPTION:	This file contains the default test information definitions used to elimination magic 
				numbers from the program as well as any other magic numbers that may appear to easily
				be edited in one spot.
*
*/


#ifndef DEFAULT_DEFINITIONS
#define DEFAULT_DEFINITIONS

#define kServer 1
#define kClient 2

#define kClientCryOut "father?"
#define kServerAcceptance "son!"

#define kDefaultBlockSize 1000
#define kDefaultSockType SOCK_STREAM
#define kDefaultRole kServer
#define kDefaultTestBlocksToSend 100000

#define kDefaultPort 8040
#define kIPV4Length 16 //include a spot for the null terminator

#define kHeaderLength 6


#define kMTU 1500 //lab computers use standard frame size, not jumbo frames
#define kTCPSize 20
#define kUDPSize 8
#define kIPSize 20
#define kEthernetHeaderFCS 18
#define kInterFrameGap 12
#define kFramePreamble 8
#define kBytesToMegabytes 1000000


#endif