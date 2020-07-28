/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: fsLowDriver.c
*
* Description: This is a demo to show how to use the fsLow
* 	routines.
*
**************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include "fsLow.h"
#include "mfs.h"

struct mfs_VCB {
  char header[16];
  uint32_t DISK_SIZE_BYTES;
  uint32_t BLOCK_SIZE_BYTES;
  uint32_t DISK_SIZE_BLOCKS;
  uint32_t freeBlockMap[];
};

int main (int argc, char *argv[]) {
  char * filename;
  uint64_t volumeSize;
  uint64_t blockSize;
  int retVal;

  if (argc > 3)
  {
    filename = argv[1];
    volumeSize = atoll (argv[2]);
    blockSize = atoll (argv[3]);
  } else {
    printf ("Usage: fsMakeVol volumeFileName volumeSize blockSize\n");
    return -1;
  }

  retVal = startPartitionSystem (filename, &volumeSize, &blockSize);	
  printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d\n", filename, (ull_t)volumeSize, (ull_t)blockSize, retVal);

  int diskSizeBlocks = volumeSize/blockSize;
  int freeMapIntCount = diskSizeBlocks<sizeof(int)?sizeof(int):ceil(diskSizeBlocks/sizeof(int));
  int vcbSize = sizeof(struct mfs_VCB)+sizeof(int[freeMapIntCount]);
  struct mfs_VCB* vcb = malloc(vcbSize);
  printf("freeMapIntCount=%d\n", freeMapIntCount);
  printf("vcbSize=%d, sizeof(*vcb)=%ld\n", vcbSize, sizeof(*vcb));
  sprintf(vcb->header, "PentaFS VCB");
  vcb->DISK_SIZE_BYTES = volumeSize;
  vcb->BLOCK_SIZE_BYTES = blockSize;
  vcb->DISK_SIZE_BLOCKS = diskSizeBlocks;
  vcb->freeBlockMap[0] = 1111;
  vcb->freeBlockMap[1] = 2222;
  vcb->freeBlockMap[2] = 3333;

  printf("[VCB|DISK_SIZE_BYTES=%d BLOCK_SIZE_BYTES=%d DISK_SIZE_BLOCKS=%d]\n", vcb->DISK_SIZE_BYTES, vcb->BLOCK_SIZE_BYTES, vcb->DISK_SIZE_BLOCKS);
  for(int i=0; i<freeMapIntCount; i++) {
    printf("\t freeBlockMap[%d]: %d\n", i, vcb->freeBlockMap[i]);
  }
  printf("[VCB|Size=%ld bytes]\n", sizeof(*vcb));

  char* vcbBuf = calloc(blockSize, sizeof(char));
  memcpy(vcbBuf, vcb, vcbSize);
//  sprintf(vcbBuf, "ABCDEFGHIJKLMNOP");

  uint64_t w = LBAwrite(vcbBuf, 1, 0);
  printf("[VCB|VCB written in %ld blocks]\n", w);

  char* vcbBuf2 = calloc(blockSize, sizeof(char));
  uint64_t r = LBAread(vcbBuf2, 1, 0);
  printf("[VCB|VCB read back in %ld blocks]\n", r);

  int c = memcmp(vcbBuf, vcbBuf2, blockSize);
  printf("[VCB|VCB comparison = %d, %s]\n", c, c==0?"Success":"Error");

  if(!c) {
    struct mfs_VCB* vcb2 = (struct mfs_VCB*)vcbBuf2;
    printf("[VCB2|DISK_SIZE_BYTES=%d BLOCK_SIZE_BYTES=%d DISK_SIZE_BLOCKS=%d]\n", vcb2->DISK_SIZE_BYTES, vcb2->BLOCK_SIZE_BYTES, vcb2->DISK_SIZE_BLOCKS);
    for(int i=0; i<freeMapIntCount; i++) {
      printf("\t freeBlockMap[%d]: %d\n", i, vcb2->freeBlockMap[i]);
    }
    printf("[VCB2|Size=%ld bytes]\n", sizeof(*vcb2));
    free(vcb2);
  }

  free(vcbBuf);
  free(vcbBuf2);
  free(vcb);

  
  char * buf = malloc(blockSize *2);
  char * buf2 = malloc(blockSize *2);
  memset (buf, 0, blockSize*2);
  strcpy (buf, "Now is the time for all good people to come to the aid of their countrymen\n");
  strcpy (&buf[blockSize+10], "Four score and seven years ago our fathers brought forth onto this continent a new nation\n");
  LBAwrite (buf, 2, 0);
  LBAwrite (buf, 2, 3);
  LBAread (buf2, 2, 0);
  if (memcmp(buf, buf2, blockSize*2)==0)
  {
    printf("Read/Write worked\n");
  } else {
    printf("FAILURE on Write/Read\n");
  }

  free (buf);
  free(buf2);
  

  closePartitionSystem();
  return 0;
}

