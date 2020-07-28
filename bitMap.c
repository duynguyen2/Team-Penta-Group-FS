/**************************************************************
* Class: CSC-415
* Name: 
* Student ID: 
* Project: 
*
* File: bitMap.c
*
* Description: 
*
**************************************************************/

#include <stdio.h>
#include "bitMap.h"

void setBit(int A[], int k)
{
    A[k / 32] |= 1 << (k % 32);
}

void clearBit(int A[], int k)
{
    A[k / 32] &= ~(1 << (k % 32));
}

int findBit(int A[], int k)
{
    return ((A[k / 32] & (1 << (k % 32))) != 0);
}
