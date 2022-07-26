/***********************************************
*
* @Purpose: Module to read and write data from a FAT16 file system volume
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


#include "FatSystem.h"

int fat_isFound = 0;
int fat_isDelete = 0;
char *uppercase_name;


/***********************************************
*
* @Purpose: Returns an integer corresponding to the operation string, /info -> 0, /find -> 1, /delete -> 2, else -> -1
* @Parameters: char *operation: operation to be done on the Ext2 filesystem
*              Valid operations: /info, /find, /delete
*
* @Return: An integer corresponding to the operation string
*
************************************************/
int FatSystem_getOperationNumber(char *operation){
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
* @Purpose: Executes the action according to operation and filename on a FAT16 filesystem
* @Parameters: int volume_fd, file descriptor of the volume read
*              char* operation, operation to be executed /find, /delete,/info
*              char *file, file with which the action is executed
* @Return:  -
*
************************************************/
void FatSystem_executeOperation(char * operation, char* file, int volume_fd){
  FatSystem fat_system;
	unsigned int root_address = 0;
	fat_system = FatSystem_readSystem(volume_fd);

	// Converting the name in the correct format
	//printf("%s\n", file);

	//printf("%s\n", file);

	switch(FatSystem_getOperationNumber(operation)){
		case 0:
			// Reading and writing FAT16 info filesystem data
      FatSystem_displayFatInfo(fat_system);
			break;
		case 1:
			root_address = FatSystem_calculateRootDirectory(fat_system);
			FatSystem_fileToUpper(file);
			FatSystem_findFile(file, volume_fd, root_address,fat_system);
			if(fat_isFound == 0){
				printf("Sorry, there is no file %s in the filesystem\n", file);
			}
			break;
		case 2:
			fat_isDelete = 1;
			FatSystem_fileToUpper(file);
			root_address = FatSystem_calculateRootDirectory(fat_system);
			FatSystem_findFile(file, volume_fd, root_address,fat_system);
			if(fat_isFound == 0){
				printf("Sorry, there is no file %s in the filesystem\n", file);
			}
			break;
		default :
			printf("Unknown operation %s\n", operation);
			break;
	}
}


/***********************************************
*
* @Purpose: Converts a string of characters to uppercase
* @Parameters: char *file: string to be converted to upper case
* @Return:  -
*
************************************************/
void FatSystem_fileToUpper(char *file){
	uppercase_name = (char *)malloc((int)strlen(file));
	strcpy(uppercase_name, file);
	//printf("%s\n", uppercase_name);
	//printf("File name len: %d\n", (int)strlen(file));
	for (int i = 0; uppercase_name[i]!='\0'; i++) {
		 if(uppercase_name[i] >= 'a' && file[i] <= 'z') {
				uppercase_name[i] = uppercase_name[i] -'a'+'A';
		 }
	}
}


/***********************************************
*
* @Purpose: Determine if the volume specidied by fd is a FAT16 volume
* @Parameters: int fd, file descriptor of the volume read
* @Return: 1 if it is ext, else 0
*
************************************************/
int FatSystem_isFatSystem(int fd){
  	char * buff = (char *) malloc (sizeof(char) * FAT_SYSTEM_SYSTYPE_SIZE);
  	lseek(fd, FAT_SYSTEM_SYSTYPE_OFFSET, SEEK_SET);
  	read (fd, buff, FAT_SYSTEM_SYSTYPE_SIZE);
  	if (strcmp(buff, FAT_SYSYTEM_NAME) == 0) {
      free(buff);
  		return 1;
  	}
    free(buff);
  	return 0;
}


/***********************************************
*
* @Purpose: Reads the FAT16 filesystem metadata information from the volume file
* @Parameters: int fd, file descriptor of the volume read
* @Return:  FatSystem structure containing the information about an FAT16 filesystem
*
************************************************/
FatSystem FatSystem_readSystem(int fd){
  FatSystem fat_system;
  // The system is always FAT16
  strcpy(fat_system.system_type, "FAT16");
  // Reserved Factors
  lseek(fd, FAT_SYSTEM_RESERVED_SECTORS_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BPB_RsvdSecCnt), FAT_SYSTEM_RESERVED_SECTORS_SIZE);
  // printf("Buffer reserved sectors: %d\n", fat_system.num_reserved_sectors);
  // Name
  lseek(fd, FAT_SYSTEM_NAME_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BS_OEMName),FAT_SYSTEM_NAME_SIZE);
  // label
  lseek(fd, FAT_SYSTEM_LABEL_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BS_VolLab), FAT_SYSTEM_LABEL_SIZE);
  // Sectors per cluster
  lseek(fd, FAT_SYSTEM_SECTOR_CLUSTER_OFFSET, SEEK_SET);
	read(fd, &fat_system.BPB_SecPerClus,FAT_SYSTEM_SECTOR_CLUSTER_SIZE);
  // Number of fats
  lseek(fd, FAT_SYSTEM_NUM_FATS_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BPB_NumFATs),FAT_SYSTEM_NUM_FATS_SIZE);
  // Number of sectors
  lseek(fd, FAT_SYSTEM_SECTORS_PER_FAT_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BPB_FATSz16),FAT_SYSTEM_SECTORS_PER_FAT_SIZE);
  // Max root entries
	lseek(fd, FAT_SYSTEM_MAX_ROOT_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BPB_RootEntCnt),FAT_SYSTEM_MAX_ROOT_SIZE);
  // Plain size
	lseek(fd, FAT_SYSTEM_SIZE_OFFSET, SEEK_SET);
	read(fd, &(fat_system.BPB_BytsPerSec),FAT_SYSTEM_SIZE_SIZE);
  return fat_system;
}


