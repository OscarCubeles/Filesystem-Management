/***********************************************
*
* @Purpose: Module to read and write data from an Ext2 file system volume
* @Author: Óscar Cubeles Ollé
* @Creation Date: March 2022
*
************************************************/
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "Ex2System.h"

int isFound = 0;
int isDelete = 0;


/***********************************************
*
* @Purpose: Returns an integer corresponding to the operation string, /info -> 0, /find -> 1, /delete -> 2, else -> -1
* @Parameters: char *operation: operation to be done on the Ext2 filesystem
*              Valid operations: /info, /find, /delete
*
* @Return: An integer corresponding to the operation string
*
************************************************/
int EXT2SYSTEM_getOperationNumber(char *operation){
	if(strcmp(operation,"/info") == 0){
		return 0;
	}else if(strcmp(operation,"/find") == 0){
		return 1;
	}else if(strcmp(operation,"/delete") == 0){
		return 2;
	}
	return -1;
}


/***********************************************
*
* @Purpose: Determine if the volume specidied by fd is an Ext2 volume
* @Parameters: int fd, file descriptor of the volume read
* @Return: 1 if it is ext, else 0
*
************************************************/
int Ex2System_isExt (int fd) {
	int buffer;
	lseek(fd, EXT_SYSTEM_SUPERBLOCK_OFFSET + EXT_SYSTEM_MAGIC_WORD_OFFSET, SEEK_SET);
	read(fd, &buffer, EXT_SYSTEM_MAGIC_WORD_SIZE);
	return buffer == EXT_SYSTEM_MAGIC_WORD;
}


/***********************************************
*
* @Purpose: Prints in screen the inode data
* @Parameters: ExtInodeData inode. Inode structure with inode metadata
* @Return: -
*
************************************************/
void EX2System_printInode(ExtInodeData inode){
	  printf("\nFilesystem: EXT2\n\nINFO INODE Inode\n");
	  printf("Size: %hu\nNum Inodes: %d\n", inode.s_inode_size, inode.s_inodes_count);
	  printf("First Inode: %d\nInodes Group: %d\nFree Inodes: %d\n", inode.s_first_ino, inode.s_inodes_per_group, inode.s_free_inodes_count);

}


/***********************************************
*
* @Purpose: Reads the inode metadata information from the volume file
* @Parameters: int fd, file descriptor of the volume read
* @Return:  ExtInodeData structure containing the information about an inode
*
************************************************/
ExtInodeData Ex2System_readInode (int fd) {
	ExtInodeData inode;

	//get the inode size
	lseek(fd, EXT_SYSTEM_INODE_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(inode.s_inode_size), EXT_SYSTEM_INODE_SIZE);
	//get the inode number of nodes
	lseek(fd, EXT_SYSTEM_INODE_COUNT_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET , SEEK_SET);
	read(fd, &(inode.s_inodes_count), EXT_SYSTEM_INODE_COUNT_SIZE);
	//get the first inode
	lseek(fd, EXT_SYSTEM_INODE_FIRST_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET , SEEK_SET);
	read(fd, &(inode.s_first_ino), EXT_SYSTEM_INODE_FIRST_SIZE);
	//get the inode group
	lseek(fd, EXT_SYSTEM_INODE_GROUP_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET , SEEK_SET);
	read(fd, &(inode.s_inodes_per_group), EXT_SYSTEM_INODE_GROUP_SIZE);
	//get the free inodes
	lseek(fd, EXT_SYSTEM_INODE_FREE_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET , SEEK_SET);
	read(fd, &(inode.s_free_inodes_count), EXT_SYSTEM_INODE_FREE_SIZE);
	//EX2System_printInode(inode);


	return inode;
}


/***********************************************
*
* @Purpose: Prints in screen the block data
* @Parameters: ExtBlockData block. Block structure with block metadata
* @Return: -
*
************************************************/
void EX2System_printBlock(ExtBlockData block){
		printf("\nINFO BLOCK\nSize Block: %d\nReserved blocks: %d\n", block.s_log_block_size, block.s_r_blocks_count);
		printf("Free blocks: %d\nTotal blocks: %d\nFirst block: %d\n", block.s_free_blocks_count, block.s_blocks_count, block.s_first_data_block);
		printf("Group blocks: %d\nFragmented Blocks: %d\n", block.s_blocks_per_group, block.s_frags_per_group);
}


