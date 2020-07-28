#include <stdio.h>
#include <unistd.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mfs.h"
#include <dirent.h> //DIR

#define MAXFCBS 20
#define BUFSIZE 512

typedef struct b_fcb
{
    int linuxFd;    //holds the systems file descriptor
    char *buf;      //holds the open file buffer
    char *writeBuf; //holds the open file buffer
    int index;      //holds the current position in the buffer
    int buflen;     //holds how many valid bytes are in the buffer
} b_fcb;

b_fcb fcbArray[MAXFCBS];
int startup = 0;
//Stack ALlocated Array and Counter
char write_Buffer[BUFSIZE];
int counter_Write = 0;

int b_open(char *filename, int flags)
{
    int fd;
    int returnFd;

    fcbArray[returnFd].writeBuf = malloc(BUFSIZE);
    if (startup == 0)
        b_init(); //Initialize our system

    // lets try to open the file before I do too much other work

    //Can read&Write permissions
    fd = open(filename, flags, 0600);

    if (fd == -1)
        return (-1); //error opening filename

    //Should have a mutex here
    returnFd = b_getFCB();           // get our own file descriptor
                                     // check for error - all used FCB's
    fcbArray[returnFd].linuxFd = fd; // Save the linux file descriptor
    //	release mutex

    //allocate our buffer
    fcbArray[returnFd].buf = malloc(BUFSIZE);

    if (fcbArray[returnFd].buf == NULL)
    {
        // very bad, we can not allocate our buffer
        close(fd);                       // close linux file
        fcbArray[returnFd].linuxFd = -1; //Free FCB
        return -1;
    }
	
    fcbArray[returnFd].writeBuf = malloc(BUFSIZE);

    if (fcbArray[returnFd].writeBuf == NULL)
    {
        // very bad, we can not allocate our buffer
        close(fd);                       // close linux file
        fcbArray[returnFd].linuxFd = -1; //Free FCB
        return -1;
    }

    fcbArray[returnFd].buflen = 0; // have not read anything yet
    fcbArray[returnFd].index = 0;  // have not read anything yet
    return (returnFd);             // all set
}

int b_open(char *filename, int flags)
{
    int fd;
    int returnFd;

    fcbArray[returnFd].writeBuf = malloc(BUFSIZE);
    if (startup == 0)
        b_init(); //Initialize our system

    // lets try to open the file before I do too much other work

    //Can read&Write permissions
    fd = open(filename, flags, 0600);

    if (fd == -1)
        return (-1); //error opening filename

    //Should have a mutex here
    returnFd = b_getFCB();           // get our own file descriptor
                                     // check for error - all used FCB's
    fcbArray[returnFd].linuxFd = fd; // Save the linux file descriptor
    //	release mutex

    //allocate our buffer
    fcbArray[returnFd].buf = malloc(BUFSIZE);

    if (fcbArray[returnFd].buf == NULL)
    {
        // very bad, we can not allocate our buffer
        close(fd);                       // close linux file
        fcbArray[returnFd].linuxFd = -1; //Free FCB
        return -1;
    }

    fcbArray[returnFd].buflen = 0; // have not read anything yet
    fcbArray[returnFd].index = 0;  // have not read anything yet
    return (returnFd);             // all set
}

// Interface to write a buffer
int b_write(int fd, char *buffer, int count)
{
    if (startup == 0)
        b_init(); //Initialize our system

    // check that fd is between 0 and (MAXFCBS-1)
    if ((fd < 0) || (fd >= MAXFCBS))
    {
        return (-1); //invalid file descriptor
    }

    if (fcbArray[fd].linuxFd == -1) //File not open for this descriptor
    {
        return -1;
    }

    if (counter_Write + count > BUFSIZE)
    {
        int left_Write = BUFSIZE - counter_Write;                 //bytes left to write until 512
        int right_Write = abs(count - left_Write);                //bytes needed to be coppied over
        memcpy(write_Buffer + counter_Write, buffer, left_Write); //Copy last few bytes to hit 512
        //memcpy(fcbArray[fd].ruf + counter_Write, buffer, ko); //Copy last few bytes to hit 512
        write(fcbArray[fd].linuxFd, write_Buffer, BUFSIZE);                     //Write 512 Bytes
        counter_Write = 0;                                                      //Reset Counter Pointer
        memcpy(write_Buffer + counter_Write, buffer + left_Write, right_Write); // Copy from Buffer up to last copy remaining left
        //memcpy(fcbArray[fd].ruf + counter_Write, buffer + ko, count_remaining); // Copy from Buffer up to last copy remaining left
        counter_Write = right_Write; //Set the counter at the last placed stopped
    }
	
    else
    {
        memcpy(write_Buffer + counter_Write, buffer, count);
        //memcpy(fcbArray[fd].ruf + counter_Write, buffer, count);
        counter_Write = counter_Write + count;
    }

    //Every time B_write is called,  load the writing, check buffer Holder > 512 , if its not just store & let go.
    //If buffer holder > 512 , write to the text file, take the rest of the buffer put it in the storage from beginning
    return 1;
}

void b_close(int fd)
{
    write(fcbArray[fd].linuxFd, write_Buffer, counter_Write); //Write Last Few Bytes in File
    close(fcbArray[fd].linuxFd);                              // close the linux file handle
    free(fcbArray[fd].buf);                                   // free the associated buffer
    free(fcbArray[fd].writeBuf);                              //Free writer buffer
    fcbArray[fd].buf = NULL;                                  // Safety First
    fcbArray[fd].linuxFd = -1;                                // return this FCB to list of available FCB's
}
struct stat st = {0}; //man stat

//Make Directory
int mfs_mkdir(const char *pathname, mode_t mode)
{
    //Making Directory if not already exists
    if (stat(pathname, &st) == -1)
    {
        mkdir(pathname, 0700);
    }
}

mfs_DIR *opendir(const char *name)
{
    DIR *dir;             //Pointer to Open Director
    struct dirent *entry; //Stuff in the Directory
    struct stat info;     //Information about Entry

    //Open
    dir = opendir(name);
    if (!dir)
    {
        printf("Directory not Found\n");
        return NULL;
    }
    //CLose
    closedir(name);
    return;
}
int closedir(mfs_DIR *dirp)
{
    int returnVal = closedir(dirp->dummy);
}
