/*	I like textures, textures are my friend, even when their bit depth, drives me round the bend

*/

#define WIN32_LEAN_AND_MEAN

#include <gelf.h>
#include <windows.h>
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>

enum {
	TEXTURE_NORMAL,
	TEXTURE_AI,
	TEXTURE_PROCEDURAL
};


struct TEXTURE_HEADER
{
	DWORD flags;
	DWORD CRC;
	short dim[2];
	char name[12];
};

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

const char* ttypeNames[3] = { "RGB", "Alpha", "Procedural" };

int TextureType(const char* filename)
{
	if (strncmp(filename,"ai_",3)==0)
		return TEXTURE_AI;
//	else if (strncmp(mys,"ac_",3)==0)	god knows...
//		return TEXTURE_AC;
	else if (strncmp(filename,"prc",3)==0)
		return TEXTURE_PROCEDURAL;
	else
		return TEXTURE_NORMAL;
}

int CountTextures(const char *folder)
{
	char path[MAX_PATH];
	int count = 0;
	WIN32_FIND_DATA find;
	HANDLE			hFind;

	strcpy(path, folder);
	strcat(path, "\\*.bmp");

	hFind = FindFirstFile(path, &find);

	while (1)
	{
		count++;
		if (!FindNextFile(hFind, &find)) break;
	}

	FindClose(hFind);

	return count;
}

int SquashBitmapToFile(const char* path, const char* shortName, FILE *out)
{
	int pal = -1;
	int width, height, type;
	void *data;
	TEXTURE_HEADER head;

	data = gelfLoad_BMP((char*)path, NULL, (void**)&pal, &width, &height, NULL, GELF_IFORMAT_16BPP555, GELF_IMAGEDATA_TOPDOWN);

	type = TextureType(shortName);
	cout << '\t' << width << 'x' << height << ' ' << ttypeNames[type] << '\n';
	cout.flush();

	head.dim[0] = width;
	head.dim[1] = height;
	head.flags = type;
	strncpy(head.name, shortName, 11);
	head.name[11] = 0;
	head.CRC = utilStr2CRC((char*)shortName);

	fwrite(&head, sizeof(head), 1, out);
	fwrite(data, (width*height*2), 1, out);

	free(data);

	return 0;
}


int MakeSquashedBitmap(const char *file, const char *output)
{
	char shortName[32], *c;
	int n = strlen(file);

	c = (char*)file + n-1;
	
	while (n--)
	{
		if (*c == '\\') { c++; break; }
	}

	strcpy(shortName, c);
	strlwr(shortName);

	FILE *out = fopen(output, "wb");
	SquashBitmapToFile(file, shortName, out);
	fclose(out);

	return 0;
}

int SquashTextureBank(const char *folder, const char *output)
{
	char path[MAX_PATH], shortName[32];
	WIN32_FIND_DATA find;
	HANDLE			hFind;
	FILE			*out;
	
	int textures = CountTextures(folder);

	strcpy(path, folder);
	strcat(path, "\\*.bmp");

	hFind = FindFirstFile(path, &find);

	out = fopen(output, "wb");

	fwrite(&textures, 4, 1, out);

	while (1)
	{
		cout << find.cFileName;

		strcpy(path, folder);
		strcat(path, "\\");
		strcat(path, find.cFileName);
		
		strcpy(shortName, find.cFileName);
		strlwr(shortName);

		SquashBitmapToFile(path, shortName, out);

		textures++;

		if (!FindNextFile(hFind, &find)) break;
	}

	FindClose(hFind);
	fclose(out);

	cout << "\nConverted and saved " << textures << " textures\n";

	return 0;
}

int MakeLotsaSquashedBitmap(const char *folder)
{
	char path[MAX_PATH], out[MAX_PATH], *c;
	WIN32_FIND_DATA find;
	HANDLE			hFind;
	int textures = 0;
	
	strcpy(path, folder);
	strcat(path, "\\*.bmp");

	hFind = FindFirstFile(path, &find);

	while (1)
	{
		cout << find.cFileName;

		strcpy(path, folder);
		strcat(path, "\\");
		strcat(path, find.cFileName);

		strcpy(out, path);
		c = out + strlen(out) - 4;
		strcpy(c, ".bit");
		
		MakeSquashedBitmap(path, out);

		textures++;

		if (!FindNextFile(hFind, &find)) break;
	}

	FindClose(hFind);

	cout << "\nConverted and saved " << textures << " textures\n";

	return 0;
}

int main(int argc, char **argv)
{
	utilInitCRC();
	gelfInit();

	cout << "Frogger2 Texture Banker (c)2000 Blitz Games\n";


	if (argc == 4 && (stricmp(argv[1], "-f") == 0))
		MakeSquashedBitmap(argv[2], argv[3]);
	if (argc == 3 && (stricmp(argv[1], "-d") == 0))
		MakeLotsaSquashedBitmap(argv[2]);
	else if (argc == 3)
		SquashTextureBank(argv[1], argv[2]);
	else
	{
		cout << "\ntexturebanker (folder) (output)\nor\ntexturebanker -f (file) (output)\n";
		return 1;
	}
	
	gelfShutdown();
	return 0;
}
