#include "bitMap.h"

//https://stackoverflow.com/questions/40701950/free-space-bitmap-c-implementation

void initMemMap(int blockSize, int blockCount) //initialize the bitmap, blocks and buffer
{
    memBlockSize = blockSize;
    memBlockCount = blockCount;
    buffer = (char*)malloc(blockSize * blockCount);
    bitMap = (uint*)calloc((blockCount / 32) + ((blockCount % 32) != 0), 4);
}

int isAllocated(int index) //return if it the block is occupied(=1)
{
    return (bitMap[index / 32] && (1 << (index % 32))) != 0;
}

void allocateFrame(int index)
{
    bitMap[index / 32] |= 1 << (index % 32);
}

void clearFrame(int index)
{
    bitMap[index / 32] &= ~(1 << (index % 32));
}

char* allocateBlock(int blockCount)
{
    int index = 0, freeFrames = 0;
    while(index < memBlockCount)
    {
        if (!isAllocated(index))
        {
            freeFrames++;
            if (freeFrames == blockCount)
            {
                int frameIndex = index - blockCount + 1;

                index = 0;
                while(index < blockCount)
                {
                    allocateBlock(frameIndex + index);
                    index++;
                }
                return (buffer + frameIndex * memBlockSize);
            }
        }
        else
		freeFrames = 0;
        index++;
    }

    perror("Memory error!\n");
    return 0;
}
