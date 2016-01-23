/*
* FILE:			errorDefinitions.h
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 9, 2015
*
* DESCRIPTION:	This file contains the error code definitions used to elimination magic numbers from the program.
*
*/



#ifndef ERROR_DEFINITIONS
#define ERROR_DEFINITIONS

#define kNoError 0
#define kCmdLineArgInvalid 1
#define kConnectionError 2
#define kTimeoutError 9
#define kGeneralError 10

#define kTrue 1
#define kFalse 0

#define kBindingError 3
#define kAcceptError 4
#define kReceiveError 5
#define kListeningError 6
#define kIPConversionError 7
#define kMallocError 11

#define kSendBlockError 8;



#endif