/***********************************************
*
* @Purpose: Prints in screen the inode data
* @Parameters: ExtInodeData inode. Inode structure with inode metadata
* @Return: -
*
************************************************/
void FatSystem_displayFatInfo (FatSystem fat_system) {
	printf("\n- - - - - -  Filesystem Information  - - - - - -\n\n");
  printf("\tFilesystem: FAT16\n");
  printf("\tSystem Name: %s\n",fat_system.BS_OEMName);
  printf("\tSize: %hu\n",fat_system.BPB_BytsPerSec );
  printf("\tSectors Per Cluster: %u\n",fat_system.BPB_SecPerClus);
  printf("\tReserved sectors: %hu\n", fat_system.BPB_RsvdSecCnt);
	printf("\tNumber of FATs: %u\n" , fat_system.BPB_NumFATs);
  printf("\tMaxRootEntries: %hu\n", fat_system.BPB_RootEntCnt);
	printf("\tSectors per FAT: %hu\n", fat_system.BPB_FATSz16);
  printf("\tLabel: %s\n", fat_system.BS_VolLab);
}


/***********************************************
*
* @Purpose: Calculates the address position of the root directory entry
* @Parameters: FatSystem fat_system, structure containing the information about the FAY16 filesystem
* @Return: returns the numeric address position where the first directory entry is located
*
************************************************/
int FatSystem_calculateRootDirectory(FatSystem fat_system){
	unsigned int root_dir_sectors = 0; // in the first time, it is 0
	// Formula to compute the root directory from page 13 of the manual
	unsigned int first_data_sector = fat_system.BPB_RsvdSecCnt + (fat_system.BPB_NumFATs * fat_system.BPB_FATSz16) + root_dir_sectors;
	// sector number * bytes per sector
	first_data_sector = first_data_sector * fat_system.BPB_BytsPerSec;

	//printf("root_dir_sectors: %d\n", root_dir_sectors);
	//printf("tFirst data sector: %d\n%d\n%d\n%d\n",first_data_sector, fat_system.BPB_RsvdSecCnt, fat_system.BPB_NumFATs, fat_system.BPB_FATSz16);
	return first_data_sector;
}


/***********************************************
*
* @Purpose: Calculates the address position of the next folder directory entry (formulas from the page 13 of the manual)
* @Parameters: FatSystem fat_system, structure containing the information about the FAY16 filesystem
*              FatDirEntry directory_entry, directory entry of current directory entry
* @Return: returns the numeric address position where the first directory entry is located
*
************************************************/
int FatSystem_calculateFirstSectorOfCluster(FatDirEntry directory_entry, FatSystem fat_system){
		//RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
		//FirstDataSector = BPB_ResvdSecCnt + (BPB_NumFATs * FATSz) + RootDirSectors;
		//FirstSectorofCluster = ((N – 2) * BPB_SecPerClus) + FirstDataSector;
		unsigned int root_dir_sectors = ((fat_system.BPB_RootEntCnt * 32) + (fat_system.BPB_BytsPerSec - 1)) / fat_system.BPB_BytsPerSec;
		unsigned int first_data_sector = fat_system.BPB_RsvdSecCnt + (fat_system.BPB_NumFATs * fat_system.BPB_FATSz16) + root_dir_sectors;
		unsigned int first_sectorof_cluster = (directory_entry.DIR_FstClusLO - 2) * fat_system.BPB_SecPerClus + first_data_sector;
		// sector number * bytes per sector
		first_sectorof_cluster = first_sectorof_cluster * fat_system.BPB_BytsPerSec;
		//printf("\n\nFirst sector of cluster = %d\n\n", first_sectorof_cluster);
		return first_sectorof_cluster;
}


