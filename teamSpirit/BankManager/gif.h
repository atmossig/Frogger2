/***************************************************************************

  GIF file loader                                         R.Hackett 2/4/95

  excepting: LZW decoder (c)1987 Steven A. Bennett

**************************************************************************/
#define LOCAL static
#define IMPORT extern

#define FAST register

#define OUT_OF_MEMORY -10
#define BAD_CODE_SIZE -20
#define READ_ERROR -1
#define WRITE_ERROR -2
#define OPEN_ERROR -3
#define CREATE_ERROR -4

//typedef short WORD;
typedef unsigned short UWORD;
typedef char TEXT;
typedef unsigned char UTINY;
typedef long LONG;
typedef unsigned long ULONG;
typedef int INT;



/* GIF file structure */

typedef struct {
  char           id[6];
  short          width;
  short          height;
  unsigned char  colorInfo;
  unsigned char  bkgColor;
  unsigned char  zero;
} GIFH;


typedef struct {
  FILE          *fp;
  GIFH           h;
  short          colorRes;
  short          colorDepth;
  unsigned char  colorMap;
  unsigned char *palette;
  unsigned char	numImages;
} GIFF;



/* Nasty globals if you want to DIY */

extern GIFF *currGIF;
extern int   GIFlineCount, GIFscrWidth, GIFscrHeight;
extern char *GIFscrPtr;



/* Exported functions */

int load320x200GIF8(char *pathname, char *scrPtr, char *palPtr);
int loadGIF8(char *pathname, char *scrPtr, char *palPtr);
int getGIF8Info(char *filename, int *width, int *height);

GIFF *openGIF(char *fName);
void closeGIF(GIFF *gif);

short decoder(short linewidth);
