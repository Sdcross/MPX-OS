#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "fat.h"

// Function prototypes
void _loadBootSectorInfo();
void _loadFATTables();
void _loadRootDirectroy();
dir_entry *_loadSectorAsDirectoryEntries(uint16_t sector);
void _readDirectoryEntry(dir_entry *dir);
long _getDiskOffsetForDirEntry(int idx);

void _saveFATTables();
void _saveDirEntry(dir_entry *dir);
void _refreshDirectory();
void _prepNewDirSector(uint16_t sector);
int _getFirstFreeIndexInSector(dir_entry *dirs);
int _getFirstFreeIndexInDirs(dir_entry *dirs, int maxSize);
uint16_t _getFirstOpenSector();

// The disk image to load from
FILE *_DiskImage;

// File System Information
boot_sector _BootSector;
fat_tables _FATTables;

// The current directory
dir_entry *_CurrentDirectory;
int _CurrDirSize = 0;
bool _isCurrentRoot = false;

/**
 * Initializes the FAT abstraction with the given disk image.
 *
 * @param diskImage The pointer to an opened file that is a FAT12 disk image.
 */
void initialize(FILE *diskImage) {
	_DiskImage = diskImage;

	_loadBootSectorInfo();
	_loadFATTables();
	_loadRootDirectroy();
}

/** 
 * Destroys the FAT abstraction, freeing any memory used internally.
 */
void destroy() {
	free(_FATTables.fat1);
	free(_FATTables.fat2);
	free(_CurrentDirectory);

	_DiskImage = NULL;
}

/** 
 * Gets the boot sector of the FAT File System.
 *
 * @return The boot sector of the FAT File System
 */
boot_sector *getBootSector() {
	return &_BootSector;
}

/**
 * Gets the FAT Tables for the FAT File System.
 *
 * @return The FAT Tables for the FAT File System
 */
fat_tables *getFATTables() {
	return &_FATTables;
}

/**
 * Gets the current directory of the FAT File System.
 *
 * @return The current directory as an array of directory entries.
 */
dir_entry *getCurrentDirectory() {
	return _CurrentDirectory;
}

/**
 * Gets the maximum size of the current directory.
 *
 * @return The maximum size of the current directory.
 */
int getCurrentDirectoryMaxSize() {
	return _CurrDirSize;
}

/**
 * Gets the file specified by the given sector.
 *
 * @param sector The starting logical sector of the file
 * @param size The size of the file (in bytes)
 * @return An array of bytes representing the file.
 */
unsigned char *getFileFromSector(uint16_t sector, int size) {
	unsigned char *file = malloc(sizeof(unsigned char) * size);
	unsigned char *buffer = malloc(sizeof(unsigned char) * _BootSector.bytesPerSector);

	int fileIdx = 0;
	int bytesRead = 0;
	uint16_t currCluster = sector;

	do {
		int bytesToRead = size - bytesRead > _BootSector.bytesPerSector ? _BootSector.bytesPerSector : size - bytesRead;

		fseek(_DiskImage, (DATA_AREA_OFFSET + currCluster - 2) * _BootSector.bytesPerSector, SEEK_SET);
		fread(buffer, 1, bytesToRead, _DiskImage);
		memcpy(file + (fileIdx++ * _BootSector.bytesPerSector), buffer, bytesToRead);

		currCluster = _FATTables.fat1[currCluster];
		bytesRead += bytesToRead;
	} while (currCluster != UNUSED && currCluster < RESERVED_CLUSTER_BEGIN);

	free(buffer);
	return file;
}

/**
 * Changes to the directory specified by the given logical sector.
 *
 * @param sector The starting logical sector of the directory.
 * @return The maximum number of entries that can appear in this directory.
 */