/***********************************************
*
* @Purpose: Recursive function that finds looks for a file in  a FAT16 filesystem
* @Parameters: FatSystem fat_system, structure containing the information about the FAY16 filesystem
*              char *file, name of the file to be found in the filesystem volume file
*							 int volume_fd, filde descriptor for the volume filesystem file
*              unsigned int initial_address, address where the function starts to look for directory entries. In the first  call, it must be the address of the root directory
* @Return: returns the numeric address position where the first directory entry is located
*
************************************************/
void FatSystem_findFile(char *file, int volume_fd, unsigned int initial_address, FatSystem fat_system){
	unsigned int entry_pointer = initial_address;
	FatDirEntry directory_entry;
	char long_name[50];
	//printf("File to be found: %s\n", file);
	// No need to iterate recursively if the file has been found already
	if(fat_isFound == 1) return;

	// Iterating through all the directory entries
	for(int i = 0; i < fat_system.BPB_RootEntCnt; i++ ){
		lseek (volume_fd, entry_pointer, SEEK_SET);
		read(volume_fd, &directory_entry, FAT_SYSTEM_DIR_ENTRY_SIZE);
		//stoping the loop if we reach 0x00, which is the end of the directory entries
		if (directory_entry.DIR_Name[0] == 0x00) {
			break;
		}
		// Parsing the name (in FAT16 the names have a weird format)
		//printf("Original file name: %s\n", file);
		//printf("Before parsing: %s, size: %d, type: %d\n", directory_entry.DIR_Name, directory_entry.DIR_FileSize, directory_entry.DIR_Attr);
		FatSystem_parseFileName(&directory_entry, entry_pointer, volume_fd, long_name);
		//printf("Parsed name: %s, size: %d, type: %d\n\n\n", directory_entry.DIR_Name, directory_entry.DIR_FileSize, directory_entry.DIR_Attr);
		//printf("(Long name, Filename, Dir_name, uppercase_name): (%s, %s,%s,%s)\n", long_name, file, directory_entry.DIR_Name, uppercase_name);
		if((strcmp(directory_entry.DIR_Name,uppercase_name) == 0 || strcmp(long_name, file) == 0) && FatSystem_isFile(directory_entry) == 1){
			if(fat_isDelete == 1){
				FatSystem_deleteEntry(entry_pointer, volume_fd, file);
			}else{
				printf("File: %s found! It has %d bytes\n", file, directory_entry.DIR_FileSize );
			}
			fat_isFound = 1;
		}
		// If it is a valid folder, recusively call the function
		if(FatSystem_isValidFolder(directory_entry) == 1){
			initial_address = FatSystem_calculateFirstSectorOfCluster(directory_entry, fat_system);
			//printf("\nDepth\n\n");
			FatSystem_findFile(file, volume_fd, initial_address,fat_system);
		}
		// Pointing to the next directory entry
		entry_pointer = entry_pointer + FAT_SYSTEM_DIR_ENTRY_SIZE;
	}
	//printf("\nBack\n\n");
}


/***********************************************
*
* @Purpose: Parses the file name from the directory entry to convert it to normal format and not FAT16 format
* @Parameters: FatSystem *directory_entry, structure containing the directory entry with the file name in FAT16 format
* @Return:-
*
************************************************/
void FatSystem_parseFileName(FatDirEntry *directory_entry, unsigned int entry_pointer, int volume_fd, char long_name[50]){
	int i = 0, j = 0, is_first = 1;
	// Checking that there is no error with the file name
	if(directory_entry->DIR_Name[0] == 0x20){
		printf("Invalid name: %s\n", directory_entry->DIR_Name);
		return;
	}
	// Removing the white spaces from the string and putting a '.' if necessary
	while (directory_entry->DIR_Name[i] && i < FAT_SYSTEM_DIR_NAME_SIZE){
		if (directory_entry->DIR_Name[i] != ' ' ){
			directory_entry->DIR_Name[j++] = directory_entry->DIR_Name[i];
		}else if (is_first == 1 && FatSystem_noMoreChars(directory_entry->DIR_Name, i)){
			directory_entry->DIR_Name[j++] = '.';
			is_first = 0;
		}
		i++;
	}
	directory_entry->DIR_Name[j] = '\0';


	//TODO: Check for long names
	/*
	if(directory_entry->DIR_Attr == ATTR_LONG_NAME){

	}

	*/
	int buff[] = {1,3,5,7,9,14,16,18,20,22,24,28,30};
	char buff2[32];
	lseek (volume_fd, entry_pointer - FAT_SYSTEM_DIR_ENTRY_SIZE, SEEK_SET);
	read(volume_fd, &buff2, sizeof(buff2));

	//printf("Buffer of the long name: ");
	//for(int i = 0; i < 32; i++){
	//	printf("%c", buff2[i]);
	//}
	//printf("\n");

	bzero(long_name, sizeof(char)*50);
	for(int i = 0; i < 13; i++){
		long_name[i] = buff2[buff[i]];
	}
	long_name[13] = '\0';
	//printf("Long  in parsing name: %s\n", long_name);

}


