#include "islmem.h"
#include "islfile.h"
#include "Main.h"
#include "windows.h"

#define PSX_BASEDIR	"X:\\TeamSpirit\\psxversion\\cd\\"

void fileInitialise(char *fileSystem)
{
	// do nothing
}

unsigned char *fileLoad(unsigned char *filename, int *bytesRead)
{
	char fname[256];
	void *buffer;
	DWORD size, read;
	HANDLE h;

	strcpy(fname, PSX_BASEDIR);
	strcat(fname, filename);

	h = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE) return NULL;
	size = GetFileSize(h, NULL);
	buffer = MALLOC0(size);
	ReadFile(h, buffer, size, &read, NULL);
	CloseHandle(h);
	
	if (bytesRead) *bytesRead = read;
	return (unsigned char*)buffer;
}