int changeToDirectory(uint16_t sector) {
	free(_CurrentDirectory);

	// Root directory
	if (sector == 0) {
		_loadRootDirectroy();
		return _CurrDirSize;
	}

	uint16_t currSector = sector;
	int numSectors = 0;
	int numDirEntriesPerSector =  _BootSector.bytesPerSector / DIR_ENTRY_SIZE;
	
	// Need to know how many sectors this covers to know how much memory to allocate
	do {
		currSector = _FATTables.fat1[currSector];
		numSectors++;
	} while (currSector != UNUSED && currSector < RESERVED_CLUSTER_BEGIN);

	_CurrentDirectory = malloc(sizeof(dir_entry) * numSectors * numDirEntriesPerSector);

	currSector = sector;

	for (int i = 0; i < numSectors; i++, currSector = _FATTables.fat1[currSector]) {
		dir_entry *dirs = _loadSectorAsDirectoryEntries(DATA_AREA_OFFSET + currSector - 2);

		for (int j = 0; j < numDirEntriesPerSector; j++) {
			memcpy(
				&_CurrentDirectory[(i * numDirEntriesPerSector) + j],
				&dirs[j],
				sizeof(dir_entry));
		}

		free(dirs);
	}

	_isCurrentRoot = false;
	_CurrDirSize = (_BootSector.bytesPerSector / DIR_ENTRY_SIZE) * numSectors;
	return _CurrDirSize;
}

/**
 * Changes the current directory to the parent of the current.
 *
 * @return The maximum size of the new current directory, or -1 if the directory didn't switch.
 */
int changeToParentDirectory() {
	if (!_isCurrentRoot) {
		int sector = _CurrentDirectory[1].firstLogicalCluster;

		if (sector < DATA_AREA_OFFSET) {
			sector = ROOT_DIRECTORY_OFFSET;
			_isCurrentRoot = true;
		} else {
			sector = DATA_AREA_OFFSET + sector - 2;
		}

		free(_CurrentDirectory);

		_CurrentDirectory = _loadSectorAsDirectoryEntries(sector);

		return _CurrDirSize;
	}

	return -1;
}

/**
* Sets the name of the file at the specified index of the current list of directory entries.
*
* @param idx The index of the listing in the current directory
* @param filename The name of the file (max 8 characters)
* @param fileExt The extension of the file (max 3 characters)
*/
void setFilename(int idx, const char *filename, const char *fileExt) {
	// Get the length of the name and extension
	int nameLen = strlen(filename);
	int extLen = strlen(fileExt);

	// Create a buffer to hold the 11 bytes to write
	char *newName = malloc(sizeof(char) * (MAX_FILENAME_LENGTH + MAX_EXT_LENGTH));

	// Copy the name and extension to the buffer
	memcpy(newName, filename, nameLen < MAX_FILENAME_LENGTH ? nameLen : MAX_FILENAME_LENGTH);
	memcpy(newName + MAX_FILENAME_LENGTH, fileExt, extLen < MAX_EXT_LENGTH ? extLen : MAX_EXT_LENGTH);

	// Pad the name with spaces
	for (int i = nameLen; i < MAX_FILENAME_LENGTH; i++) {
		newName[i] = ' ';
	}

	// Pad the extension with spaces
	for (int i = MAX_FILENAME_LENGTH + extLen; i < MAX_FILENAME_LENGTH + MAX_EXT_LENGTH; i++) {
		newName[i] = ' ';
	}

	// Convert all characters to uppercase
	for (int i = 0; i < MAX_FILENAME_LENGTH + MAX_EXT_LENGTH; i++) {
		newName[i] = toupper(newName[i]);
	}
	
	// Update the name currently stored in the directory entries
	memcpy(_CurrentDirectory[idx].filename, newName, MAX_FILENAME_LENGTH);
	memcpy(_CurrentDirectory[idx].extension, newName + MAX_FILENAME_LENGTH, MAX_EXT_LENGTH);

	// Seek to the correct location on the disc and write the new name
	fseek(_DiskImage, _getDiskOffsetForDirEntry(idx), SEEK_SET);
	fwrite(newName, sizeof(char), MAX_FILENAME_LENGTH + MAX_EXT_LENGTH, _DiskImage);
	fflush(_DiskImage);

	// Free the memory used for the buffer
	free(newName);
}

