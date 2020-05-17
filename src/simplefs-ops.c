#include "simplefs-ops.h"
#include "simplefs-disk.h"
extern struct filehandle_t file_handle_array[MAX_OPEN_FILES]; // Array for storing opened files

int max(int a, int b) { return (a > b) ? a : b; }
int min(int a, int b) { return (a < b) ? a : b; }

static int simplefs_search(char *filename)
{
	//Search file with name 'filename' from disk

	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));

	for (int i = 0; i < NUM_INODES; ++i)
	{
		simplefs_readInode(i, inode);
		if (strcmp(inode->name, filename) == 0)
		{
			return i;
		}
	}

	free(inode);

	return -1;
}

int simplefs_create(char *filename)
{
	//Create file with name 'filename' from disk

	int inode_Num;

	inode_Num = simplefs_search(filename);

	if (inode_Num == -1)
	{
		inode_Num = simplefs_allocInode();

		if (inode_Num == -1)
		{
			return -1;
		}
		else
		{
			struct inode_t *inode_Ptr = (struct inode_t *)malloc(sizeof(struct inode_t));

			simplefs_readInode(inode_Num, inode_Ptr);

			memcpy(inode_Ptr->name, filename, MAX_NAME_STRLEN);
			inode_Ptr->status = INODE_IN_USE;

			simplefs_writeInode(inode_Num, inode_Ptr);

			free(inode_Ptr);
		}
	}
	else
	{
		return -1;
	}

	return inode_Num;
}

void simplefs_delete(char *filename)
{
	// delete file with name 'filename' from disk

	int inode_Num = simplefs_search(filename);

	if (inode_Num != -1)
	{
		struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
		struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));

		simplefs_readSuperBlock(superblock);
		simplefs_readInode(inode_Num, inode);

		inode->status = INODE_FREE;
		inode->file_size = 0;
		for (int i = 0; i < MAX_FILE_SIZE; i++)
		{
			if (inode->direct_blocks[i] != -1)
			{
				int block_Num = inode->direct_blocks[i];
				inode->direct_blocks[i] = -1;

				assert(superblock->datablock_freelist[block_Num] == DATA_BLOCK_USED);
				superblock->datablock_freelist[block_Num] = DATA_BLOCK_FREE;
			}
		}

		assert(superblock->inode_freelist[inode_Num] == INODE_IN_USE);
		superblock->inode_freelist[inode_Num] = INODE_FREE;

		simplefs_writeSuperBlock(superblock);
		simplefs_writeInode(inode_Num, inode);

		free(superblock);
		free(inode);
	}
}

int simplefs_open(char *filename)
{
	//open file with name 'filename'

	int inode_Num = simplefs_search(filename);

	if (inode_Num == -1)
	{
		return -1;
	}
	else
	{
		for (int i = 0; i < MAX_OPEN_FILES; i++)
		{
			if (file_handle_array[i].inode_number == inode_Num)
			{
				return i;
			}
		}

		for (int i = 0; i < MAX_OPEN_FILES; i++)
		{
			if (file_handle_array[i].inode_number == -1)
			{
				file_handle_array[i].inode_number = inode_Num;
				return i;
			}
		}
	}

	return -1;
}

void simplefs_close(int file_handle)
{
	//close file with name 'filename'

	assert(0 <= file_handle && file_handle < MAX_OPEN_FILES);
	assert(file_handle_array[file_handle].inode_number != -1);

	file_handle_array[file_handle].inode_number = -1;
	file_handle_array[file_handle].offset = 0;
}

int simplefs_read(int file_handle, char *buf, int nbytes)
{
	//read 'nbytes' of data into 'buf' from file pointed by 'file_handle' starting at current offset

	assert(0 <= file_handle && file_handle <= MAX_OPEN_FILES);
	assert(nbytes > 0);

	//Check overflow
	int file_Inode = file_handle_array[file_handle].inode_number;
	int file_Offset = file_handle_array[file_handle].offset;

	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	simplefs_readInode(file_Inode, inode);
	assert(file_Offset + nbytes <= inode->file_size);

	//Calculate number of data blocks required
	int start_DirectBlock_Index = file_Offset / BLOCKSIZE;
	int start_DirectBlock_Offset = file_Offset % BLOCKSIZE;
	int start_DirectBlock_Capacity = BLOCKSIZE - start_DirectBlock_Offset;
	int dataBlock_req = 1;

	if (nbytes > start_DirectBlock_Capacity)
	{
		int left = nbytes - start_DirectBlock_Capacity;
		dataBlock_req += left / BLOCKSIZE;

		if (left % BLOCKSIZE > 0)
			++dataBlock_req;
	}

	//Find the required data blocks
	int dataBlock_allocArray[dataBlock_req];
	int dataBlock_allocIndex = 0;
	for (int i = start_DirectBlock_Index; dataBlock_allocIndex < dataBlock_req; ++i)
	{
		dataBlock_allocArray[dataBlock_allocIndex++] = inode->direct_blocks[i];
	}

	//Read data blocks
	char tempBuf[BLOCKSIZE];
	int curr_dataBlock = inode->direct_blocks[start_DirectBlock_Index];
	int curr_readSize = min(nbytes, start_DirectBlock_Capacity);

	simplefs_readDataBlock(curr_dataBlock, tempBuf);
	bzero(tempBuf + curr_readSize, start_DirectBlock_Capacity - curr_readSize);
	sprintf(buf, "%s", tempBuf + start_DirectBlock_Offset);
	buf += curr_readSize;
	nbytes -= curr_readSize;

	while (nbytes > 0)
	{
		curr_dataBlock = inode->direct_blocks[++start_DirectBlock_Index];
		curr_readSize = min(nbytes, BLOCKSIZE);

		simplefs_readDataBlock(curr_dataBlock, tempBuf);
		if (curr_readSize < BLOCKSIZE)
			bzero(tempBuf + curr_readSize, BLOCKSIZE - curr_readSize);
		sprintf(buf, "%s", tempBuf);
		buf += curr_readSize;
		nbytes -= curr_readSize;
	}

	if (nbytes > 0)
		return -1;
	*buf = '\0';

	free(inode);

	return 0;
}

