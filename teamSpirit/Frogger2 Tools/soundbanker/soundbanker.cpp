#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <mmsystem.h>
#include <iostream.h>
#include <stdlib.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <dinput.h>
#include <stdio.h>

// ------------------------ CRC shiiite ---------------------

#define POLYNOMIAL			0x04c11db7L			// polynomial for crc algorithm
static unsigned long	CRCtable[256];			// crc table

void utilInitCRC()
{
	register int i, j;
	register unsigned long CRCaccum;

	for (i=0; i<256; i++)
		{
		CRCaccum = ((unsigned long)i<<24);
		for (j=0; j<8; j++)
			{
			if (CRCaccum & 0x80000000L)
				CRCaccum = (CRCaccum<<1)^POLYNOMIAL;
			else
				CRCaccum = (CRCaccum<<1);
			}
		CRCtable[i] = CRCaccum;
		}
}


unsigned long utilStr2CRC(char *ptr)
{
	register int i, j;
	int size = strlen(ptr);
	unsigned long CRCaccum = 0;

	for (j=0; j<size; j++)
		{
		i = ((int)(CRCaccum>>24)^(*ptr++))&0xff;
		CRCaccum = (CRCaccum<<8)^CRCtable[i];
		}
	return CRCaccum;
}


int CountSamples(const char *folder)
{
	char path[MAX_PATH];
	int count = 0;
	WIN32_FIND_DATA find;
	HANDLE			hFind;

	strcpy(path, folder);
	strcat(path, "\\*.wav");

	hFind = FindFirstFile(path, &find);
	if( hFind == INVALID_HANDLE_VALUE )
		return 0;

	do
	{
		count++;
	} while( FindNextFile(hFind, &find) );

	FindClose(hFind);

	return count;
}

typedef struct
{
	unsigned long length, uid, loop;

} SAMPLE_HEADER;

int SquashSampleToFile(char* path, char* shortName, FILE *out)
{
	HMMIO 			hwav;    // handle to wave file
	MMCKINFO		parent,  // parent chunk
					child;   // child chunk
	WAVEFORMATEX    wfmtx;   // wave format structure
	unsigned char *snd_buffer;
	unsigned long i;
	SAMPLE_HEADER shdr;

  
	// open the WAV file
	if( (hwav = mmioOpen( path, NULL, MMIO_READ | MMIO_ALLOCBUF )) == NULL )
		return 0;

	// descend into the RIFF 
	parent.fccType = mmioFOURCC ( 'W', 'A', 'V', 'E' );

	if ( mmioDescend ( hwav, &parent, NULL, MMIO_FINDRIFF ) )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, no wave section
		return 0; 	
    }

	// descend to the WAVEfmt 
	child.ckid = mmioFOURCC ( 'f', 'm', 't', ' ' );

	if ( mmioDescend ( hwav, &child, &parent, 0 ) )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, no format section
		return 0; 	
    }

	// now read the wave format information from file
	if ( mmioRead ( hwav, ( char * ) &wfmtx, sizeof ( wfmtx ) ) != sizeof ( wfmtx ) )
    {
		// close file
		mmioClose ( hwav, 0 );

		// return error, no wave format data
		return 0;
    }

	// make sure that the data format is PCM
	if ( wfmtx.wFormatTag != WAVE_FORMAT_PCM )
    {
		// close the file
		mmioClose ( hwav, 0 );

		// return error, not the right data format
		return 0; 
    }

	// now ascend up one level, so we can access data chunk
	if ( mmioAscend ( hwav, &child, 0 ) )
	{
		// close file
		mmioClose ( hwav, 0 );

		// return error, couldn't ascend
		return 0; 	
	}

	// descend to the data chunk 
	child.ckid = mmioFOURCC ( 'd', 'a', 't', 'a' );

	if ( mmioDescend ( hwav, &child, &parent, MMIO_FINDCHUNK ) )
    {
		// close file
		mmioClose ( hwav, 0 );

		// return error, no data
		return 0; 	
    }

	i=0;
	while( shortName[i] && shortName[i] != '.' ) i++;
	shortName[i] = '\0';

	// allocate the memory to load sound data
	snd_buffer = (unsigned char *)malloc( child.cksize );
	mmioRead ( hwav, (char *)snd_buffer, child.cksize );
	mmioClose ( hwav, 0 );

	shdr.length = child.cksize;
	shdr.loop = (shortName[0] == 'l' && shortName[1] == 'p' && shortName[2] == '_');
	shdr.uid = utilStr2CRC( shortName );

	// Write a sample header
	fwrite((void *)&shdr, sizeof(SAMPLE_HEADER), 1, out );

	// And the format information
	fwrite( (void *)&wfmtx, sizeof(WAVEFORMATEX), 1, out );

	// And then the actual data
	fwrite((void *)snd_buffer, child.cksize, 1, out);
	free(snd_buffer);

	return 0;
}

int SquashSampleBank(const char *folder, const char *output)
{
	char path[MAX_PATH], shortName[32];
	WIN32_FIND_DATA find;
	HANDLE			hFind;
	FILE			*out;
	
	int samples = CountSamples(folder);

	strcpy(path, folder);
	strcat(path, "\\*.wav");

	hFind = FindFirstFile(path, &find);

	out = fopen(output, "wb");

	fwrite(&samples, 4, 1, out);

	if( samples > 0 )
	{
		do
		{
			cout << find.cFileName << endl;
			cout.flush();

			strcpy(path, folder);
			strcat(path, "\\");
			strcat(path, find.cFileName);
			
			strcpy(shortName, find.cFileName);
			strlwr(shortName);

			SquashSampleToFile(path, shortName, out);

		} while( FindNextFile(hFind, &find) );

		FindClose(hFind);
	}

	fclose(out);

	cout << "\nConverted and saved " << samples << " samples\n";

	return 0;
}

int main(int argc, char **argv)
{
	utilInitCRC();

	SquashSampleBank(argv[1], argv[2]);

	return 0;
}

