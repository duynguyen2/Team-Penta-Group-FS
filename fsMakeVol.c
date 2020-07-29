/**************************************************************
* Class:  CSC-415
* Name: Taylor Artunian
* Team: Team Penta
* Student ID: N/A
* Project: Basic File System
*
* File: fsMakeVol.c
*
* Description: This program is used to create a volume for the
*              Penta File System.
*
**************************************************************/

#include "fsMakeVol.h"

int ceilDiv(int a, int b) {
  /* Rounds up integer division. */
  return (a + b - 1) / b;
}

void initializeVCB(uint64_t volumeSize, uint64_t blockSize) {
  printf("------------------------Initializing VCB-------------------------\n");

  /* Compute size of disk in blocks. */
  int diskSizeBlocks = ceilDiv(volumeSize, blockSize);
  
  /* Compute the number of blocks needed to hold the VCB. */
  int totalVCBBlocks = ceilDiv(sizeof(mfs_VCB), blockSize);

  mfs_VCB* vcb_p = calloc(totalVCBBlocks, blockSize);
  sprintf(vcb_p->header, "*****PentaFS****");
  for(int i=0; i<4; i++) {
    vcb_p->freeBlockMap[i] = 4294967295;	//uint32_t with all 32 bits hi
  }

  /* Initialize properties of the root directory entry. */
  vcb_p->diskSizeBlocks = diskSizeBlocks;
  vcb_p->rootDir.d_ino = 1;
  vcb_p->rootDir.d_off = 1;
  vcb_p->rootDir.d_reclen = 16;
  vcb_p->rootDir.d_type = '1';
  sprintf(vcb_p->rootDir.d_name, "root");
  printVCB(vcb_p);

  /* Write the VCB to disk. */
  char* char_p = (char*) vcb_p;
  LBAwrite(char_p, totalVCBBlocks, VCB_START_BLOCK);
  printf("Wrote VCB of size %ld in %d blocks starting at block %d.\n", sizeof(mfs_VCB), totalVCBBlocks, VCB_START_BLOCK);
  free(vcb_p);
}

void initializeInodes(uint64_t volumeSize, uint64_t blockSize) {
  printf("-----------------------Initializing inodes-----------------------\n");

  /* Compute size of disk in blocks. */
  int diskSizeBlocks = ceilDiv(volumeSize, blockSize);

  /* Calculate number of inodes for entire system based on BLOCKS_PER_INODE. */
  int totalInodes = ceilDiv(diskSizeBlocks,BLOCKS_PER_INODE);

  /* Calculate number of blocks required to hold all inodes. */
  int totalInodeBlocks = ceilDiv(totalInodes*sizeof(mfs_DIR), blockSize);
  printf("Total disk blocks: %d, total inodes: %d, total inode blocks: %d\n", diskSizeBlocks, totalInodes, totalInodeBlocks);

  /* Allocate and initialize inodes. First inode is root directory and has id=1. */
  mfs_DIR* inodes = calloc(totalInodeBlocks, blockSize);
  inodes[0].id = 1;
  inodes[0].type = 1;
  inodes[0].parent = 0;
  sprintf(inodes[0].name, "root_inode");
  inodes[0].numDirectBlockPointers = 0;
  for(int i = 1; i<totalInodes; i++) {
    inodes[i].id = 0;
    inodes[i].type = 0;
    inodes[i].parent = 0;
    sprintf(inodes[i].name, "unused_inode");
    inodes[i].numDirectBlockPointers = 0;
  }

  /* Write inodes to disk. */
  char* char_p = (char*) inodes;
  LBAwrite(char_p, totalInodeBlocks, INODE_START_BLOCK);
  printf("Wrote %d inodes of size %ld bytes each starting at block %d.\n", totalInodes, sizeof(mfs_DIR), INODE_START_BLOCK);
  free(inodes);
}

void printVCB(mfs_VCB* vcb_p) {
  int size = sizeof(*vcb_p);
  int width = 16;
  char* char_p = (char*)vcb_p;
  char ascii[width+1];
  sprintf(ascii, "%s", "................");
  printf("--------------------------Printing VCB---------------------------\n");
  for(int i = 0; i<size; i++) {
    printf("%02x ", char_p[i] & 0xff);
    if(char_p[i]) {
      ascii[i%width] = char_p[i];
    }
    if((i+1)%width==0&&i>0) {
      ascii[i%width+1] = '\0';
      printf("%s\n", ascii);
      sprintf(ascii, "%s", "................");
    } else if (i==size-1) {
      for(int j=0; j<width-(i%(width-1)); j++) {
        printf("   ");
      }
      ascii[i%width+1] = '\0';
      printf("%s\n", ascii);
      sprintf(ascii, "%s", "................");
    }
  }
  printf("VCB Size: %d bytes\n", size);
  printf("Size of mfs_dirent: %ld bytes\n", sizeof(struct mfs_dirent));
}

int createVolume(char* fileName, uint64_t volumeSize, uint64_t blockSize) {

  /* Check whether volume exists already. */
  if(access(fileName, F_OK) != -1) {
    printf("Cannot create volume '%s'. Volume already exists.\n", fileName);
    return -3;
  }

  uint64_t existingVolumeSize = volumeSize;
  uint64_t existingBlockSize = blockSize;

  /* Initialize the volume with the fsLow library. */
  int retVal = startPartitionSystem (fileName, &existingVolumeSize, &existingBlockSize);

  printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d\n", fileName, (ull_t)volumeSize, (ull_t)blockSize, retVal);

  switch(retVal) {

    /* startPartitionSystem had no errors creating the volume.
     * Now compute the number of uint32_t required to serve as
     * the freeBlockMap and store in the mfs_VCB struct. Then
     * write the mfs_VCB struct to block 0 of the disk.
     */
    case 0: {
      initializeVCB(volumeSize, blockSize);
      break;
    }

    /* startPartitionSystem could not write to the volume. */
    case -1: {
      printf("File '%s' exists but cannot open for write.\n", fileName);
      return -1;
    }

    /* startPartitionSystem did not have enough space to
     * create the volume.
     */
    case -2: {
      printf("Insufficient space for the volume '%s'.\n", fileName);
      return -2;
    }
  }

  initializeInodes(volumeSize, blockSize);

  closePartitionSystem();
  return 0;

}




