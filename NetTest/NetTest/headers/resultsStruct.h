/*
* FILE:			resultsStruct.h
*
* PROJECT:		IAD - Assignment 1
*
* PROGRAMMER:	Greg Ward
*
* DATE:			January 16, 2015
*
* DESCRIPTION:	This file contains the definition for the TestResults struct. This struct is used to store the
				results of the tests upon completion and is passed to the printResults function to be interpreted
*/


#ifndef RESULT_STRUCT
#define RESULT_STRUCT



#include "allHeaders.h"


struct TestResults {
	int blockSize;
	int blocksRecv;
	int blocksMissing;
	int blocksOutOfOrder;
	double timeInSeconds;
};

#endif