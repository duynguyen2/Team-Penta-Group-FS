/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: fsShell.c
*
* Description: Main driver for file system assignment.
*
* Make sure to set the #defined on the CMDxxxx_ON from 0 to 1 
* when you are ready to test that feature
*
**************************************************************/


#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <getopt.h>
#include <string.h>

#include "mfs.h"

/***************  START LINUX TESTING CODE FOR SHELL ***************/
#define TEMP_LINUX 0  //MUST be ZERO for working with your file system
#if (TEMP_LINUX == 1)
	// All the following it TEMPORARY ONLY - It allows testing the shell in linux
	// but using the test files system function calls  
	// This must be off for testing your file system
	
	#include <sys/stat.h>
	#include <dirent.h>
	#define mfs_mkdir	mkdir
	#define mfs_getcwd	getcwd
	#define mfs_setcwd	chdir
	#define mfs_rmdir	rmdir
	#define mfs_delete unlink
	
	
	mfs_DIR * mfs_opendir(const char *name)
		{
		DIR * dir;
		dir = opendir(name);
		return ((mfs_DIR *) dir);
		}
		
	struct mfs_dirent *mfs_readdir(mfs_DIR *dirp)
		{
		DIR *dir;
		dir = (DIR *) dirp;
		struct dirent * de;
		de = readdir (dir);
		
		return ((struct mfs_dirent *)de);
		}
		
	int mfs_closedir(mfs_DIR *dirp)
		{
		DIR *dir;
		dir = (DIR *) dirp;
		return (closedir (dir));
		}

	int mfs_stat(const char *path, struct mfs_stat *buf)
		{
		struct stat * path_stat;
		path_stat = (struct stat *) buf;
		return (stat (path, path_stat));
		}

	
	int mfs_isFile (char * path)
		{
		struct stat path_stat;
		stat(path, &path_stat);
		return S_ISREG(path_stat.st_mode);
		}
	
	int mfs_isDir (char * path)
		{
		struct stat path_stat;
		if (stat(path, &path_stat) != 0)
			return 0;
		return S_ISDIR(path_stat.st_mode);
		}
#endif
/***************  END LINUX TESTING CODE FOR SHELL ***************/




#define SINGLE_QUOTE	0x27
#define DOUBLE_QUOTE	0x22
#define BUFFERLEN		200
#define DIRMAX_LEN		4096

/****   SET THESE TO 1 WHEN READY TO TEST THAT COMMAND ****/
#define CMDLS_ON	1
#define CMDCP_ON	1
#define CMDMV_ON	1
#define CMDMD_ON	1
#define CMDRM_ON	1
#define CMDCP2L_ON	1
#define CMDCP2FS_ON	1
#define CMDCD_ON	1
#define CMDPWD_ON	1


typedef struct dispatch_t
	{
	char * command;
	int (*func)(int, char**);
	char * description;
	} dispatch_t, * dispatch_p;


int cmd_ls (int argcnt, char *argvec[]);
int cmd_cp (int argcnt, char *argvec[]);
int cmd_mv (int argcnt, char *argvec[]);
int cmd_md (int argcnt, char *argvec[]);
int cmd_rm (int argcnt, char *argvec[]);
int cmd_cp2l (int argcnt, char *argvec[]);
int cmd_cp2fs (int argcnt, char *argvec[]);
int cmd_cd (int argcnt, char *argvec[]);
int cmd_pwd (int argcnt, char *argvec[]);
int cmd_history (int argcnt, char *argvec[]);
int cmd_help (int argcnt, char *argvec[]);

dispatch_t dispatchTable[] = {
	{"ls", cmd_ls, "Lists the file in a directory"},
	{"cp", cmd_cp, "Copies a file - source [dest]"},
	{"mv", cmd_mv, "Moves a file - source dest"},
	{"md", cmd_md, "Make a new directory"},
	{"rm", cmd_rm, "Removes a file or directory"},
	{"cp2l", cmd_cp2l, "Copies a file from the test file system to the linux file system"},
	{"cp2fs", cmd_cp2fs, "Copies a file from the Linux file system to the test file system"},
	{"cd", cmd_cd, "Changes directory"},
	{"pwd", cmd_pwd, "Prints the working directory"},
	{"history", cmd_history, "Prints out the history"},
	{"help", cmd_help, "Prints out help"}
};

static int dispatchcount = sizeof (dispatchTable) / sizeof (dispatch_t);