/**
* Moves a file at the specified index of the current directory to the directory at the destination sector.
*
* @param idx The index of the listing in the current direcory
* @param destSector The beginning sector of the target directory
*/
bool moveFile(int idx, uint16_t destSector) {
	dir_entry entry;

	fseek(_DiskImage, _getDiskOffsetForDirEntry(idx), SEEK_SET);
	_readDirectoryEntry(&entry);

	uint16_t currSector = destSector;

	// Transferring to root directory
	if (destSector == 0) {
		uint16_t currDir = _CurrentDirectory[0].firstLogicalCluster;

		free(_CurrentDirectory);
		_loadRootDirectroy();
		
		int freeIdx = _getFirstFreeIndexInDirs(_CurrentDirectory,_CurrDirSize);

		// No free entries
		if (freeIdx == -1) {
			changeToDirectory(currDir);
			return false;
		}

		fseek(_DiskImage, (ROOT_DIRECTORY_OFFSET * _BootSector.bytesPerSector) + (freeIdx * DIR_ENTRY_SIZE), SEEK_SET);

		bool remainingAreFree = _CurrentDirectory[freeIdx].filename[0] == REMAINING_FREE;
		_saveDirEntry(&entry);

		if (remainingAreFree && freeIdx < _CurrDirSize) {
			_CurrentDirectory[freeIdx + 1].filename[0] = REMAINING_FREE;
			_saveDirEntry(&_CurrentDirectory[freeIdx + 1]);
		}

		changeToDirectory(currDir);
	} else {



		int freeIdx = -1;
		while (freeIdx == -1) {
			dir_entry *dirs = _loadSectorAsDirectoryEntries(DATA_AREA_OFFSET + currSector - 2);

			freeIdx = _getFirstFreeIndexInSector(dirs);

			if (freeIdx == -1) {
				uint16_t nextSector = _FATTables.fat1[currSector];

				// Check and see if we need to create a new directory sector
				if (LAST_CLUSTER_BEGIN <= nextSector && nextSector <= LAST_CLUSTER_END) {
					uint16_t newSector = _getFirstOpenSector();

					if (newSector != -1) {
						_prepNewDirSector(newSector);

						_FATTables.fat1[currSector] = newSector;
						_FATTables.fat1[newSector] = LAST_CLUSTER_BEGIN;

						_FATTables.fat2[currSector] = newSector;
						_FATTables.fat2[newSector] = LAST_CLUSTER_BEGIN;

						_saveFATTables();
						_refreshDirectory();
					} else {
						return false;
					}
				}

				currSector = nextSector;
			} else {
				fseek(_DiskImage, ((currSector + DATA_AREA_OFFSET - 2) * _BootSector.bytesPerSector) + (freeIdx * DIR_ENTRY_SIZE), SEEK_SET);

				bool remainingAreFree = dirs[freeIdx].filename[0] == REMAINING_FREE;
				_saveDirEntry(&entry);

				if (remainingAreFree && freeIdx < _BootSector.bytesPerSector / DIR_ENTRY_SIZE) {
					dirs[freeIdx + 1].filename[0] = REMAINING_FREE;
					_saveDirEntry(&dirs[freeIdx + 1]);
				}
			}
		}
	}

	entry.filename[0] = DELETED;
	fseek(_DiskImage, _getDiskOffsetForDirEntry(idx), SEEK_SET);
	_saveDirEntry(&entry);
	_refreshDirectory();

	return true;
}

/**
 * Loads boot sector information.
 */
void _loadBootSectorInfo() {
	// Buffer for reading the informationm, must be big enough for the largest value.
	unsigned char buffer[461];

	// Ensure we are starting from the beginning of the file.
	fseek(_DiskImage, BOOT_SECTOR_OFFSET, SEEK_SET);

	// First ignored section
	fread(buffer, 1, 11, _DiskImage);
	memcpy(_BootSector.ignore_1, buffer, 11);

	// Bytes per Sector
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.bytesPerSector = buffer[1] << 8 | buffer[0];

	// Sectors per Cluster
	fread(buffer, 1, 1, _DiskImage);
	_BootSector.sectorsPerCluster = buffer[0];

	// Number of Reserved Sectors
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.numReservedSectors = buffer[1] << 8 | buffer[0];

	// Number of FAT Copies
	fread(buffer, 1, 1, _DiskImage);
	_BootSector.numFATCopies = buffer[0];

	// Max Number of Root Directory Entries
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.maxRootDirEntries = buffer[1] << 8 | buffer[0];

	// Total # of Sectors in the File System
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.numSectors = buffer[1] << 8 | buffer[0];

	// Second ignored section
	fread(buffer, 1, 1, _DiskImage);
	memcpy(_BootSector.ignore_2, buffer, 1);

	// Number of Sectors per FAT
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.sectorsPerFAT = buffer[1] << 8 | buffer[0];

	// Sectors per Track
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.sectorsPerTrack = buffer[1] << 8 | buffer[0];

	// Number of Heads
	fread(buffer, 1, 2, _DiskImage);
	_BootSector.numHeads = buffer[1] << 8 | buffer[0];

	// Third ignored section
	fread(buffer, 1, 4, _DiskImage);
	memcpy(_BootSector.ignore_3, buffer, 4);

	// Total Sector Count for FAT32
	fread(buffer, 1, 4, _DiskImage);
	_BootSector.sectorCountFAT32 = buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];

	// Fourth ignored section
	fread(buffer, 1, 2, _DiskImage);
	memcpy(_BootSector.ignore_4, buffer, 2);

	// Boot Signature
	fread(buffer, 1, 1, _DiskImage);
	_BootSector.bootSig = buffer[0];

	// Volume ID
	fread(buffer, 1, 4, _DiskImage);
	_BootSector.volId = buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];

	// Volume Label
	fread(buffer, 1, 11, _DiskImage);
	buffer[11] = '\0';
	strcpy(_BootSector.volName, buffer);

	// File System Type
	fread(buffer, 1, 8, _DiskImage);
	buffer[8] = '\0';
	strcpy(_BootSector.fileSystemType, buffer);

	// Fifth ignored section
	fread(buffer, 1, 450, _DiskImage);
	memcpy(_BootSector.ignore_5, buffer, 450);
}

