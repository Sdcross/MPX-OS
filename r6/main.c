#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// MSVC required for outputting binary data
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include "fat.h"

// Internal function prototypes
void _launchCommandInterface();
void _printBootSectorInfo();
void _printFATTableInfo();
void _printDirectoryEntries(dir_entry *entries, int maxEntries);
void _printDirectoryEntriesByType(dir_entry *entries, int maxEntries, char * ext);
void _printDirectoryEntriesByFileName(dir_entry *entries, int maxEntries, char * name, char * fileExt);
int _getClusterOfFileWithName(const char *name, const char *ext);
int _getSizeOfFileWithName(const char *name, const char *ext);
void _printFile(uint16_t sector, int fileSize, bool pag);
int _getIndexOfFileWithName(const char *name, const char *ext);
bool _fncmp(const char *n1, const char *n2);
bool _extcmp(const char *n1, const char *n2);
bool _nameCmpHelper(const char *n1, const char *n2, int maxElements);
void _callCommand(char *command);

char *paths[256];
int depth = 0;

// The name of the disk image
const char *imageName = "";

// For use when a filename is specified to display
bool printFileFlag = false;
char *filename = "";

// The actual disk image
FILE *diskImage;

int main(int numArgs, char *args[]) {
	// First arg is the program name
	if (numArgs < 2) {
		printf("Error. Disk image name not specified.\n");
		return 1;
	}

	// Get the disk image name
	imageName = args[1];
	
	// Check if a file name was provided
	if (numArgs == 3) {
		printFileFlag = true;
		filename = args[2];
	}

	// Open the disk image
	diskImage = fopen(imageName, "rb+");
	if (diskImage == NULL) {
		printf("Error opening the disk. Does the file exist?\n");
		return 2;
	}

	initialize(diskImage);

	if (!printFileFlag) {
		paths[depth++] = "root";
		_launchCommandInterface();
	} else {
		if (strcmp(filename, "") == 0) {
			printf("Filename doesnt exist\n");
		}
		else {
			bool goingToDir = true;

			char * path = strtok(filename, "/");
			char * prevPath;

			while (goingToDir) {
				prevPath = path;
				path = strtok(NULL, "/");

				if (path == NULL) {
					goingToDir = false;
				} else {
					int idx = _getClusterOfFileWithName(prevPath, "");
					if (idx == -1) {
						printf("Invalid subdirectory name\n");
						return 3;
					}
					changeToDirectory(idx);
				}
			}

			char *filename1 = strtok(prevPath, ".");
			char *fileext1 = strtok(NULL, ".");

			int clust = _getClusterOfFileWithName(filename1, fileext1);
			int size = _getSizeOfFileWithName(filename1, fileext1);

			if (clust == -1 || size == -1) {
				printf("Invalid filename\n");
				return 4;
			}

			_printFile(clust, size, false);
		}
	}

	destroy();
	fclose(diskImage);

	return 0;
}

/**
 * Starts the interactive shell session.
 */
void _launchCommandInterface() {
	bool continueInput = true;

	while (continueInput) {
		char userInput[512]; //512 should be big enough right?

		printf("%s", paths[0]);
		for (int i = 1; i < depth; i++) {
			printf("/%s", paths[i]);
		}
		printf(">");
		scanf(" %[^\n]", userInput);

		if (strcmp(userInput, "exit") == 0) {
			continueInput = false;
		}
		else {
			_callCommand(userInput);
		}
	}
}

