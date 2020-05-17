
/*******************************************************************
						DISK EMULATION
********************************************************************/

#ifndef SIMPLEFS_DISK
#define SIMPLEFS_DISK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

#define  BLOCKSIZE 				64
#define  NUM_BLOCKS 			35 	
#define  NUM_DATA_BLOCKS 		30
#define  NUM_INODE_BLOCKS 		4
#define  NUM_INODES_PER_BLOCK 	2
#define  NUM_INODES 			8
#define  MAX_FILES 				8
#define  MAX_FILE_SIZE 			4   
#define  MAX_OPEN_FILES 		8 //20?  
#define  MAX_NAME_STRLEN 		8
#define  INODE_FREE 			'x'
#define  INODE_IN_USE 			'1'
#define  DATA_BLOCK_FREE 		'x'
#define  DATA_BLOCK_USED 		'1'											
#define  line 					printf("\n");

/*

	********************** superblock_t **********************
	 _______________________________________________________
    |_______________________disk_name_______________________|  
    |__i0__|__i1__|__i2__|__i3__|__i4__|__i5__|__i6__|__i7__|
    |__d0__|__d1__|__d2__|__d3__|__d4__|__d5__|__d6__|__d7__|
    |__d8__|__d9__|__d10_|__d11_|__d12_|__d13_|__d14_|__d15_|
    |__d16_|__d17_|__d18_|__d19_|__d20_|__d21_|__d22_|__d23_|
    |__d24_|__d25_|__d26_|__d27_|__d28_|__d29_|______|______|
    |_______________________________________________________|
    |_______________________________________________________|


	************************* inode_t *************************
	 _______________________________________________________
	|_______________________file_name_______________________|
	|___________status__________|___________size____________|
	|___________db[0]___________|___________db[1]___________|
	|___________db[2]___________|___________db[3]___________|


	filehandle_t -->  < inode_number , offset >

*/


struct superblock_t
{
	char name[MAX_NAME_STRLEN]; 				// "simplefs" after formatting
	char inode_freelist[NUM_INODES];			// INODE_FREE if free, INODE_IN_USE if used
	char datablock_freelist[NUM_DATA_BLOCKS];   // DATA_BLOCK_FREE if free, DATA_BLOCK_USED if used
};	

struct inode_t
{	
	char name[MAX_NAME_STRLEN];					// name of the file                             
	int status;									// INODE_FREE if free, INODE_IN_USE if used		
	int file_size;								// size of the file in bytes					
	int direct_blocks[MAX_FILE_SIZE];			// -1 if free, block number if used				
};

struct filehandle_t
{
	int inode_number; 							// Inode number for the file
	int offset;		  							// current offset in opened file
};


void simplefs_formatDisk();
void simplefs_dump();

void simplefs_readSuperBlock  (struct superblock_t *superblock);
void simplefs_writeSuperBlock (struct superblock_t *superblock);

int  simplefs_allocInode ();
void simplefs_freeInode  (int inodenum);
void simplefs_readInode  (int inodenum, struct inode_t *inodeptr);
void simplefs_writeInode (int inodenum, struct inode_t *inodeptr); 

int  simplefs_allocDataBlock ();
void simplefs_freeDataBlock  (int blocknum);
void simplefs_readDataBlock  (int blocknum, char *buf);
void simplefs_writeDataBlock (int blocknum, char *buf);

#endif