/***********************************************
*
* @Purpose: Reads the blcok metadata information from the volume file
* @Parameters: int fd, file descriptor of the volume read
* @Return:  ExtBlockData structure containing the information about a block
*
************************************************/
ExtBlockData Ex2System_readBlock (int fd) {
	ExtBlockData block;
	//read the block size
	lseek(fd, EXT_SYSTEM_BLOCK_SIZE_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_log_block_size), EXT_SYSTEM_BLOCK_SIZE_SIZE);
	block.s_log_block_size = 1024 << block.s_log_block_size;	// shifting as it says in the page 11 of the manual

	//read the reserved blocks
	lseek(fd, EXT_SYSTEM_BLOCK_RESERVED_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_r_blocks_count), EXT_SYSTEM_BLOCK_RESERVED_SIZE);
	//read the free blocks
	lseek(fd, EXT_SYSTEM_BLOCK_FREE_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_free_blocks_count), EXT_SYSTEM_BLOCK_FREE_SIZE);
	//read the total blocks
	lseek(fd, EXT_SYSTEM_BLOCK_N_TOTALBLOCKS_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_blocks_count), EXT_SYSTEM_BLOCK_N_TOTALBLOCKS_SIZE);
	//read the first block
	lseek(fd, EXT_SYSTEM_BLOCK_FIRST_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_first_data_block), EXT_SYSTEM_BLOCK_FIRST_SIZE);
	//read the group blocks
	lseek(fd, EXT_SYSTEM_BLOCK_GROUP_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_blocks_per_group), EXT_SYSTEM_BLOCK_GROUP_SIZE);
	//read the fragmented groups
	lseek(fd, EXT_SYSTEM_BLOCK_FRAGS_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(block.s_frags_per_group), EXT_SYSTEM_BLOCK_FRAGS_SIZE);

	return block;
}