// Display files for use by ls command
int displayFiles (mfs_DIR * dirp, int flall, int fllong)
	{
	if (dirp == NULL)	//get out if error
		return (-1);
	
	struct mfs_dirent * de;
	struct mfs_stat statbuf;
	
	de = mfs_readdir (dirp);
	printf("\n");
	while (de != NULL) 
		{
		if ((de->d_name[0] != '.') || (flall)) //if not all and starts with '.' it is hidden
			{
			if (fllong)
				{
				mfs_stat (de->d_name, &statbuf);
				printf ("%s    %9ld   %s\n", mfs_isDir(de->d_name)?"D":"-", statbuf.st_size, de->d_name);
				}
			else
				{
				printf ("%s\n", de->d_name);
				}
			}
		de = mfs_readdir (dirp);
		}
	mfs_closedir (dirp);
	return 0;
	}
	

/****************************************************
*  ls commmand
****************************************************/
int cmd_ls (int argcnt, char *argvec[])
	{
#if (CMDLS_ON == 1)				
	int option_index;
	int c;
	int fllong;
	int flall;
	char cwd[DIRMAX_LEN];
		
	static struct option long_options[] = 
		{
			/* These options set their assigned flags to value and return 0 */
			/* These options don't set flags and return the value */	 
			{"long",	no_argument, 0, 'l'},  
			{"all",		no_argument, 0, 'a'},
			{"help",	no_argument, 0, 'h'},
			{0,			0,       0,  0 }
		};
		
	option_index = 0;
#ifdef __GNU_LIBRARY__
    // WORKAROUND
    // Setting "optind" to 0 triggers initialization of getopt private
    // structure (holds pointers on data between calls). This helps
    // to avoid possible memory violation, because data passed to getopt_long()
    // could be freed between parse() calls.
    optind = 0;
#else
    // "optind" is used between getopt() calls to get next argument for parsing and should be
    // initialized before each parsing loop.
    optind = 1;
#endif
	fllong = 0;
	flall = 0;

	while (1)
		{	
		c = getopt_long(argcnt, argvec, "alh",
				long_options, &option_index);
				
		if (c == -1)
		   break;

		switch (c) {
			case 0:			//flag was set, ignore
			   printf("Unknown option %s", long_options[option_index].name);
			   if (optarg)
				   printf(" with arg %s", optarg);
			   printf("\n");
				break;
				
			case 'a':
				flall = 1;
				break;
				
			case 'l':
				fllong = 1;
				break;
				
			case 'h':
			default:
				printf ("Usage: ls [--all-a] [--long/-l] [pathname]\n");
				return (-1);
				break;
			}
		}
	
	
	if (optind < argcnt)
		{
		//processing arguments after options
		for (int k = optind; k < argcnt; k++)
			{
			if (mfs_isDir(argvec[k]))
				{
				mfs_DIR * dirp;
				dirp = mfs_opendir (argvec[k]);
				displayFiles (dirp, flall, fllong);
				}
			else // it is just a file ?
				{
				if (mfs_isFile (argvec[k]))
					{
					//no support for long format here
					printf ("%s\n", argvec[k]);
					}
				else
					{
					printf ("%s is not found\n", argvec[k]);
					}
				}
			}		
		}
	else   // no pathname/filename specified - use cwd
		{
		char * path = mfs_getcwd(cwd, DIRMAX_LEN);	//get current working directory
		mfs_DIR * dirp;
		dirp = mfs_opendir (path);
		return (displayFiles (dirp, flall, fllong));
		}
	return 0;
#endif
	}

	
/****************************************************
*  Copy file commmand
****************************************************/
	
int cmd_cp (int argcnt, char *argvec[])
	{
#if (CMDCP_ON == 1)	
	int testfs_src_fd;
	int testfs_dest_fd;
	char * src;
	char * dest;
	int readcnt;
	char buf[BUFFERLEN];
	
	switch (argcnt)
		{
		case 2:	//only one name provided
			src = argvec[1];
			dest = src;
			break;
			
		case 3:
			src = argvec[1];
			dest = argvec[2];
			break;
		
		default:
			printf("Usage: cp srcfile [destfile]\n");
			return (-1);
		}
	
	
	testfs_src_fd = b_open (src, O_RDONLY);
	testfs_dest_fd = b_open (dest, O_WRONLY | O_CREAT | O_TRUNC);
	do 
		{
		readcnt = b_read (testfs_src_fd, buf, BUFFERLEN);
		b_write (testfs_dest_fd, buf, readcnt);
		} while (readcnt == BUFFERLEN);
	b_close (testfs_src_fd);
	b_close (testfs_dest_fd);
	return 0;
#endif
	}
	