/***********************************************
*
* @Purpose: Function used to check if there are more characters different from ' ' in a name starting from the position index of the string
* @Parameters: char *name, string of character to be checked
*              int index, value indicating from which position the parameter name is checked
* @Return: returns 1 if the name does not have more characters apart from ' ', and 0 otherwise
*
************************************************/
int FatSystem_noMoreChars(char *name, int index){
	for(int i = index; i < FAT_SYSTEM_DIR_NAME_SIZE; i++){
		if(name[i] != ' '){
			return 1;
		}
	}
	return 0;
}


/***********************************************
*
* @Purpose: Function that returns 1 if a directory entry is  a file, 0 otherwise
* @Parameters: FatDirEntry directory_entry, directory entry whose type is checked
* @Return:  returns 1 if a directory entry is  a file, 0 otherwise
*
************************************************/
int FatSystem_isFile(FatDirEntry directory_entry){
	return directory_entry.DIR_Attr == FAT_SYSTEM_DIR_ENTRY_FILE;
}


/***********************************************
*
* @Purpose: Function that returns 1 if a directory entry is  a folder, 0 otherwise
* @Parameters: FatDirEntry directory_entry, directory entry whose type is checked
* @Return:  returns 1 if a directory entry is  a folder, 0 otherwise
*
************************************************/
int FatSystem_isFolder(FatDirEntry directory_entry){
	return directory_entry.DIR_Attr == FAT_SYSTEM_DIR_ENTRY_FOLDER;
}


/***********************************************
*
* @Purpose: Checks if the file has been found or not
* @Parameters: -
*
* @Return:  returns the value of the global variable isFound
*
************************************************/
int FatSystem_isFound(){
	return fat_isFound;
}


/***********************************************
*
* @Purpose: Checks if the directory entry is and if it is different than .. and .
* @Parameters: -
*
* @Return:  returns 1 if it is valid, 0 otherwise
*
************************************************/
int FatSystem_isValidFolder(FatDirEntry directory_entry){
	return (strcmp(directory_entry.DIR_Name,"..") != 0 && strcmp(directory_entry.DIR_Name,".") != 0 && FatSystem_isFolder(directory_entry) == 1);
}


/***********************************************
*
* @Purpose: Deletes the directory entry in the file system
* @Parameters: unsigned int dir_entry_pos: Position of the filesystem to be deleted
*              int volume_fd: file descriptor  of the filesystem
*
* @Return:  returns 1 if it is valid, 0 otherwise
*
************************************************/
void FatSystem_deleteEntry(unsigned int dir_entry_pos, int volume_fd, char *name){
	FatDirEntry directory_entry;

	lseek (volume_fd, dir_entry_pos, SEEK_SET);
	read(volume_fd, &directory_entry, FAT_SYSTEM_DIR_ENTRY_SIZE);
	//printf("Dir to be deleted: %s", directory_entry.DIR_Name);
	// Deleting all the directory entry
	bzero(&directory_entry, sizeof(FatDirEntry));
	// Setting the first byte to 0xE5 which tells that this directory entry is free
	directory_entry.DIR_Name[0] = 0xE5;
	// Writting the empty directory entry
	lseek (volume_fd, dir_entry_pos, SEEK_SET);
	write(volume_fd, &directory_entry, FAT_SYSTEM_DIR_ENTRY_SIZE);
	//read(volume_fd, &directory_entry, FAT_SYSTEM_DIR_ENTRY_SIZE);
	//printf("Dir to be deleted after memset: %s, Size: %d", directory_entry.DIR_Name, directory_entry.DIR_FileSize);

	printf("File %s deleted in the filesystem\n", name);
}
