#ifndef __FILES_H
#define __FILES_H

#define MAX_OBJECTS			500
#define MAX_OBJECT_BANKS	100

#define MAX_TEXTURES		500
#define MAX_TEXTURE_BANKS	100



/*
typedef struct
{
	char name[MAX_PATH];
	char filename[MAX_PATH];
	int flags;
}OBJECT_TYPE;
*/

typedef struct
{
	char	segmentident[32];
	unsigned int 	firstframe;
	unsigned int	lastframe;
	float	speed;
	int	 	flags;

}ANIMRANGE;



typedef struct TAGOBJECTINFO
{
	struct TAGOBJECTINFO	*next,*prev;
	char	filename[64];
	char	fullFilename[256];	
	int		size;
//	char	*pData;
	INPUT_OBJECT	*object;
	int		objID;
	int		animStart, animEnd;
	int		defaultAnimSpeed;
	int		numAnimRanges;
	ANIMRANGE	*ranges;
	BOOL	drawList;
	BOOL	skinned;
	INPUT_OBJECT	fileObject;

}OBJECTINFO;

typedef struct
{
	OBJECTINFO	head;
	int	 	numEntries;
}OBJECTLIST;



typedef struct
{
	char	filename[MAX_PATH];
	char	bankFile[MAX_PATH];
	char	objectOutput[MAX_PATH];
	char	textureOutput[MAX_PATH];
	char	lndSource[MAX_PATH];
	char	lndDest[MAX_PATH];
	OBJECTLIST objectList;
	short		loaded;
	short		saveMe;


}OBJECT_BANK_TYPE;

typedef struct
{
	char	filename[MAX_PATH];
	char	destination[MAX_PATH];
	char	exportPath[MAX_PATH];
	BITMAP_TYPE	*textures;//[MAX_TEXTURES];
	int		numTextures;
	int		loaded;
}TEXTURE_BANK_TYPE;





extern OBJECT_BANK_TYPE		objectBanks[];
extern int					numObjectBanks;
extern int					selectedObjectBank;
extern int					selectedObject;

extern TEXTURE_BANK_TYPE	textureBanks[];
extern int					numTextureBanks;
extern int					selectedTextureBank;
extern int					selectedTexture;



extern char filePath[];
extern char commandPath[];
extern char recentAddPath[];



void LoadDefaults();
int	GetSaveName(char *exportPath, char *title, char *filter);
void WriteChar(FILE *fp, char outShort);
void WriteShort(FILE *fp, unsigned short outShort);
void WriteInt(FILE *fp, int outInt);
void SaveObjectBanks(int);
void ChangeTextureBankInfo(char *origName, char *newName);
BOOL FileExists(char *name);

int AddTextures();
int DeleteTextures();
void KillTextureBank(int num, int removeOffset);
void LoadTextureBank(int num, char *name);
void WaitForCommand();
BOOL FAR PASCAL WaitForFlynn(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
void RefreshTextureBank(int removeOffset);
void ChangeCyclingSpeed(BITMAP_TYPE *bmp);
int LoadObjectBank(int num);
int BrowseForFile(char *title, char *filter, char *name);
//int BrowseForFile(char *name);
int BrowseForDirectory(HWND hwnd, char *title, char *name);
BOOL FAR PASCAL MyBrowseForDirectory(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
int CountFilesInDirectory(char *buf);
void ReadConfigFile();
void WriteConfigFile();
void CopyLNDFile(int bankNum);






#endif