/**
* Handles parsing of command and calling of correct operations
*/
void _callCommand(char *command) {
	if (command == NULL) {
		return;
	}

	char * commName = strtok(command, " ");

	char * arguments[10];
	int nrArgs = 0;
	bool continueArgs = true;

	while (continueArgs) {
		char * newArg = strtok(NULL, " ");
		if (newArg == NULL) {
			continueArgs = false;
		}
		else {
			arguments[nrArgs++] = newArg;
		}
		if (nrArgs > 9) {
			printf("Invalid number of command arguments\n\n");
			return;
		}
	}

	//if through

	//print boot sector information
	if (strcmp(commName, "pbs") == 0) {
		if (nrArgs > 0) {
			printf("Invalid number of arguments for pbs command\n\n");
			return;
		}
		_printBootSectorInfo();
		printf("\n");
	}
	//print root directory
	else if (strcmp(commName, "root") == 0) {
		uint16_t currDir = getCurrentDirectory()[0].firstLogicalCluster;

		int max = changeToDirectory(0);
		_printDirectoryEntries(getCurrentDirectory(), max);

		changeToDirectory(currDir);
	}
	//change directory
	else if (strcmp(command, "cd") == 0) {
		if (nrArgs == 1) {
			if (strcmp(arguments[0], "..") == 0) {
				changeToParentDirectory();
				free(paths[--depth]);

			}
			else {
				int idx = _getClusterOfFileWithName(arguments[0], "");
				if (idx == -1) {
					printf("Invalid subdirectory name\n\n");
					return;
				}
				changeToDirectory(idx);
				paths[depth] = malloc(sizeof(char) * (strlen(arguments[0]) + 1));
				strcpy(paths[depth++], arguments[0]);
			}
		}
		else {
			printf("Invalid number of arguments for cd command\n\n");
		}
	}
	//list directory
	else if (strcmp(command, "list") == 0) {
		if (nrArgs == 0) {
			_printDirectoryEntries(getCurrentDirectory(), getCurrentDirectoryMaxSize());
			printf("\n");
		}
		if (nrArgs == 1) {
			if (strlen(arguments[0]) == 5) { //wildcard ex: *.ext
				if (arguments[0][0] == '*' && arguments[0][1] == '.') {
					//if wildcard
					arguments[0] += 2;
					_printDirectoryEntriesByType(getCurrentDirectory(), getCurrentDirectoryMaxSize(), arguments[0]);
					printf("\n");
					return;
				}
			}
			if (strlen(arguments[0]) > 12) {
				printf("Invalid argument length\n\n");
			}
			char * filename = strtok(arguments[0], ".");
			char * fileExt = strtok(NULL, ".");
			_printDirectoryEntriesByFileName(getCurrentDirectory(), getCurrentDirectoryMaxSize(), filename, fileExt);
			printf("\n");
		}
	}
	//type
	else if (strcmp(command, "type") == 0) {
		if (nrArgs == 0 || nrArgs > 1) {
			printf("Invalid number of arguments\n\n");
			return;
		}
		char *filename1 = strtok(arguments[0], ".");
		char *fileext1 = strtok(NULL, ".");
		if (fileext1 == NULL) {
			fileext1 = "";
		}

		if (!(_extcmp(fileext1, "C") == true || _extcmp(fileext1, "TXT") == true || _extcmp(fileext1, "BAT") == true)) {
			printf("Invalid filename\n\n");
			return;
		}


		int clust = _getClusterOfFileWithName(filename1, fileext1);
		int size =  _getSizeOfFileWithName(filename1, fileext1);

		if (clust == -1 || size == -1) {
			printf("Invalid filename\n\n");
			return;
		}

		_printFile(clust, size, true);
	}
	//rename
	else if (strcmp(command, "rename") == 0) {
		if (nrArgs == 0 || nrArgs > 2) {
			printf("Invalid number of arguments\n\n");
		}
		char *filename1 = strtok(arguments[0], ".");
		char *fileext1 = strtok(NULL, ".");

		char * filename2 = strtok(arguments[1], ".");
		char * fileext2 = strtok(NULL, ".");

		if (filename1 == NULL || filename2 == NULL || ((fileext1 == NULL && fileext2 != NULL) || (fileext2 == NULL && fileext1 != NULL))) {
			printf("Invalid file or directory names\n\n");
			return;
		}
		if (fileext1 == NULL) {
			fileext1 = "";
		}
		if (fileext2 == NULL) {
			fileext2 = "";
		}

		setFilename(_getIndexOfFileWithName(filename1, fileext1), filename2, fileext2);
	}
	//move
	else if (strcmp(command, "mv") == 0) {
		//todo

		if (nrArgs != 2) {
			printf("Invalid arguments\n\n");
			return;
		}

		char *filename1 = strtok(arguments[0], ".");
		char *fileext1 = strtok(NULL, ".");

		char * filename2 = arguments[1];

		if (filename1 == NULL || filename2 == NULL) {
			printf("Invalid file or directory names\n\n");
			return;
		}
		if (fileext1 == NULL) {
			fileext1 = "";
		}

		int idx = _getIndexOfFileWithName(filename1, fileext1);
		int clust = _getClusterOfFileWithName(filename2, "");
		if (idx == -1 || clust == -1) {
			printf("Invalid file or paths\n\n");
			return;
		}
		//PRAY TO THE ALL POWERFUL GODS OF COMPUTING
		//SACRAFICES TO OUR GODS ELON AND TURING
		bool isMoved = moveFile(idx, clust);
		if (isMoved) { 
			printf("File successfully moved\n\n");
		}
		else {
			printf("Unable to move file\n\n");
		}
	}
	else if (strcmp(command, "help") == 0) {
		printf("Commands:\n\n");
		printf("pbs\n");
		printf("   -- Prints the boot sector of the disk\n\n");
		printf("root\n");
		printf("   -- Prints a list of all files and directories in the root directory\n\n");
		printf("cd [new directory]\n");
		printf("   -- Changes current directory to the one provided as [new directory]\n\n");
		printf("list [filename/type filter]\n");
		printf("   -- Lists all files in the current directory if no filter provided\n");
		printf("   -- If provided with filter will display only files or directories matching it\n\n");
		printf("type [filename]\n");
		printf("   -- Prints the contents of a .txt, .bat, or .c file to screen provided the [filename]\n\n");
		printf("rename [oldName] [newName]\n");
		printf("   -- Renames the file with name [oldName] to have the name [newName]\n");
		printf("   -- NOTE - file names must be 8 characters with a 3 character extension\n\n");
		printf("mv [oldFilePath] [newDirectory]\n");
		printf("   -- Moves the file with path [oldFilePath] to locaton [newDirectory]\n\n");
		printf("help\n");
		printf("   --prints this\n\n");
	}
	else {
		printf("Invalid command\n\n");
	}
}