/****************************************************
*  Move file commmand
****************************************************/
int cmd_mv (int argcnt, char *argvec[])
	{
#if (CMDMV_ON == 1)				
	return -99;
	// **** TODO ****  For you to implement	
#endif
	}

/****************************************************
*  Make Directory commmand
****************************************************/
// Make Directory	
int cmd_md (int argcnt, char *argvec[])
	{
#if (CMDMD_ON == 1)				
	if (argcnt != 2)
		{
		printf("Usage: md pathname\n");
		return -1;
		}
	else
		{
		return(mfs_mkdir(argvec[1], 0777));
		}
#endif
	}
	
/****************************************************
*  Remove directory or file commmand
****************************************************/
int cmd_rm (int argcnt, char *argvec[])
	{
#if (CMDRM_ON == 1)
	if (argcnt != 2)
		{
		printf ("Usage: rm path\n");
		return -1;
		}
		
	char * path = argvec[1];	
	
	//must determine if file or directory
	if (mfs_isDir (path))
		{
		return (mfs_rmdir (path));
		}		
	if (mfs_isFile (path))
		{
		return (mfs_delete(path));
		}	
		
	printf("The path %s is neither a file not a directory\n", path);
	return -1;
#endif
	}
	
/****************************************************
*  Copy file from test file system to Linux commmand
****************************************************/
int cmd_cp2l (int argcnt, char *argvec[])
	{
#if (CMDCP2L_ON == 1)				
	int testfs_fd;
	int linux_fd;
	char * src;
	char * dest;
	int readcnt;
	char buf[BUFFERLEN];
	
	switch (argcnt)
		{
		case 2:	//only one name provided
			src = argvec[1];
			dest = src;
			break;
			
		case 3:
			src = argvec[1];
			dest = argvec[2];
			break;
		
		default:
			printf("Usage: cp2l srcfile [Linuxdestfile]\n");
			return (-1);
		}
	
	
	testfs_fd = b_open (src, O_RDONLY);
	linux_fd = open (dest, O_WRONLY | O_CREAT | O_TRUNC);
	do 
		{
		readcnt = b_read (testfs_fd, buf, BUFFERLEN);
		write (linux_fd, buf, readcnt);
		} while (readcnt == BUFFERLEN);
	b_close (testfs_fd);
	close (linux_fd);
	return 0;
#endif
	}
	
/****************************************************
*  Copy file from Linux to test file system commmand
****************************************************/
int cmd_cp2fs (int argcnt, char *argvec[])
	{
#if (CMDCP2FS_ON == 1)				
	int testfs_fd;
	int linux_fd;
	char * src;
	char * dest;
	int readcnt;
	char buf[BUFFERLEN];
	
	switch (argcnt)
		{
		case 2:	//only one name provided
			src = argvec[1];
			dest = src;
			break;
			
		case 3:
			src = argvec[1];
			dest = argvec[2];
			break;
		
		default:
			printf("Usage: cp2fs Linuxsrcfile [destfile]\n");
			return (-1);
		}
	
	
	testfs_fd = b_open (dest, O_WRONLY | O_CREAT | O_TRUNC);
	linux_fd = open (src, O_RDONLY);
	do 
		{
		readcnt = read (linux_fd, buf, BUFFERLEN);
		b_write (testfs_fd, buf, readcnt);
		} while (readcnt == BUFFERLEN);
	b_close (testfs_fd);
	close (linux_fd);
	return 0;
#endif
	}
	
/****************************************************
*  cd commmand
****************************************************/
int cmd_cd (int argcnt, char *argvec[])
	{
#if (CMDCD_ON == 1)	
	if (argcnt != 2)
		{
		printf ("Usage: cd path\n");
		return (-1);
		}
	char * path = argvec[1];	//argument
	
	if (path[0] == '"')
		{
		if (path[strlen(path)-1] == '"')
			{
			//remove quotes from string
			path = path + 1;
			path[strlen(path) - 1] = 0;
			}
		}
	int ret = mfs_setcwd (path);
	if (ret != 0)	//error
		{
		printf ("Could not change path to %s\n", path);
		return (ret);
		}			
	return 0;
#endif
	}
	
/****************************************************
*  PWD commmand
****************************************************/
int cmd_pwd (int argcnt, char *argvec[])
	{
#if (CMDPWD_ON == 1)
	char * dir_buf = malloc (DIRMAX_LEN +1);
	char * ptr;	
	ptr = mfs_getcwd (dir_buf, DIRMAX_LEN);	
	if (ptr == NULL)			//an error occurred
		{
		printf ("An error occurred while trying to get the current working directory\n");
		}
	else
		{
		printf ("%s\n", ptr);
		}
	free (dir_buf);
	dir_buf = NULL;
	ptr = NULL;
	
	return 0;
#endif
	}

