/*	Animated texture parser for PSX

	Part of Frogger2, (c) 2000 Blitz Games

	Programmer:	David Swift
	Date:		19 May 00
*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <io.h>
#include <direct.h>

#define POLYNOMIAL			0x04c11db7L			// polynomial for crc algorithm
static unsigned long	CRCtable[256];			// crc table


struct AnimFrame
{
	unsigned long CRC;
	long time;
};

struct Anim
{
	unsigned long CRC;
	AnimFrame frames[50];
	int numFrames;
};

Anim anims[20];
int numAnims;

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


int loadTextureTxt(const char* filename)
{
	FILE *animFp;
	int numFrames;
	char tempName[255], *c;

	strcpy(tempName, filename);

	for (c = tempName; *c && (*c!='.'); c++)
		*c = toupper(*c);
	*c = 0;

	Anim* anim = &anims[numAnims];

	anim->CRC = utilStr2CRC(tempName);

	if (animFp = fopen(filename,"rt"))
	{
		char line[255];

		fgets(line,255,animFp);
		sscanf(line,"%d", &numFrames);

		printf("Animated texture: %s (%d frames)\n", filename, numFrames);
		
		anim->numFrames = numFrames;
		
		
		anim->CRC;
		
		for (int i=0; i<numFrames; i++)
		{
			fgets(line,255,animFp);
			sscanf(line,"%s",tempName);

			for(char*up=tempName;(*up)&&(*up!='.');up++)
				*up = toupper(*up);
			*up = 0;

			fgets(line,255,animFp);

			if (line[0]=='"')
				anim->frames[i].time = anim->frames[i-1].time;
			else
				sscanf(line,"%ld",&anim->frames[i].time);										

			anim->frames[i].CRC= utilStr2CRC(tempName);

			printf("%02d: %-12s -> %08lx (%d frames)\n", i, tempName, anim->frames[i].CRC, anim->frames[i].time);
		}
		fclose(animFp);
	}

	return numFrames;
}


int CheckFiles(const char* path)
{
	long find;
	_finddata_t findInfo;

	find = _findfirst(path, &findInfo);

	if (find == -1) return 0;
	
	while (1)
	{
		loadTextureTxt(findInfo.name);
		numAnims++;

		if (_findnext(find, &findInfo) == -1) break;
	}

	_findclose(find);

	return 1;
}

int WriteOutData(const char* out)
{
	FILE *f;
	short n;
	unsigned long l;
	int anim, frame;

	f = fopen(out, "wb");
	if (!f)
		return 0;

	n = numAnims;
	fwrite(&n, 2, 1, f);

	for (anim=0; anim<numAnims; anim++)
	{
		n = anims[anim].numFrames;
		fwrite(&n, 2, 1, f);

		l = anims[anim].CRC;
		fwrite(&l, 4, 1, f);

		for (frame=0; frame<anims[anim].numFrames; frame++)
		{
			l = anims[anim].frames[frame].CRC;
			n = (short)anims[anim].frames[frame].time;

			fwrite(&l, 4, 1, f);
			fwrite(&n, 2, 1, f);
		}
	}

	fclose(f);

	return 1;
}


int main(int argc, char**argv)
{
	char oldFolder[1024];
	int res;

	if (argc != 3)
	{
		fprintf(stderr, "Usage: animtex.exe <folder> <output>\n");
		return -1;
	}

	utilInitCRC();

	_getcwd(oldFolder, 1024);
	_chdir(argv[1]);

	res = CheckFiles("*.txt");
	
	if (!res) {
		fprintf(stderr, "Failed reading files from texture bank\n");
	}

	_chdir(oldFolder);	
	
	if (res)
		WriteOutData(argv[2]);
	
	return 0;
}