/**
 * Loads the FAT table information
 */
void _loadFATTables() {
	// The size of the FAT Table in bytes
	int fatSizeBytes = _BootSector.sectorsPerFAT * _BootSector.bytesPerSector;

	// The number of entries in the FAT tables
	_FATTables.numEntries = fatSizeBytes * 8 / 12;

	// Allocate memory to hold the FAT tables
	_FATTables.fat1 = malloc(sizeof(uint16_t) * _FATTables.numEntries);
	_FATTables.fat2 = malloc(sizeof(uint16_t) * _FATTables.numEntries);

	// Read the raw FAT Table data
	uint8_t *fat1 = malloc(sizeof(uint8_t) * fatSizeBytes);
	uint8_t *fat2 = malloc(sizeof(uint8_t) * fatSizeBytes);

	fseek(_DiskImage, FAT1_OFFSET * _BootSector.bytesPerSector, SEEK_SET);
	fread(fat1, 1, fatSizeBytes, _DiskImage);
	fseek(_DiskImage, (long)(FAT2_OFFSET * _BootSector.bytesPerSector), SEEK_SET);
	fread(fat2, 1, fatSizeBytes, _DiskImage);

	// Convert by iterating 3 bytes at a time, creating 2 entries for each one
	for (int i = 0, j = 0, k = 0; i < fatSizeBytes; i += 3) {
		// FAT Table 1
		_FATTables.fat1[j++] = (fat1[i] + (fat1[i + 1] << 8)) & 0x0FFF;
		_FATTables.fat1[j++] = (fat1[i + 1] + (fat1[i + 2] << 8)) >> 4;

		// FAT Table 2
		_FATTables.fat2[k++] = (fat2[i] + (fat2[i + 1] << 8)) & 0x0FFF;
		_FATTables.fat2[k++] = (fat2[i + 1] + (fat2[i + 2] << 8)) >> 4;
	}

	free(fat1);
	free(fat2);
}

/**
 * Loads the root directory as the current directory.
 */
void _loadRootDirectroy() {
	int numDirEntriesPerSector = _BootSector.bytesPerSector / DIR_ENTRY_SIZE;
	int numRootDirSectors = _BootSector.maxRootDirEntries / (numDirEntriesPerSector);

	_CurrentDirectory = malloc(sizeof(dir_entry) * _BootSector.maxRootDirEntries);

	for (int i = 0; i < numRootDirSectors; i++) {
		dir_entry *dirs = _loadSectorAsDirectoryEntries(ROOT_DIRECTORY_OFFSET + i);

		for (int j = 0; j < numDirEntriesPerSector; j++) {
			memcpy(
				&_CurrentDirectory[(i * numDirEntriesPerSector) + j],
				&dirs[j],
				sizeof(dir_entry));
		}

		free(dirs);
	}

	_CurrDirSize = _BootSector.maxRootDirEntries;
	_isCurrentRoot = true;
}