int simplefs_write(int file_handle, char *buf, int nbytes)
{
	//write `nbytes` of data from `buf` to file pointed by `file_handle` starting at current offset

	assert(nbytes > 0);

	//Check overflow
	int file_Inode = file_handle_array[file_handle].inode_number;
	int file_Offset = file_handle_array[file_handle].offset;

	assert(file_Offset + nbytes <= MAX_FILE_SIZE * BLOCKSIZE);

	//Calculate number of data blocks required
	int start_DirectBlock_Index = file_Offset / BLOCKSIZE;
	int start_DirectBlock_Offset = file_Offset % BLOCKSIZE;
	int start_DirectBlock_Capacity = BLOCKSIZE - start_DirectBlock_Offset;
	int dataBlock_req = 1;

	if (nbytes > start_DirectBlock_Capacity)
	{
		int left = nbytes - start_DirectBlock_Capacity;
		dataBlock_req += left / BLOCKSIZE;

		if (left % BLOCKSIZE > 0)
			++dataBlock_req;
	}

	//Allocate required number of data blocks
	struct superblock_t *superblock = (struct superblock_t *)malloc(sizeof(struct superblock_t));
	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));

	simplefs_readSuperBlock(superblock);
	simplefs_readInode(file_Inode, inode);

	//Fulfill requirement from aleady allocated Inode direct blocks
	int dataBlock_allocArray[dataBlock_req];
	int dataBlock_allocIndex = 0;
	for (int i = start_DirectBlock_Index; dataBlock_allocIndex < dataBlock_req; ++i)
	{
		if (inode->direct_blocks[i] == -1)
			break;
		else
			dataBlock_allocArray[dataBlock_allocIndex++] = inode->direct_blocks[i];
	}

	//Fulfill requirement from data block free list
	for (int i = 0; dataBlock_allocIndex < dataBlock_req && i < NUM_DATA_BLOCKS; i++)
	{
		if (superblock->datablock_freelist[i] == DATA_BLOCK_FREE)
		{
			superblock->datablock_freelist[i] = DATA_BLOCK_USED;
			dataBlock_allocArray[dataBlock_allocIndex++] = i;
		}
	}

	if (dataBlock_allocIndex != dataBlock_req)
		return -1;

	//Write data blocks
	char tempBuf[BLOCKSIZE];
	int save = nbytes;

	dataBlock_allocIndex = 0;
	int curr_dataBlock = dataBlock_allocArray[dataBlock_allocIndex];
	int curr_WriteSize = min(nbytes, start_DirectBlock_Capacity);

	//Writing first data block
	if (inode->direct_blocks[start_DirectBlock_Index] == -1)
	{
		bzero(tempBuf, BLOCKSIZE);
		memcpy(tempBuf, buf, curr_WriteSize);
	}
	else
	{
		simplefs_readDataBlock(curr_dataBlock, tempBuf);
		memcpy(tempBuf + start_DirectBlock_Offset, buf, curr_WriteSize);
	}
	simplefs_writeDataBlock(curr_dataBlock, tempBuf);
	buf += curr_WriteSize;
	nbytes -= curr_WriteSize;

	//Writing subsequent blocks
	while (nbytes > 0)
	{
		curr_dataBlock = dataBlock_allocArray[++dataBlock_allocIndex];
		curr_WriteSize = min(nbytes, BLOCKSIZE);

		if (curr_WriteSize < BLOCKSIZE)
		{
			if (inode->direct_blocks[start_DirectBlock_Index + dataBlock_allocIndex] == -1)
				bzero(tempBuf, BLOCKSIZE);
			else
				simplefs_readDataBlock(curr_dataBlock, tempBuf);
		}
		memcpy(tempBuf, buf, curr_WriteSize);
		simplefs_writeDataBlock(curr_dataBlock, tempBuf);
		buf += curr_WriteSize;
		nbytes -= curr_WriteSize;
	}

	if (buf[0] != '\0' || nbytes > 0)
		return -1;

	//Update Inode & file handle
	inode->file_size = max(inode->file_size, file_Offset + save);

	dataBlock_allocIndex = 0;
	for (int i = start_DirectBlock_Index; dataBlock_allocIndex < dataBlock_req; ++i)
		inode->direct_blocks[i] = dataBlock_allocArray[dataBlock_allocIndex++];

	//Push changes to disk
	simplefs_writeSuperBlock(superblock);
	simplefs_writeInode(file_Inode, inode);

	free(superblock);
	free(inode);

	return 0;
}

int simplefs_seek(int file_handle, int nseek)
{
	//increase `file_handle` offset by `nseek`

	struct inode_t *inode = (struct inode_t *)malloc(sizeof(struct inode_t));
	int file_Inode = file_handle_array[file_handle].inode_number;
	int file_Offset = file_handle_array[file_handle].offset;

	simplefs_readInode(file_Inode, inode);

	file_Offset += nseek;
	if (file_Offset < 0 || inode->file_size < file_Offset)
		return -1;

	file_handle_array[file_handle].offset = file_Offset;

	free(inode);

	return 0;
}