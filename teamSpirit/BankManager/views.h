#ifndef __VIEWS_H
#define __VIEWS_H



#define OBJECT_TEXTURES_X	6
#define OBJECT_TEXTURES_Y	4

#define TEXTURE_BANK_X		8
#define TEXTURE_BANK_Y		4

#define TEXTURE_WINDOW		1
#define OBJECT_WINDOW		2


typedef struct ObjectExplorerType {				// container for object explorer information
		HWND			mainhandle;		// main window handle
		HWND			treehandle;		// tree control handle
		HWND			infohandle;		// info control handle
		HIMAGELIST		imagelist;		// our tree control image list
		HMENU			meshpopupmenu;		// right button menu for a mesh
		HMENU			objectpopupmenu;	// right button menu for an object
		HMENU			noitempopupmenu;	// right button menu for when over nothing
		int				treewidth;	// width of tree control
		HTREEITEM		selecthitem;		// selected tree item handle
} ObjectExplorerType;



typedef struct
{
	int window, tileX, tileY, tileNum, tileValid;
	void *bitmap;
	int crc;
}TILE;

typedef struct 
{
	TILE currentTile;
	TILE selectedTile[1000];
	int numSelectedTiles;
	int xPos, yPos;
	int flags;
	RECT invalidateRect;
}MOUSE;

HTREEITEM InsertObject(INPUT_OBJECT *obj, HTREEITEM parent);

void FillObjList();
int HandleTreeNotification(NMHDR *nmh);
void DrawObjectTextures(HWND hwnd, PAINTSTRUCT *ps);
void DrawTextureBankTextures(HWND hwnd, PAINTSTRUCT *ps);
void CreateTreeWindow();
LONG InsertTreeItem(HTREEITEM parent, char *text, int haschildren, int isexpanded, int imageid, LPARAM lp);
void FindObjExpItemInfo(HTREEITEM item, INPUT_OBJECT **obj);
void SelectFirstObjectInTree();
void InitTreeWindow();
void FillExplorerTree(INPUT_OBJECT *obj);
void UpdateObjectStatusBar();
void UpdateTextureStatusBar();
void UpdateMouse();
void ClearMouseSelection();
void UpdateReferenceList();

void PrintMouse(PAINTSTRUCT *ps);
void CreateImage(BITMAPOBJ *ptr, int paletteOffset);
void PrintBitmap(PAINTSTRUCT *ps, BITMAPOBJ *ptr, int xp, int yp, int xsize, int ysize, int offset, int cycleStart, int cycleEnd);


extern int objectTexturesOffset;
extern int textureBankOffset;
extern int currentobjectNum;
extern ObjectExplorerType objectexplorer;
extern int mouseX, mouseY;

extern MOUSE mouse; 

#endif
