#include <stdio.h>
#include <limits.h>    /* for CHAR_BIT */
#include <stdint.h>   /* for uint32_t */

#ifndef FREESPACEBITMAP_H
#define FREESPACEBITMAP_H

//https://stackoverflow.com/questions/40701950/free-space-bitmap-c-implementation

int memBlockSize;
int memBlockCount;
uint *bitMap;
char *buffer;

void initMemMap(int blockSize, int blockCount);
int isAllocated(int index);
void allocateFrame(int index);
void clearFrame(int index);
char* allocateBlock(int blockCount);

#endif