/**
* Prints information for the boot sector
*/
void _printBootSectorInfo() {
	boot_sector *bootSector = getBootSector();

	printf(
		"Bytes per Sector: %d\n"
		"Sectors per Cluster: %d\n"
		"Number of Reserved Sectors: %d\n"
		"Number of FAT Copies: %d\n"
		"Max Number of Root Directory Entries: %d\n"
		"Total # of Sectors in the File System: %d\n"
		"Number of Sectors per FAT: %d\n"
		"Sectors per Track: %d\n"
		"Number of Heads: %d\n"
		"Total Sector Count for FAT32: %d\n"
		"Boot Signature: %d\n"
		"Volume ID: %d\n"
		"Volume Label: %s\n"
		"File System Type: %s\n",
		bootSector->bytesPerSector,
		bootSector->sectorsPerCluster,
		bootSector->numReservedSectors,
		bootSector->numFATCopies,
		bootSector->maxRootDirEntries,
		bootSector->numSectors,
		bootSector->sectorsPerFAT,
		bootSector->sectorsPerTrack,
		bootSector->numHeads,
		bootSector->sectorCountFAT32,
		bootSector->bootSig,
		bootSector->volId,
		bootSector->volName,
		bootSector->fileSystemType
	);
}

/**
* Prints fat table information
*/
void _printFATTableInfo() {
	fat_tables *fatTables = getFATTables();

	printf("FAT Table 1\n");
	for (int i = 0; i < fatTables->numEntries; i++) {
		printf("%x\n", fatTables->fat1[i] & 0xFFF);
	}

	printf("FAT Table 2\n");
	for (int i = 0; i < fatTables->numEntries; i++) {
		printf("%x\n", fatTables->fat2[i] & 0xFFF);
	}
}

