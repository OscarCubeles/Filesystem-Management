/***********************************************
*
* @Purpose: Module to read and write data from an Ext2 file system volume
* @Author: Óscar Cubeles Ollé
* @Creation Date: March 2022
*
************************************************/
#ifndef EXSYSTEM_H
    #define EXSYSTEM_H

    #define EXT_SYSTEM_SUPERBLOCK_OFFSET 1024

    // Magic word constants
    #define EXT_SYSTEM_MAGIC_WORD 0xEF53
    #define EXT_SYSTEM_MAGIC_WORD_SIZE 2
    #define EXT_SYSTEM_MAGIC_WORD_OFFSET 56

    // Inode constants
    #define EXT_SYSTEM_INODE_OFFSET 88
    #define EXT_SYSTEM_INODE_SIZE 2
    #define EXT_SYSTEM_INODE_COUNT_OFFSET 0
    #define EXT_SYSTEM_INODE_COUNT_SIZE 4
    #define EXT_SYSTEM_INODE_FIRST_OFFSET 84
    #define EXT_SYSTEM_INODE_FIRST_SIZE 4
    #define EXT_SYSTEM_INODE_GROUP_OFFSET 40
    #define EXT_SYSTEM_INODE_GROUP_SIZE 4
    #define EXT_SYSTEM_INODE_FREE_OFFSET 16
    #define EXT_SYSTEM_INODE_FREE_SIZE 4

    // Block constants
    #define EXT_SYSTEM_BLOCK_SIZE_OFFSET 24
    #define EXT_SYSTEM_BLOCK_SIZE_SIZE 4
    #define EXT_SYSTEM_BLOCK_RESERVED_OFFSET 8
    #define EXT_SYSTEM_BLOCK_RESERVED_SIZE 4
    #define EXT_SYSTEM_BLOCK_FREE_OFFSET 12
    #define EXT_SYSTEM_BLOCK_FREE_SIZE 4
    #define EXT_SYSTEM_BLOCK_N_TOTALBLOCKS_OFFSET 4
    #define EXT_SYSTEM_BLOCK_N_TOTALBLOCKS_SIZE 4
    #define EXT_SYSTEM_BLOCK_FIRST_OFFSET 20
    #define EXT_SYSTEM_BLOCK_FIRST_SIZE 4
    #define EXT_SYSTEM_BLOCK_GROUP_OFFSET 32
    #define EXT_SYSTEM_BLOCK_GROUP_SIZE 4
    #define EXT_SYSTEM_BLOCK_FRAGS_OFFSET 36
    #define EXT_SYSTEM_BLOCK_FRAGS_SIZE 4

    // Volume constants
    #define EXT_SYSTEM_VOLUME_NAME_OFFSET 120
    #define EXT_SYSTEM_VOLUME_NAME_RBYTES 16
    #define EXT_SYSTEM_VOLUME_CHECKED_OFFSET 64
    #define EXT_SYSTEM_VOLUME_CHECKED_RBYTES 4
    #define EXT_SYSTEM_VOLUME_MOUNT_OFFSET 44
    #define EXT_SYSTEM_VOLUME_MOUNT_RBYTES 4
    #define EXT_SYSTEM_VOLUME_WRITE_OFFSET 48
    #define EXT_SYSTEM_VOLUME_WRITE_RBYTES 4

    // Inode file types
    #define EXT2_FT_UNKNOWN 0
    #define EXT2_FT_REG_FILE 1
    #define EXT2_FT_DIR 2
    #define EXT2_FT_INIT -1

    typedef struct Inode{
    	unsigned short s_inode_size;         // 16bit value indicating the size of the inode structure
    	unsigned int s_inodes_count;         // 32bit value indicating the total number of inodes, both used and free, in the file system
    	unsigned int s_first_ino;            // 32bit value used as index to the first inode useable for standard files
    	unsigned int s_inodes_per_group;     // 32bit value indicating the total number of inodes per group.
                                           // This is also used to determine the size of the inode bitmap of each block group
    	unsigned int s_free_inodes_count;    // 32bit value indicating the total number of free inodes of all the block groups
    } ExtInodeData;

    typedef struct Block{
    	unsigned int s_log_block_size;              // The block size is computed using this 32bit value as the number of bits to shift left the value 1024
    	unsigned int s_r_blocks_count;              // 32bit value indicating the total number of blocks reserved for the usage of the super user
    	unsigned int s_free_blocks_count;           // 32bit value indicating the total number of free blocks, including the number of reserved blocks
    	unsigned int s_blocks_count;                // 32bit value indicating the total number of blocks in the system including all used, free and reserved
    	unsigned int s_first_data_block;            // 32bit value identifying the first data block, in other word the id of the block containing the superblock structure.
    	unsigned int s_blocks_per_group;            // 32bit value indicating the total number of blocks per group.
    	unsigned int s_frags_per_group;             // 32bit value indicating the total number of fragments per group.
    } ExtBlockData ;

    typedef struct Volume {
    	unsigned int s_lastcheck;                   // Unix time, as defined by POSIX, of the last file system check.
    	unsigned int s_mtime;                       // Unix time, as defined by POSIX, of the last time the file system was mounted.
    	unsigned int s_wtime;                       // Unix time, as defined by POSIX, of the last write access to the file system.
      char s_volume_name[16];                     // 16 bytes volume name, mostly unusued. A valid volume name would consist of only ISO-Latin-1 characters and be 0 terminated
    } ExtVolumeData;



    typedef struct BlockGroupDescriptorTable{
      unsigned int bg_block_bitmap;             // 32bit block id of the first block of the “block bitmap” for the group represented
                                                // The actual block bitmap is located within its own allocated blocks starting at the block ID specified by this value
      unsigned int bg_inode_bitmap;             // 32bit block id of the first block of the “inode bitmap” for the group represented.
      unsigned int bg_inode_table;              // 32bit block id of the first block of the “inode table” for the group represented.
      unsigned short bg_free_blocks_count;      // 16bit value indicating the total number of free blocks for the represented group.
      unsigned short bg_free_inodes_count;      // 16bit value indicating the total number of free inodes for the represented group.
      unsigned short bg_used_dirs_count;        // 16bit value indicating the number of inodes allocated to directories for the represented group
      unsigned short bg_pad;                    // 16bit value used for padding the structure on a 32bit boundary.
    }BlockGroupDescriptorTable;


    typedef struct InodeTableEntry{
      unsigned short i_mode;                     // 16bit value used to indicate the format of the described file and the access rights
      unsigned short i_uid;                      // 16bit user id associated with the file
      unsigned int i_size;                       // In revision 0, (signed) 32bit value indicating the size of the file in bytes
      unsigned int i_atime;                      // 32bit value representing the number of seconds since january 1st 1970 of the last time this inode was accessed
      unsigned int i_ctime;                      // 32bit value representing the number of seconds since january 1st 1970, of when the inode was created
      unsigned int i_mtime;                       // 32bit value representing the number of seconds since january 1st 1970, of the last time this inode was modified.
      unsigned int i_dtime;                       // 32bit value representing the number of seconds since january 1st 1970, of when the inode was deleted.
      unsigned short i_gid;                       // 16bit value of the POSIX group having access to this file
      unsigned short i_links_count;               // 16bit value indicating how many times this particular inode is linked (referred to). Most files will have a link count of 1.
      unsigned int i_blocks;                      // 32-bit value representing the total number of 512-bytes blocks reserved to contain the data of this inode,
      unsigned int i_flags;                       // 32bit value indicating how the ext2 implementation should behave when accessing the data for this inode.
      unsigned int i_osd1;                        // 32bit OS dependant value
      unsigned int i_block[15];                   // 15 x 32bit block numbers pointing to the blocks containing the data for this inode
      unsigned int i_generation;                  // 32bit value used to indicate the file version (used by NFS).
      unsigned int i_file_acl;                    // 32bit value indicating the block number containing the extended attributes. In revision 0 this value is always 0
      unsigned int i_dir_acl;                     // In revision 0 this 32bit value is always 0.
      unsigned int i_faddr;                       // 32bit value indicating the location of the file fragmen
      unsigned int i_osd2[3];                     // 96bit OS dependant structure.
    }InodeTableEntry;


    typedef struct DirEntry{
      char name_len;                               // 8bit unsigned value indicating how many bytes of character data are contained in the name
      char file_type;                              // 8bit unsigned value used to indicate file type.
    	unsigned int inode;                          // 32bit inode number of the file entry. A value of 0 indicate that the entry is not used.
    	unsigned short rec_len;                      // 16bit unsigned displacement to the next directory entry from the start of the current directory entry
    	char name[256];                              // Name of the entry. The ISO-Latin-1 character set is expected in most system. The name must be no longer than 255 bytes after encoding
    }DirEntry;





    int Ex2System_isExt (int fd);
    ExtInodeData Ex2System_readInode (int fd);
    ExtBlockData Ex2System_readBlock (int fd);
    ExtVolumeData Ex2System_readVolume(int fd);
    void EX2System_printVolume(ExtVolumeData volume);
    void EX2System_printBlock(ExtBlockData block);
    void EX2System_printInode(ExtInodeData inode);
    void EX2SYSTEM_executeOperation(char * operation, char* file, int volume_fd);
    void EX2System_findFile(char* filename, int volume_fd, ExtBlockData block, ExtInodeData inode, unsigned int root_inode);
    void Ext2System_fillBlockGroupDescriptorTable(int fd, unsigned int block_size, 	BlockGroupDescriptorTable *bg_descriptor_table );
    InodeTableEntry Ext2System_findAndGetInode(unsigned int first_inode, BlockGroupDescriptorTable *bg_descriptor_table, ExtBlockData block, ExtInodeData inode, int volume_fd );
    DirEntry Ext2System_readDirEntry(int volume_fd, unsigned short *len, int dir_entry_block_position);
    int Ext2System_isDirectory(char *filename, int file_type);
    int Ext2System_isFound();
    void Ext2System_deleteEntry(unsigned short ptr_next_inode_name, unsigned int dir_entry_block_position, unsigned short prev_dir_len, DirEntry directory_entry, int volume_fd);



#endif
