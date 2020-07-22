/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: fsLow.h
*
* Description: 
*	This is the file system interface.
*	This is the interface needed by the driver to interact with
*	your filesystem.
*
**************************************************************/
#ifndef _MFS_H
#define _MFS_H
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

// The following should be in b_io.h but included for for completness
#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open (char * filename, int flags);
int b_read (int fd, char * buffer, int count);
int b_write (int fd, char * buffer, int count);
int b_seek (int fd, off_t offset, int whence);
void b_close (int fd);

#endif


struct mfs_dirent
{
    ino_t          d_ino;       /* inode number  - not used by driver*/
    off_t          d_off;       /* offset to the next dirent  - not used by driver*/
    unsigned short d_reclen;    /* length of this record */
    unsigned char  d_type;      /* type of file; not supported
                                   by all file system types  - not used by driver*/
    char           d_name[256]; /* filename */
};

/*****TO DO:  Fill in this structure with what your open/read directory needs  *****/

typedef struct
{
	int dummy;
} mfs_DIR;

int mfs_mkdir(const char *pathname, mode_t mode);
int mfs_rmdir(const char *pathname);
mfs_DIR * opendir(const char *name);
struct mfs_dirent *readdir(mfs_DIR *dirp);
int closedir(mfs_DIR *dirp);

char * mfs_getcwd(char *buf, size_t size);
char * mfs_setcwd(char *buf);   //linux chdir



struct mfs_stat {
	dev_t     st_dev;     /* ID of device containing file - not needed by driver*/
	ino_t     st_ino;     /* inode number - not needed by driver program*/
	mode_t    st_mode;    /* protection  - not needed by driver program*/
	nlink_t   st_nlink;   /* number of hard links - not needed by driver program*/
	uid_t     st_uid;     /* user ID of owner - not needed by driver program*/
	gid_t     st_gid;     /* group ID of owner - not needed by driver program*/
	dev_t     st_rdev;    /* device ID (if special file) - not needed by driver program*/
	off_t     st_size;    /* total size, in bytes */
	blksize_t st_blksize; /* blocksize for file system I/O */
	blkcnt_t  st_blocks;  /* number of 512B blocks allocated */
	time_t    st_accesstime;   /* time of last access */
	time_t    st_modtime;   /* time of last modification */
	time_t    st_createtime;   /* time of last status change */
	
	/* add additional attributes here for your file system */
};

int mfs_stat(const char *path, struct mfs_stat *buf);

#endif