/**
* Prints all directory entries in current dir
*/
void _printDirectoryEntries(dir_entry *entries, int maxEntries) {
	int numSubDirs = 0;
	int *subDirIdxs = malloc(sizeof(int) * maxEntries);

	printf("File Name     Ext     File Size     Start Cluster\n");
	printf("=================================================\n");

	for (int i = 0; i < maxEntries; i++) {
		// Stop printing if all other enties are empty
		if (entries[i].filename[0] == 0x00) {
			break;
		}

		// Don't print deleted files, or system/hidden files, or the volume label
		if (entries[i].filename[0] == 0xE5 || entries[i].attributes & (HIDDEN | VOLUME_LABEL)) {
			continue;
		}

		// We'll display subdirectories later
		if (entries[i].attributes & SUBDIRECTORY) {
			subDirIdxs[numSubDirs++] = i;
			continue;
		}

		printf("%-9s     %-3s     %-10d    %d\n",
			entries[i].filename,
			entries[i].extension,
			entries[i].fileSize,
			entries[i].firstLogicalCluster);
	}

	for (int i = 0; i < numSubDirs; i++) {
		printf("%-9s                           %d\n",
			entries[subDirIdxs[i]].filename,
			entries[subDirIdxs[i]].firstLogicalCluster);
	}

	free(subDirIdxs);
}

/**
* Prints all directory entries in the current dir with the provided extension
*/
void _printDirectoryEntriesByType(dir_entry *entries, int maxEntries, char * ext) {
	int numSubDirs = 0;
	int *subDirIdxs = malloc(sizeof(int) * maxEntries);

	printf("File Name     Ext     File Size     Start Cluster\n");
	printf("=================================================\n");

	for (int i = 0; i < maxEntries; i++) {
		// Stop printing if all other enties are empty
		if (entries[i].filename[0] == 0x00) {
			break;
		}

		// Don't print deleted files, or system/hidden files, or the volume label
		if (entries[i].filename[0] == 0xE5 || entries[i].attributes & (HIDDEN | VOLUME_LABEL)) {
			continue;
		}

		// We'll display subdirectories later
		if (entries[i].attributes & SUBDIRECTORY) {
			subDirIdxs[numSubDirs++] = i;
			continue;
		}

		if (strcmp(entries[i].extension, ext) == 0) {
			printf("%-9s     %-3s     %-10d    %d\n",
				entries[i].filename,
				entries[i].extension,
				entries[i].fileSize,
				entries[i].firstLogicalCluster);
		}
	}

	free(subDirIdxs);
}

/**
* Prints any files with given name
*/
void _printDirectoryEntriesByFileName(dir_entry *entries, int maxEntries, char * name, char * fileExt) {
	int numSubDirs = 0;
	int *subDirIdxs = malloc(sizeof(int) * maxEntries);

	printf("File Name     Ext     File Size     Start Cluster\n");
	printf("=================================================\n");

	for (int i = 0; i < maxEntries; i++) {
		// Stop printing if all other enties are empty
		if (entries[i].filename[0] == 0x00) {
			break;
		}

		// Don't print deleted files, or system/hidden files, or the volume label
		if (entries[i].filename[0] == 0xE5 || entries[i].attributes & (HIDDEN | VOLUME_LABEL)) {
			continue;
		}

		// We'll display subdirectories later
		if (entries[i].attributes & SUBDIRECTORY) {
			subDirIdxs[numSubDirs++] = i;
			continue;
		}

		if (strcmp(entries[i].filename, name) == 0 && strcmp(entries[i].extension, fileExt) == 0) {
			printf("%-9s     %-3s     %-10d    %d\n",
				entries[i].filename,
				entries[i].extension,
				entries[i].fileSize,
				entries[i].firstLogicalCluster);
		}
	}

	free(subDirIdxs);
}