/****************************************************
*  History commmand
****************************************************/
int cmd_history (int argcnt, char *argvec[])
	{
	HIST_ENTRY * he;
	int i = 0;
	
	
	for (i = history_base; i <= history_length; i++)
		{
		he = history_get(i);
		
		if (he != NULL)
			{
			printf ("%s\n", he->line);
			}
		}
	return 0;
	}
	
/****************************************************
*  Help commmand
****************************************************/
int cmd_help (int argcnt, char *argvec[])
	{
	for (int i = 0; i < dispatchcount; i++)
		{
		printf ("%s\t%s\n", dispatchTable[i].command, dispatchTable[i].description);
		}
	return 0;
	} 

void processcommand (char * cmd)
	{
	int cmdLen;
	char ** cmdv;	//command vector
	int cmdc;		//command count
	int i, j;
	
	cmdLen = strlen(cmd);
	cmdv = (char **) malloc (sizeof(char *) * ((cmdLen/2)+2));
	cmdc = 0;
	
	cmdv[cmdc] = cmd;
	++cmdc;
	
	for (i = 0; i < cmdLen; i++)
		{
		switch (cmd[i])
			{
			case ' ':
				cmd[i] = 0;	//NULL terminate prior string
				while (cmd[i+1] == ' ')  // null at end will prevent from overshooting string
					{
					i++;
					}
				if ((i+1) < cmdLen)	//there is still more
					{
					cmdv[cmdc] = &cmd[i+1];
					++cmdc;
					}
				break;
				
			case '\\':
				++i;	//skip next character
				break;
				
			case DOUBLE_QUOTE:	//ignore everything till next quote (unless unterminated)
				for (j = i+1; j < cmdLen; j++)
					{
					if (cmd[j] == '\\')
						{
						++j;	//skip next character
						}	
					else if (cmd[j] == DOUBLE_QUOTE)
						{
						break;
						}
					}
				if (j >= cmdLen)
					{
					printf("Unterminated string\n");
					free(cmdv);
					cmdv = NULL;
					return;
					}
				i=j;
				break;
				
			case SINGLE_QUOTE:
				for (j = i+1; j < cmdLen; j++)
					{
					if (cmd[j] == '\\')
						{
						++j;	//skip next character
						}	
					else if (cmd[j] == SINGLE_QUOTE)
						{
						break;
						}
					}
				if (j >= cmdLen)
					{
					printf("Unterminated string\n");
					free(cmdv);
					cmdv = NULL;
					return;
					}
				i=j;
				break;
			default:
				break;
			}
		}
		
#ifdef COMMAND_DEBUG
	for (i = 0; i < cmdc; i++)
		{
		printf("%s: length %d\n", cmdv[i], strlen(cmdv[i]));
		}
#endif		
	cmdv[cmdc] = 0;		//just be safe - null terminate array of arguments
	
	for (i = 0; i < dispatchcount; i++)
		{
		if (strcmp(dispatchTable[i].command, cmdv[0]) == 0)
			{
			dispatchTable[i].func(cmdc,cmdv);
			free (cmdv);
			cmdv = NULL;
			return;
			}
		}
	printf("%s is not a regonized command.\n", cmdv[0]);
	cmd_help(cmdc, cmdv);	
	free (cmdv);
	cmdv = NULL;
	}



int main (int argc, char * argv[])
	{
	char * cmdin;
	char * cmd;
	HIST_ENTRY *he;
		
	using_history();
	stifle_history(200);	//max history entries
	
	while (1)
		{
		cmdin = readline("Prompt > ");
#ifdef COMMAND_DEBUG
		printf ("%s\n", cmdin);
#endif
		
		cmd = malloc (strlen(cmdin) + 30);
		strcpy (cmd, cmdin);
		free (cmdin);
		cmdin = NULL;
		
		if (strcmp (cmd, "exit") == 0)
			{
			free (cmd);
			cmd = NULL;
			// exit while loop and terminate shell
			break;
			}
			
		if ((cmd != NULL) && (strlen(cmd) > 0))
			{
			he = history_get(history_length);
			if (!((he != NULL) && (strcmp(he->line, cmd)==0)))
				{
				add_history(cmd);
				}
			processcommand (cmd);
			}
				
		free (cmd);
		cmd = NULL;		
		} // end while
	}