/***********************************************
*
* @Purpose: Prints in screen the Volume data
* @Parameters: ExtVolumeData block. Volume structure with volume metadata
* @Return: -
*
************************************************/
void EX2System_printVolume(ExtVolumeData volume){
	time_t rawtime;
	struct tm ts;
	char buffer[80];

	// Printing the name
	printf("\nINFO VOLUME\nVolume Name: %s\n",volume.s_volume_name);

	// Printing the last checked time
	rawtime = (time_t) volume.s_lastcheck;
	ts = *localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("%s\n", buffer);

	// Printing the last written time
	rawtime = (time_t) volume.s_wtime;
	ts = *localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("%s\n", buffer);

	// Printing the last mount time
	rawtime = (time_t) volume.s_mtime;
	ts = *localtime(&rawtime);
	strftime(buffer, sizeof(buffer), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
	printf("%s\n", buffer);
}


/***********************************************
*
* @Purpose: Reads the Volume metadata information from the volume file
* @Parameters: int fd, file descriptor of the volume read
* @Return:  ExtVolumeData structure containing the information about the volume
*
************************************************/
ExtVolumeData Ex2System_readVolume(int fd) {
	ExtVolumeData volume;

	//read the volume name
	lseek(fd, EXT_SYSTEM_VOLUME_NAME_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(volume.s_volume_name), EXT_SYSTEM_VOLUME_NAME_RBYTES);
	//read the last_check
	lseek(fd, EXT_SYSTEM_VOLUME_CHECKED_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(volume.s_lastcheck), EXT_SYSTEM_VOLUME_CHECKED_RBYTES);
	//read the last write
	lseek(fd, EXT_SYSTEM_VOLUME_WRITE_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(volume.s_wtime), EXT_SYSTEM_VOLUME_WRITE_RBYTES);
	//read the last mount
	lseek(fd, EXT_SYSTEM_VOLUME_MOUNT_OFFSET + EXT_SYSTEM_SUPERBLOCK_OFFSET, SEEK_SET);
	read(fd, &(volume.s_mtime), EXT_SYSTEM_VOLUME_MOUNT_RBYTES);
	return volume;
}


/***********************************************
*
* @Purpose: Executes the action according to operation and filename on an Ext2 filesystem
* @Parameters: int fd, file descriptor of the volume read
*              char* operation, operation to be executed /find, /delete,/info
*              char *file, file with which the action is executed
* @Return:  -
*
************************************************/
void EX2SYSTEM_executeOperation(char * operation, char* file, int volume_fd){
	ExtInodeData inode;
	ExtBlockData block;
	ExtVolumeData volume;
	// Reading EXT2 info filesystem data in all cases
	inode = Ex2System_readInode (volume_fd);
	block = Ex2System_readBlock (volume_fd);
	volume = Ex2System_readVolume(volume_fd);

	// Perform the action according to the operation
	switch(EXT2SYSTEM_getOperationNumber(operation)){
		default :
			printf("Unknown operation %s\n", operation);
			break;
		case 0:
			// /info Print the data read from the volume
			EX2System_printInode(inode);
			EX2System_printBlock(block);
			EX2System_printVolume(volume);
			break;
		// /find
		case 1:
			printf("You selected to find file: %s in EXT2 volume\n\n", file);
			// Finding the file and showing its size
			EX2System_findFile(file, volume_fd, block, inode,2);
			if(Ext2System_isFound() == 0){
				printf("Sorry, the file %s does not exist in the file system\n", file);
			}
			break;
		// /delte
		case 2:
			//printf("%s\n", file);
			// Setting the flag to delete the file if found
			isDelete = 1;
			// Finding and deleting the file
			EX2System_findFile(file, volume_fd, block, inode,2);
			if(Ext2System_isFound() == 0){
				printf("Sorry, the file %s does not exist in the file system\n", file);
			}
			break;

	}
}


/***********************************************
*
* @Purpose: Reads the blcok metadata information from the volume file
* @Parameters: int fd, file descriptor of the volume read
* @Return:  ExtBlockData structure containing the information about a block
*
************************************************/
void EX2System_findFile(char* filename, int volume_fd, ExtBlockData block, ExtInodeData inode, unsigned int root_inode){
	BlockGroupDescriptorTable bg_descriptor_table;
	InodeTableEntry inode_entry;
	DirEntry directory_entry;
	directory_entry.file_type = EXT2_FT_INIT;
	unsigned int dir_entry_block_position = 0;
	unsigned short ptr_inode_name = 0;
	unsigned short prev_dir_len = 0;

	// Filling the block group descriptor table that contains info about the inode bitmaps and tables
	Ext2System_fillBlockGroupDescriptorTable(volume_fd, block.s_log_block_size, &bg_descriptor_table );

	// Finding the inode entry from the table given the block descriptor and the current root inode ( for recusive calls, the root needs to be changed)
	inode_entry = Ext2System_findAndGetInode(root_inode,&bg_descriptor_table, block, inode, volume_fd );
	/*
	printf("%s\n%d\n", filename, block.s_log_block_size);

	printf("Inode size: %d\n", inode_entry.i_size);
	printf("Inode blocks: %d\n", inode_entry.i_blocks);
	for (int i = 0; i < 15; i++){
		printf("Block num %d: %d\n", i, inode_entry.i_block[i]);
	}*/

	// Iterating through the 12 direct blocks of the inode
	for(int i = 0; i < 12 && ptr_inode_name < inode_entry.i_size; i++){
		if(inode_entry.i_block[i] != 0){
			// Computing the position of the directory entry
			dir_entry_block_position = inode_entry.i_block[i] * block.s_log_block_size;
			// Iterating through the linked list of directory entries until the entry type is Unknown (i.e. until the last one from the list)
			for(int j = 0; directory_entry.file_type != EXT2_FT_UNKNOWN && ptr_inode_name < inode_entry.i_size; j++ ){
					prev_dir_len = directory_entry.rec_len;
					// Reading the directory entry
					directory_entry = Ext2System_readDirEntry(volume_fd, &ptr_inode_name,  dir_entry_block_position);
					// Safe checking that the pointer to the next inode name does not go beyond the size of the inode
					if (ptr_inode_name > inode_entry.i_size)	break;
					// Checking if the name is the same and it is not a directory
					if(strcmp(directory_entry.name,filename) == 0 && Ext2System_isDirectory(directory_entry.name, directory_entry.file_type) == 0){
							if(isDelete == 1){
									Ext2System_deleteEntry(ptr_inode_name,dir_entry_block_position, prev_dir_len, directory_entry, volume_fd);
							}else{
								//printf("Inode for this file: %d\n", directory_entry.inode);
								// finding the inode in the file system to get its size, instead of first inode, here its the inode from the directory entry
								InodeTableEntry aux_inode = Ext2System_findAndGetInode(directory_entry.inode, &bg_descriptor_table,  block,  inode,  volume_fd );
								printf("The file %s has %d bytes\n", directory_entry.name, aux_inode.i_size);
								//printf("Inode size: %d bytes\n", aux_inode.i_size);
							}
							isFound = 1;
					}
					// Recursive calls when it is a directory
					if(Ext2System_isDirectory(directory_entry.name, directory_entry.file_type) == 1){
						//printf("\nDepth\n\n");
						EX2System_findFile(filename, volume_fd, block, inode, directory_entry.inode);
					}
			}
		}
	}
	//printf("\nBack\n\n");
}




/***********************************************
*
* @Purpose: Reads and fills the information of the block group descriptor table
* @Parameters: int fd, file descriptor of the volume read
*              unsigned int block_size, size of one block of the Ext2 system read
*              BlockGroupDescriptorTable *bg_descriptor_table, block group descriptor table to be filled
* @Return:  -
*
************************************************/
void Ext2System_fillBlockGroupDescriptorTable(int fd, unsigned int block_size, 	BlockGroupDescriptorTable *bg_descriptor_table ){
	// block descriptor table starts at block 2, that is, offset = size superblock + one block
	lseek(fd, EXT_SYSTEM_SUPERBLOCK_OFFSET + block_size, SEEK_SET);
	read(fd, bg_descriptor_table, (int)sizeof(BlockGroupDescriptorTable)); // Reading the whole structure of the block descriptor table
	/*
	printf("Size: %d", (int)sizeof(BlockGroupDescriptorTable));
	printf("\n\n\n\n");
	printf("block_num_block_bitmap: %d\nblock_num_inode_bitmap: %d\n", bg_descriptor_table->bg_block_bitmap, bg_descriptor_table->bg_inode_bitmap);
	printf("block_num_first_indode_table_block: %d\nnum_free_blocks_group: %hu\n", bg_descriptor_table->bg_inode_table, bg_descriptor_table->bg_free_blocks_count);
	printf("num_free_inodes_group: %hu\nnum_dir_group: %hu\n", bg_descriptor_table->bg_free_inodes_count, bg_descriptor_table->bg_used_dirs_count);
	printf("alignment_to_word: %hu\n", bg_descriptor_table->bg_pad);
	printf("\n\n\n\n");*/

}


/***********************************************
*
* @Purpose: Function that gets the inode number from the inode table, (the formulas come from the page 22/34 of the documentation),
*           and returns the inode entry  from the inode table
* @Parameters: unsigned int first_inode, root inode from where this inode is located
*              int fd, file descriptor of the volume read
*              BlockGroupDescriptorTable *bg_descriptor_table, structure with the infomation of the block group descriptor
*              ExtBlockData block, structure with the information about a block
*              ExtInodeData inode, structure with the information about an inode
* @Return:  InodeTableEntry with the data from the inode entry from the inode table
*
************************************************/
InodeTableEntry Ext2System_findAndGetInode(unsigned int first_inode, BlockGroupDescriptorTable *bg_descriptor_table, ExtBlockData block, ExtInodeData inode, int volume_fd ){
	unsigned int inode_number = (first_inode - 1) % inode.s_inodes_per_group;																					// Inode number (from formula)

	unsigned int block_group_number  = (first_inode - 1) / inode.s_inodes_per_group; 							// Block group number (from formula)
	unsigned int block_group_offset  = block_group_number * block.s_blocks_per_group * block.s_log_block_size; 				// block offset as block group * blocks per group * block size

	// Pointing to the exact position: position of the block group + position of the table + position in the table
	unsigned int global_inode_position = block_group_offset + bg_descriptor_table->bg_inode_table * block.s_log_block_size + inode_number * inode.s_inode_size;


	InodeTableEntry inode_entry;
	//printf("global inode pos: %d\n", global_inode_position);
	//printf("first inode: %d\ninode number: ", first_inode);

	// Read the entry in the inode table
	lseek(volume_fd, global_inode_position, SEEK_SET);
	read(volume_fd, &inode_entry, sizeof(InodeTableEntry));
	/*
	printf("Inode size: %d\n", inode_entry.i_size);
	printf("Inode blocks: %d\n", inode_entry.i_blocks);
	for (int i = 0; i < 15; i++){
		printf("Block num %d: %d\n", i, inode_entry.i_block[i]);
	}*/

	return inode_entry;
}


/***********************************************
*
* @Purpose: Reads the directory entry given the pointer to the position and the pointer to the directory
* @Parameters: int fd, file descriptor of the volume read
*              unsigned short *ptr_inode_name, pointer to the entry inside the directory entry
*              int dir_entry_block_position, pointer to the directory entry
* @Return:  DirEntry structure containing the information about the directory entry read
*
************************************************/
DirEntry Ext2System_readDirEntry(int volume_fd, unsigned short *ptr_inode_name, int dir_entry_block_position){
	DirEntry directory_entry;
	lseek(volume_fd, dir_entry_block_position + *ptr_inode_name, SEEK_SET);
	read(volume_fd, &directory_entry.inode, sizeof(int));
	read(volume_fd, &directory_entry.rec_len, sizeof(short));
	read(volume_fd, &directory_entry.name_len, sizeof(char));
	read(volume_fd, &directory_entry.file_type, sizeof(char));
	read(volume_fd, directory_entry.name, directory_entry.name_len);
	directory_entry.name[(int)directory_entry.name_len] = '\0';
	//printf("File name: %s, file type: %d\n", directory_entry.name,directory_entry.file_type );
	// Pointing to the next directory entry
	*ptr_inode_name = *ptr_inode_name + directory_entry.rec_len;
	return directory_entry;
}


/***********************************************
*
* @Purpose: Checks if the file specified is a directory given its name and type
* @Parameters: int file_type, file type
*              char *filename, name of the inode
* @Return:  integer with 1 if it is a directory, 0 otherwise
*
************************************************/
int Ext2System_isDirectory(char *filename, int file_type){
	return file_type == EXT2_FT_DIR && strcmp (filename, "..") != 0 && strcmp (filename, ".") != 0;
}


/***********************************************
*
* @Purpose: Checks if the file has been found or not
* @Parameters: -
*
* @Return:  returns the value of the global variable isFound
*
************************************************/
int Ext2System_isFound(){
	return isFound;
}


// Linked list: prev->curr->"next"
// Here we read the prev  dir entry, and point to the "next" dir entry skipping the curr, as the curr is the one to be deleted
void Ext2System_deleteEntry(unsigned short ptr_next_inode_name, unsigned int dir_entry_block_position, unsigned short prev_dir_len, DirEntry directory_entry, int volume_fd){
	DirEntry aux_dir_entry;
	unsigned int ptr_prev_dir_entry = ptr_next_inode_name - directory_entry.rec_len - prev_dir_len;
	unsigned int ptr_curr_dir_entry = ptr_next_inode_name - directory_entry.rec_len;


	printf("File %s deleted\n", directory_entry.name);

	// Reading the prev dir entry
	lseek(volume_fd, dir_entry_block_position + ptr_prev_dir_entry, SEEK_SET);
	read(volume_fd, &aux_dir_entry.inode, sizeof(int));
	read(volume_fd, &aux_dir_entry.rec_len, sizeof(short));
	read(volume_fd, &aux_dir_entry.name_len, sizeof(char));
	read(volume_fd, &aux_dir_entry.file_type, sizeof(char));
	read(volume_fd, aux_dir_entry.name, aux_dir_entry.name_len);
	aux_dir_entry.name[(int)aux_dir_entry.name_len] = '\0';


	// The previous directory entry size must be equal to the its size plus the current one that we want to delete
	aux_dir_entry.rec_len = aux_dir_entry.rec_len + directory_entry.rec_len;
	// Write the new rec_len field
	lseek(volume_fd, dir_entry_block_position + ptr_prev_dir_entry + sizeof(int), SEEK_SET);
	write(volume_fd, &aux_dir_entry.rec_len, sizeof(short));


	// Going to the curr dir position and deleting all the entries there
	lseek(volume_fd, dir_entry_block_position + ptr_curr_dir_entry, SEEK_SET);
	bzero( &aux_dir_entry.inode, sizeof(int));
	bzero( &aux_dir_entry.rec_len, sizeof(short));
	bzero( aux_dir_entry.name, aux_dir_entry.name_len);
	bzero( &aux_dir_entry.name_len, sizeof(char));
	bzero( &aux_dir_entry.file_type, sizeof(char));
	write(volume_fd, &aux_dir_entry, sizeof(int) + sizeof(short) + sizeof(char) + sizeof(char));


}
