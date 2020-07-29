/**************************************************************
* Class:  CSC-415
* Name: Taylor Artunian
* Team: Team Penta
* Student ID: N/A
* Project: Basic File System
*
* File: fsMakeVol.c
*
* Description: This is a set of routines used to create a volume
*              for the Penta File System.
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

#define BLOCKS_PER_INODE	4	//Number of disk blocks per one inode.
#define VCB_START_BLOCK		0	//First block of VCB
#define INODE_START_BLOCK	1	//First block of inodes

typedef struct {
  char header[16];
  uint32_t freeBlockMap[4];
  uint32_t diskSizeBlocks;
  struct mfs_dirent rootDir;
} mfs_VCB;

/* Creates and writes a VCB to VCB_START_BLOCK using minimum
 * number of blocks.
 */
void initializeVCB(uint64_t, uint64_t);

/* Creates and writes all inodes for the system to
 * INODE_START_BLOCK.
 */
void initializeInodes(uint64_t, uint64_t);

/* Utility function for printing the VCB in hex and ASCII. */
void printVCB(mfs_VCB*);

/* Creates a new volume with the specified fileName, volumeSize and blockSize.
 * Initializes VCB and inodes.
 * Return Values:  0 Volume created successfully.
 *                -1 File exists but cannot be written to.
 *                -2 Insufficient space to create volume.
 *                -3 Volume already exists.
 */
int createVolume(char*, uint64_t, uint64_t);


