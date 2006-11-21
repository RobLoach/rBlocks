/*******************************************************************************
*
*   Title:		Rand.h
*   Author:	    Rob Loach (http://robloach.net)
*   Date:		November 18th, 2004
*
*   Description:
*				A random number generator.
*
*******************************************************************************/


/*******************************************************************************
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*******************************************************************************/

 
//******************************************************************************
// Definition

#ifndef _RAND_H_INCLUDED_
#define _RAND_H_INCLUDED_

 
//******************************************************************************
// Includes

#include <ctime>
#include <stdlib.h>

 
//******************************************************************************
// Functions

// InitRandomNumber - Initiate the random number generator
void InitRandomNumber(){
	srand( (unsigned)time( NULL ) );
}

// RandomNumber(low, high) - Returns an integer between the low and high number
int RandomNumber(int lowest, int highest){
  return (rand() % (highest - lowest + 1)) + lowest;
}

// RandomNumber(high) - Returns an integer between 0 and the high number
int RandomNumber(int highest){
  return (rand() % (highest + 1));
}

// RandomNumber returns a random number
int RandomNumber(){
  return rand();
}


#endif // _RAND_H_INCLUDED_
