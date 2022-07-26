/***********************************************
*
* @Purpose: Module to read and write data from a FAT16 file system volume
* @Author: Óscar Cubeles Ollé
* @Creation Date: March 2022
*
************************************************/
#ifndef FATSYSTEM_H
    #define FATSYSTEM_H

    #define FAT_SYSYTEM_NAME "FAT16   "
    // System type size and offset
    #define FAT_SYSTEM_SYSTYPE_SIZE 8
    #define FAT_SYSTEM_SYSTYPE_OFFSET 54
    // System name size and offset
    #define FAT_SYSTEM_NAME_SIZE 8
    #define FAT_SYSTEM_NAME_OFFSET 3
    // System reserved sectors size and offset
    #define FAT_SYSTEM_RESERVED_SECTORS_OFFSET 14
    #define FAT_SYSTEM_RESERVED_SECTORS_SIZE 2
    // System label size and offset
    #define FAT_SYSTEM_LABEL_SIZE 11
    #define FAT_SYSTEM_LABEL_OFFSET 43
    // System sectors per cluster size and offset
    #define FAT_SYSTEM_SECTOR_CLUSTER_OFFSET 13
    #define FAT_SYSTEM_SECTOR_CLUSTER_SIZE 1
    // System number of fats size and offset
    #define FAT_SYSTEM_NUM_FATS_OFFSET 16
    #define FAT_SYSTEM_NUM_FATS_SIZE 1
    // System sectors per fat size and offset
    #define FAT_SYSTEM_SECTORS_PER_FAT_OFFSET 22
    #define FAT_SYSTEM_SECTORS_PER_FAT_SIZE 2
    // System max root size and offset
    #define FAT_SYSTEM_MAX_ROOT_OFFSET 17
    #define FAT_SYSTEM_MAX_ROOT_SIZE 2
    // System size size and offset
    #define FAT_SYSTEM_SIZE_OFFSET 11
    #define FAT_SYSTEM_SIZE_SIZE 2

    #define FAT_SYSTEM_DIR_ENTRY_SIZE 32
    #define FAT_SYSTEM_DIR_NAME_SIZE 11

    #define FAT_SYSTEM_DIR_ENTRY_FILE 0x20
    #define FAT_SYSTEM_DIR_ENTRY_FOLDER 0x10

    // First one as unisgned short b.c. is the smalles block of 2 bytes
    typedef struct FatSystem {
      unsigned short BPB_RsvdSecCnt;          // Number of reserved sectors in the Reserved region of the volume starting at the first sector of the volume
      char BS_OEMName [8];                    // It is only a name string
      char BS_VolLab [11];                    // Volume label. This field matches the 11-byte volume label recorded in the root directory.
      unsigned char BPB_SecPerClus;           // Number of sectors per allocation unit. This value must be a power of 2 that is greater than 0.
      unsigned char BPB_NumFATs;              // The count of FAT data structures on the volume
      unsigned short BPB_FATSz16;             // This field is the FAT12/FAT16 16-bit count of sectors occupied by ONE FAT
      unsigned short BPB_RootEntCnt;          // For FAT12 and FAT16 volumes, this field contains the count of 32-byte directory entries in the root directory.
      unsigned short BPB_BytsPerSec;          // Count of bytes per sector
      char system_type[6];
    } FatSystem;

    typedef struct FatDirEntry{
      char DIR_Name[11];                      // Short name
      char DIR_Attr;                          // File attributes
      char DIR_NTRes;                         // Reserved for use by Windows NT
      char DIR_CrtTimeTenth;                  // Millisecond stamp at file creation time
      unsigned short DIR_CrtTime;             // Time file was created
      unsigned short DIR_CrtDate;             // Date file was created.
      unsigned short DIR_LstAccDate;          // Last access date.
      unsigned short DIR_FstClusHI;           // High word of this entry’s first cluster number (always 0 for a FAT12 or FAT16 volume).
      unsigned short DIR_WrtTime;             // Time of last write
      unsigned short DIR_WrtDate;             // Date of last write
      unsigned short DIR_FstClusLO;           // Low word of this entry’s first cluster number.
      unsigned int DIR_FileSize;              // 32-bit DWORD holding this file’s size in bytes
    }FatDirEntry;


    int FatSystem_isFatSystem(int fd);
    FatSystem FatSystem_readSystem(int fd);
    void FatSystem_displayFatInfo (FatSystem fat_system);
    int FatSystem_getOperationNumber(char *operation);
    void FatSystem_executeOperation(char * operation, char* file, int volume_fd);
    int FatSystem_calculateRootDirectory(FatSystem fat_system);
    void FatSystem_findFile(char *file, int volume_fd, unsigned int initial_address, FatSystem fat_system);
    void FatSystem_parseFileName(FatDirEntry *directory_entry, unsigned int entry_pointer, int volume_fd, char long_name[50]);
    int FatSystem_noMoreChars(char *name, int index);
    int FatSystem_isFile(FatDirEntry directory_entry);
    int FatSystem_isFolder(FatDirEntry directory_entry);
    int FatSystem_isValidFolder(FatDirEntry directory_entry);
    void FatSystem_deleteEntry(unsigned int dir_entry_pos, int volume_fd, char *name);
    void FatSystem_fileToUpper(char *file);
#endif
