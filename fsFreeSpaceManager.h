/**************************************************************
* Class: CSC-415
* Name: Aaron Singh , Wameed, Tailor , Duy
* Student ID: NA 
* Project: Team Penta FS 
*
* File: fsFreeSpaceManager.h
*
* Description: 
* Resides in the 2nd block, presumably after the VCB, keeps track of allocated space, returns # to block to Write to 
**************************************************************/

#ifndef FSFREESPACEMANAGER_H
#define FSFREESPACEMANAGER_H

#include <stdint.h>  //uint64
#include <stdbool.h> //Bool
bool initBitVector(uint64_t blockSize, uint64_t volumeSize);

void setBit(int bitVector[], int bitPosition);
void clearBit(int bitVector[], int bitPosition);
int findBit(int bitVector[], int bitPosition);

#endif