/** 
 * Loads a sector as an array of (bytesPerSector / DIR_ENTRY_SIZE) directory entries.
 *
 * @param sector The sector to load from.
 * @return An array of directory entries.
 */
dir_entry *_loadSectorAsDirectoryEntries(uint16_t sector) {
	int numEntries = _BootSector.bytesPerSector / DIR_ENTRY_SIZE;
	dir_entry *directoryEntries = malloc(sizeof(dir_entry) * numEntries);

	fseek(_DiskImage, sector * _BootSector.bytesPerSector, SEEK_SET);

	for (int i = 0; i < numEntries; i++) {
		_readDirectoryEntry(&directoryEntries[i]);

		// Check if all remaining entries are free.
		if (directoryEntries[i].filename[0] == 0x00) {
			break;
		}
	}

	return directoryEntries;
}

/**
 * Reads a single directory entry into the passed entry.
 *
 * @param The directory entry to store the information in
 */
void _readDirectoryEntry(dir_entry *dir) {
	unsigned char buffer[9];

	// Read file name
	fread(buffer, 1, 8, _DiskImage);
	buffer[8] = '\0';
	strcpy(dir->filename, buffer);

	// Read file extenstion
	fread(buffer, 1, 3, _DiskImage);
	buffer[3] = '\0';
	strcpy(dir->extension, buffer);

	// Read attributes
	fread(buffer, 1, 1, _DiskImage);
	dir->attributes = buffer[0];

	// Read reserved section
	fread(buffer, 1, 2, _DiskImage);
	dir->reserved = buffer[1] << 8 | buffer[0];

	// Read creation time
	fread(buffer, 1, 2, _DiskImage);
	dir->creationTime = buffer[1] << 8 | buffer[0];

	// Read creation date
	fread(buffer, 1, 2, _DiskImage);
	dir->creationDate = buffer[1] << 8 | buffer[0];

	// Read last access date
	fread(buffer, 1, 2, _DiskImage);
	dir->lastAccess = buffer[1] << 8 | buffer[0];

	// Read ignore section
	fread(buffer, 1, 2, _DiskImage);
	dir->ignore = buffer[1] << 8 | buffer[0];

	// Read last write time
	fread(buffer, 1, 2, _DiskImage);
	dir->lastWriteTime = buffer[1] << 8 | buffer[0];

	// Read last write date
	fread(buffer, 1, 2, _DiskImage);
	dir->lastWriteDate = buffer[1] << 8 | buffer[0];

	// Read first logical sector
	fread(buffer, 1, 2, _DiskImage);
	dir->firstLogicalCluster = buffer[1] << 8 | buffer[0];

	// Read file size
	fread(buffer, 1, 4, _DiskImage);
	dir->fileSize = buffer[3] << 24 | buffer[2] << 16 | buffer[1] << 8 | buffer[0];
}

/**
 * Gets the disk offset (in bytes) of the file at the given index of the current directory.
 *
 * @param idx The index of the current directory
 * @return The disk offset in byte
 */
long _getDiskOffsetForDirEntry(int idx) {
	// Find the correct sector and offset through the sector
	int secAhead = idx / (_BootSector.bytesPerSector / DIR_ENTRY_SIZE);
	int offset = idx % (_BootSector.bytesPerSector / DIR_ENTRY_SIZE);

	uint16_t sector;

	// Offset the sector to the correct segment
	if (!_isCurrentRoot) {
		sector = _CurrentDirectory[0].firstLogicalCluster;

		// Advance through extra sectors
		for (int i = 0; i < secAhead; i++) {
			sector = _FATTables.fat1[sector];
		}

		sector += DATA_AREA_OFFSET - 2;
	} else {
		sector = ROOT_DIRECTORY_OFFSET;
	}

	return (sector * _BootSector.bytesPerSector) + (offset * DIR_ENTRY_SIZE);
}

/**
 * Gets the first free directory entry index in the given array of entries.
 * 
 * @param dirs The array on entries to search through
 * @return The index of the first free directory entry
 */
int _getFirstFreeIndexInSector(dir_entry *dirs) {
	return _getFirstFreeIndexInDirs(dirs, _BootSector.bytesPerSector / DIR_ENTRY_SIZE);
}

