#include "include.h"

char* fileLoad(char *filename,int *bytesRead)
{
    PKMDWORD    filePtr;
    GDFS        gdfs;
    long        FileBlocks;
    int			i,flag = TRUE;
    char		buffer[256],*fptr;
	Sint32		status;

//	syCacheICI();
	
	gdFsChangeDir("\\");

	// change to the appropriate directory
	i = 0;
	fptr = filename;
	while(*fptr != 0)
	{
		switch(*fptr)
		{
			case '\\':
				buffer[i] = 0;
				gdFsChangeDir(buffer);
				fptr++;	
				i=0;			
				break;
				
			default:
				buffer[i++] = *fptr++;				
				break;		
		}
	}
	buffer[i] = 0;
	
    // Open input file.
    if(!(gdfs = gdFsOpen(buffer, NULL)))
        return NULL;

    // Get file size (in blocks/sectors).
    if(bytesRead)
		gdFsGetFileSize(gdfs, bytesRead);
    gdFsGetFileSctSize(gdfs, &FileBlocks);

    // Allocate memory to nearest block size (2048 bytes).
    filePtr = Align32Malloc(FileBlocks * 2048);
	if(filePtr == NULL)
		utilPrintf("Error\n");
	
    // Read file to memory region (Destination address must be 32-byte aligned).
    gdFsReqRd32(gdfs, FileBlocks, filePtr);

    // Wait for file access to finish.
    while(gdFsGetStat(gdfs) != GDD_STAT_COMPLETE)
    {
	    status = gdFsGetStat(gdfs);
    	if(status == GDD_STAT_ERR)
    		utilPrintf("Error\n");

    }

    // Close file.
    gdFsClose(gdfs);

    return filePtr;
}

