#ifndef __BITMAPS_H
#define __BITMAPS_H

#define GIF_FORMAT		(1<<0)
#define BMP_FORMAT		(1<<1)
#define COLOUR_CYCLING	(1<<2)
#define GIF_FORMATPH	(1<<3)


#define MAX_OBJECT_BITMAPS	500

typedef struct bitInfo
{
	BITMAPINFOHEADER	bi;
	RGBQUAD				palette[256];
}bitInfo;


typedef struct BITMAPOBJ
{	
	int	xpos,ypos, width, height;
	char	filename[256];
	int		ident;
	unsigned char	*data;
	bitInfo bitInf;
	unsigned char	*palette;//[768];
	int		bpp;
	LPBITMAPINFO bmi;
}BITMAPOBJ;


typedef struct 
{
	char		name[64];
	char		fullFilename[256];
	BITMAPOBJ	*bmpObj;
	int			flags;
	char		cycleStart, cycleEnd, cycleSpeed;
}BITMAP_TYPE;



BOOL IsBitmapInObject(BITMAP_TYPE *tex, INPUT_OBJECT *obj);

void GetObjectBitmapsFromNDO(INPUT_OBJECT *obj);
void GetObjectBitmapsFromBank(int bankNum);
void ShowUnusedTextures(int bankNum);
BOOL FAR PASCAL TexBankProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);

int LoadPicy(BITMAP_TYPE * picy);
int setupPic(BITMAPOBJ * ptr);
int GetBMPInfo(char *filename);
BITMAP_TYPE *FindTexture(unsigned long crc);
void AddObjectTextures(INPUT_OBJECT *obj, BOOL doChildren);
void ExportTextureBank(int bankNum);
void ExportObjectTextureBank(int bankNum);
void AdjustColours(unsigned char *r, unsigned char *g, unsigned char *b);
void CalculateAndOutputMipMap(BITMAPOBJ *bmp,FILE *fp);

void EdgeCorrect(int x, int y, unsigned char *rv, unsigned char *gv, unsigned char *bv, BITMAPOBJ *bmp);
void ConvertBitmapDataToShorts(unsigned short *destData,BITMAPOBJ *bmp);
void OutputTextureHeader(FILE *fp,int size);
void OutputData(FILE *fp,unsigned short *data,int numShorts,int width,int pitch);
void FilterData(unsigned short *destData,unsigned short *data,int width,int height);
void UpdateObjectBitmapWindow(INPUT_OBJECT *obj);
BOOL FAR PASCAL BitmapProperties(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
int CountTextureReference(BITMAP_TYPE *bitmap);
void ListTextureReference(BITMAP_TYPE *bitmap);





extern BITMAP_TYPE myBmp;

extern BITMAP_TYPE	*objectBitmaps[];
extern int					numObjectBitmaps;
extern int					currentObjectBitmap;
extern BITMAP_TYPE missingBmp;



#endif