/**
* Prints contents of given file
*/
void _printFile(uint16_t sector, int fileSize, bool pag) {
	unsigned char *file = getFileFromSector(sector, fileSize);

	if (pag) {
		int newlineCount = 0;
		int outStart = 0;

		int i = 0;
		for (; i < fileSize; i++) {
			if (file[i] == '\r' || file[i] == '\n') {
				newlineCount++;
			}

			if (newlineCount > 25) {
				fwrite(file + outStart, 1, i - outStart, stdout);
				outStart = i;
				newlineCount = 0;
				getc(stdin);
			}
		}
		fwrite(file + outStart, 1, i - outStart, stdout);
	} else{
		#ifdef _WIN32
		_setmode(1, _O_BINARY);				// MSVC
		#endif

		fwrite(file, sizeof(unsigned char), fileSize, stdout);

		#ifdef _WIN32
		_setmode(1, _O_TEXT);				// MSVC
		#endif
	}

	free(file);
}

/**
 * Gets the index of the file with the specified name and extension
 */
int _getIndexOfFileWithName(const char *name, const char *ext) {
	dir_entry *dirs = getCurrentDirectory();
	
	for (int i = 0; i < getCurrentDirectoryMaxSize(); i++) {
		if (_fncmp(dirs[i].filename, name) && _extcmp(dirs[i].extension, ext)) {
			return i;
		}
	}

	return -1;
}

/**
* Gets the cluster of the file with the given name
*/
int _getClusterOfFileWithName(const char *name, const char *ext) {
	dir_entry *dirs = getCurrentDirectory();

	for (int i = 0; i < getCurrentDirectoryMaxSize(); i++) {
		if (_fncmp(dirs[i].filename, name) && _extcmp(dirs[i].extension, ext)) {
			return dirs[i].firstLogicalCluster;
		}
	}

	return -1;
}

/**
* Gets the size of the file with the given name
*/
int _getSizeOfFileWithName(const char *name, const char *ext) {
	dir_entry *dirs = getCurrentDirectory();

	for (int i = 0; i < getCurrentDirectoryMaxSize(); i++) {
		if (_fncmp(dirs[i].filename, name) && _extcmp(dirs[i].extension, ext)) {
			return dirs[i].fileSize;
		}
	}

	return -1;
}


/**
 * Compares the filename with the given filename to determine equality.
 * // TODO: Words are hard
 * @param n1 The first name of the comparison (no null terminator)
 * @param n2 The second name of the comparison (null terminated)
 * 
 * @return True, if the names are equal 
 */
bool _fncmp(const char *n1, const char *n2) {
	return _nameCmpHelper(n1, n2, MAX_FILENAME_LENGTH);
}

/**
 * Compares the extension with the given extension to determine equality.
 * // TODO: Words are hard
 * @param n1 The first name of the comparison (no null terminator)
 * @param n2 The second name of the comparison (null terminated)
 * 
 * @return True, if the names are equal 
 */
bool _extcmp(const char *n1, const char *n2) {
	return _nameCmpHelper(n1, n2, MAX_EXT_LENGTH);
}

/**
 * Compares two string name with up to 'maxElements' number of characters.
 * // TODO: Words are hard
 * @param n1 The first name of the comparison (no null terminator)
 * @param n2 The second name of the comparison (null terminated)
 * @param maxElements The number of elements to check
 * 
 * @return True, if the names are equal 
 */
bool _nameCmpHelper(const char *n1, const char *n2, int maxElements) {
	bool foundNull = false;
	bool equal = true;

	for (int i = 0; i < maxElements && equal; i++) {
		if (foundNull) {
			equal = equal && (n1[i] == ' ');
		} else {
			if (n2[i] == '\0') {
				foundNull = true;
				equal = equal && (n1[i] == ' ');
			} else {
				equal = equal & (toupper(n1[i]) == toupper(n2[i]));
			}
		}
	}

	return equal;
}