/**
* Gets the first free directory entry index in the given array of entries.
* Supports a variable max size.
*
* @param dirs The array on entries to search through
* @param maxSize The size of the array
* @return The index of the first free directory entry
*/
int _getFirstFreeIndexInDirs(dir_entry *dirs, int maxSize) {
	for (int i = 0; i < maxSize; i++) {
		if (dirs[i].filename[0] == DELETED || dirs[i].filename[0] == REMAINING_FREE) {
			return i;
		}
	}

	return -1;
}

/**
 * Saves the FAT Tables to the disc.
 */
void _saveFATTables() {
	uint8_t *fatBuffer = malloc(_BootSector.bytesPerSector * _BootSector.sectorsPerFAT);
	
	fseek(_DiskImage, FAT1_OFFSET * _BootSector.bytesPerSector, SEEK_SET);
	for (int i = 0, j = 0; j < _FATTables.numEntries; j += 2) {
		fatBuffer[i++] = (uint8_t) _FATTables.fat1[j];
		fatBuffer[i++] = (uint8_t) ((_FATTables.fat1[j] >> 8) & (0x0F)) | ((_FATTables.fat1[j + 1] << 4) & (0xF0));
		fatBuffer[i++] = (uint8_t) (_FATTables.fat1[j + 1] >> 4);
	}
	fwrite(fatBuffer, sizeof(uint8_t), _BootSector.bytesPerSector * _BootSector.sectorsPerFAT, _DiskImage);

	fseek(_DiskImage, FAT2_OFFSET * _BootSector.bytesPerSector, SEEK_SET);
	for (int i = 0, j = 0; j < _FATTables.numEntries; j += 2) {
		fatBuffer[i++] = (uint8_t)_FATTables.fat2[j];
		fatBuffer[i++] = (uint8_t)((_FATTables.fat2[j] >> 8) & (0x0F)) | ((_FATTables.fat2[j + 1] << 4) & (0xF0));
		fatBuffer[i++] = (uint8_t)(_FATTables.fat2[j + 1] >> 4);
	}
	fwrite(fatBuffer, sizeof(uint8_t), _BootSector.bytesPerSector * _BootSector.sectorsPerFAT, _DiskImage);

	free(fatBuffer);
}

/**
 * Saves the passed directory entry to the disc.
 */
void _saveDirEntry(dir_entry *dir) {
	fwrite(dir->filename, 1, 8, _DiskImage);
	fwrite(dir->extension, 1, 3, _DiskImage);
	fwrite(&dir->attributes, 1, 1, _DiskImage);
	fwrite(&dir->reserved, 1, 2, _DiskImage);
	fwrite(&dir->creationTime, 1, 2, _DiskImage);
	fwrite(&dir->creationDate, 1, 2, _DiskImage);
	fwrite(&dir->lastAccess, 1, 2, _DiskImage);
	fwrite(&dir->ignore, 1, 2, _DiskImage);
	fwrite(&dir->lastWriteTime, 1, 2, _DiskImage);
	fwrite(&dir->lastWriteDate, 1, 2, _DiskImage);
	fwrite(&dir->firstLogicalCluster, 1, 2, _DiskImage);
	fwrite(&dir->fileSize, 1, 4, _DiskImage);
	fflush(_DiskImage);
}

/**
 * Refreshes (reloads) the current directory.
 */
void _refreshDirectory() {
	if (_isCurrentRoot) {
		_loadRootDirectroy();
	} else {
		changeToDirectory(_CurrentDirectory[0].firstLogicalCluster);
	}
}

/**
 * Preps the given sector to be a new directory sector.
 *
 * @param sector The sector ID to set
 */
void _prepNewDirSector(uint16_t sector) {
	unsigned char *buffer = malloc(sizeof(unsigned char) * _BootSector.bytesPerSector);
	memset(buffer, 0, _BootSector.bytesPerSector);
	
	fseek(_DiskImage, (sector + DATA_AREA_OFFSET - 2) * _BootSector.bytesPerSector, SEEK_SET);
	fwrite(buffer, sizeof(unsigned char), _BootSector.bytesPerSector, _DiskImage);
	fflush(_DiskImage);
}

/**
 * Gets the first open sector.
 *
 * @return The ID of the first empty sector
 */
uint16_t _getFirstOpenSector() {
	for (int i = 0; i < _FATTables.numEntries; i++) {
		if (_FATTables.fat1[i] == UNUSED) {
			return i;
		}
	}

	return -1;
}