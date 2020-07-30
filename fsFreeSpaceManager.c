/**************************************************************
* Class: CSC-415
* Name: Aaron Singh , Wameed, Tailor , Duy
* Student ID: NA 
* Project: Team Penta FS 
*
* File: fsFreeSpaceManager.c
*
* Description: 
* Resides in the 2nd block, presumably after the VCB, returns # to block to Write to 
**************************************************************/

#include "fsFreeSpaceManager.h"

bool initBitVector(uint64_t blockSize, uint64_t volumeSize)
{
    int freeMapIntCount = blockSize < sizeof(int) ? 1 : volumeSize / blockSize / 32;
    int bitVector[freeMapIntCount];
    for (int i = 0; i < freeMapIntCount; i++)
    {
        bitVector[i] = i;
    }
    char *char_g = (char *)bitVector;
    LBAwrite(char_g, freeMapIntCount / 512, 0);
    LBAread(char_g, 2, 0);
    return true;
}
void setBit(int bitVector[], int bitPosition)
{
    /* TODO
    1.Read From Disk[Location Block 2] <--------REMOVE bitVector[] argument
    2.Convert Into Array 
    3.Apply Below Formula 
    4.Re-Write to Disk

    ? Not sure how to keep track of the bitPositioning 
    
    */
    bitVector[bitPosition / 32] |= 1 << (bitPosition % 32);
}

void clearBit(int bitVector[], int bitPosition)
{

    /* TODO
    1.Read From Disk[Location Block 2]
    2.Convert Into Array 
    3.Apply Below Formula 
    4.Re-Write to Disk
    */
    bitVector[bitPosition / 32] &= ~(1 << (bitPosition % 32));
}

int findBit(int bitVector[], int bitPosition)
{

    /* TODO
    1.Read From Disk[Location Block 2]
    2.Convert Into Array 
    3.Apply Below Formula 
    4.Re-Write to Disk
    */
    return ((bitVector[bitPosition / 32] & (1 << (bitPosition % 32))) != 0);
}
