#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

// Sector offsets for the main sections of FAT12
#define BOOT_SECTOR_OFFSET 0
#define FAT1_OFFSET 1
#define FAT2_OFFSET 10
#define ROOT_DIRECTORY_OFFSET 19
#define DATA_AREA_OFFSET 33

// FAT Meanings
#define UNUSED 0x00
#define RESERVED_CLUSTER_BEGIN 0xFF0
#define RESERVED_CLUSTER_END 0xFF6
#define BAD_CLUSTER 0xFF7
#define LAST_CLUSTER_BEGIN 0xFF8
#define LAST_CLUSTER_END 0xFFF

// Attribtues
#define READ_ONLY 0x01
#define HIDDEN 0x02
#define SYSTEM 0x04
#define VOLUME_LABEL 0x08
#define SUBDIRECTORY 0x10
#define ARCHIVE 0x20

// Random Sizes
#define DIR_ENTRY_SIZE 32
#define MAX_FILENAME_LENGTH 8
#define MAX_EXT_LENGTH 3

// File Name Codes
#define DELETED 0xE5
#define REMAINING_FREE 0x00

/**
 * Struct representing the boot sector
 */
typedef struct {
	uint8_t ignore_1[11];
	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster;
	uint16_t numReservedSectors;
	uint8_t numFATCopies;
	uint16_t maxRootDirEntries;
	uint16_t numSectors;
	uint8_t ignore_2[1];
	uint16_t sectorsPerFAT;
	uint16_t sectorsPerTrack;
	uint16_t numHeads;
	uint8_t ignore_3[4];
	uint32_t sectorCountFAT32;
	uint8_t ignore_4[2];
	uint8_t bootSig;
	uint32_t volId;
	unsigned char volName[12];			// String, Extra byte for null-terminator
	unsigned char fileSystemType[9];	// String, Extra byte for null-terminator
	uint8_t ignore_5[450];
} boot_sector;
 
/**
 * Struct representing a directory entry
 */
typedef struct {
	unsigned char filename[9];			// String, Extra byte for null-terminator
	unsigned char extension[4];			// String, Extra byte for null-terminator
	uint8_t attributes;
	uint16_t reserved;
	uint16_t creationTime;
	uint16_t creationDate;
	uint16_t lastAccess;
	uint16_t ignore;
	uint16_t lastWriteTime;
	uint16_t lastWriteDate;
	uint16_t firstLogicalCluster;
	uint32_t fileSize;
} dir_entry;

/**
 * Struct containing the FAT table information
 */
typedef struct {
	int numEntries;
	uint16_t *fat1;
	uint16_t *fat2;
} fat_tables;

/**
 * Initializes the FAT abstraction with the given disk image.
 *
 * @param diskImage The pointer to an opened file that is a FAT12 disk image.
 */
void initialize(FILE *diskImage);

/**
 * Destroys the FAT abstraction, freeing any memory used internally.
 */
void destroy();

/**
 * Gets the boot sector of the FAT File System.
 *
 * @return The boot sector of the FAT File System
 */
boot_sector *getBootSector();

/**
* Gets the FAT Tables for the FAT File System.
*
* @return The FAT Tables for the FAT File System
*/
fat_tables *getFATTables();

/**
 * Gets the current directory of the FAT File System.
 *
 * @return The current directory as an array of directory entries.
 */
dir_entry *getCurrentDirectory();

/**
 * Gets the maximum size of the current directory.
 *
 * @return The maximum size of the current directory.
 */
int getCurrentDirectoryMaxSize();

/**
 * Gets the file specified by the given cluster.
 *
 * @param cluster The starting logical cluster of the file
 * @param size The size of the file (in bytes)
 * @return An array of bytes representing the file.
 */
unsigned char *getFileFromSector(uint16_t cluster, int size);

/**
 * Changes to the directory specified by the given logical cluster.
 *
 * @param cluster The starting logical cluster of the directory.
 * @return The maximum number of entries that can appear in this directory.
 */
int changeToDirectory(uint16_t cluster);

/**
* Changes the current directory to the parent of the current.
*
* @return The maximum size of the new current directory.
*/
int changeToParentDirectory();

/**
 * Sets the name of the file at the specified index of the current list of directory entries.
 *
 * @param idx The index of the listing in the current directory
 * @param filename The name of the file (max 8 characters)
 * @param fileExt The extension of the file (max 3 characters)
 */
void setFilename(int idx, const char* filename, const char* fileExt);

/**
 * Moves a file at the specified index of the current directory to the directory at the destination sector.
 *
 * @param idx The index of the listing in the current direcory
 * @param destSector The beginning sector of the target directory
 */
bool moveFile(int idx, uint16_t destSector);