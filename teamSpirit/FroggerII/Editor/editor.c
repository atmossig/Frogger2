/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: editor.c
	Programmer	: David Swift
	Date		: 28/06/99 15:53:22

----------------------------------------------------------------------------------------------- */


#include <ultra64.h>
#include <limits.h>
#include <stdio.h>

#include "incs.h"
#include "gelf.h"
#include "directx.h"
#include "mavis.h"	// ick

#include "editfile.h"
#include "editdefs.h"
#include "edcommand.h"
#include "edglobals.h"
#include "edtoolbars.h"
#include "editor.h"

/* ----------------------------------------------------------------------------------------------- */

const char* ENTITYTYPESTRINGS[] = {
	"Enemy", "Platform", "Garib"
};

//static const char *EDITVARSTRINGS[] = { "char*", "int","float" };

//char EDITOR_SHORTCUTS_A[] = "1234567890qwertyuiop";
//char EDITOR_SHORTCUTS_B[] = "asdfghjklzxcvbnm";

KMAP EDKEYMAP[] = 
{
	{ 'f',	TB_PLACEFLAG },
	{ 'k',	TB_CLEARPATH },
	{ 'e',	TB_PLACEENEMY },
	{ 'p',	TB_PLACEPLATFORM },
	{ 'g',	TB_PLACEGARIB },
	{ 'a',	TB_AUTOCAMERA },
	{ 'i',	TB_INFO },
	{ '3',	TB_TEST },
	{ '4',	TB_TEST_SCRIPTING },
	{ 'u',	TB_UNDO },
	{ '1',	TB_SAVE },
	{ 'l',	TB_LOAD },
	{ 's',	TB_RECTSELECT },
	{ 'c',	TB_COPY_SELECTION },
	{ 'v',	TB_PASTE_SELECTION },
	{ 'd',	TB_PICKFLAG },
	{ 'r',	TB_SET_ALL_PATHSPEEDS },
	{ 'b',	TB_SET_ALL_BOTHOFFSETS },
	{ 'w',	TB_SET_ALL_WAITTIMES },
	{ ' ',	TB_CLEAR_SELECTION },
	{ 'h',	TB_HELPSCREEN },
	{ '9',	TB_999LIVES },
	{ 'q',	TB_PLACEFROG },
	{ 't',	TB_TILESELECT },
	{ 'y',	TB_CAMERACASE_ADDTILES },
	{ 'n',	TB_SET_ENTITYTYPE },
	{ 'z',	TB_SCALE },
	{ 'o',	TB_RADIUS },
	{ '0',	TB_KILL_EVERYTHING },
	{ 'j',	TB_AUTOMAP_BABIES },
	{ 'x',  TB_SET_ID },
	{ 'm',	TB_ANIMSPEED },
	{ '.',	TB_CAMERACASE_SETDIRECTION },
	{ -1, -1 }
};
/* ----------------------------------------------------------------------------------------------- */

RECT statusRect = { 0, 380, 640, 480-60 };
RECT borderRect = { 0, 376, 640, 380 };
RECT toolbarRect = { 0, 480-60, 640, 480 };

static int commonFlagIcons[8] = {
	PLATFORM_NEW_FORWARDS,
	PLATFORM_NEW_BACKWARDS, 
	PLATFORM_NEW_CYCLE,
	PLATFORM_NEW_PINGPONG,
	PLATFORM_NEW_MOVEUP,
	PLATFORM_NEW_MOVEDOWN,
	0
};

static int platformFlagIcons[8] = {
	PLATFORM_NEW_STEPONACTIVATED,
	PLATFORM_NEW_CRUMBLES,
	PLATFORM_NEW_REGENERATES,
	PLATFORM_NEW_NOWALKUNDER,
	0, 0, 0, 0
};


struct TAGSTATUSWINDOW
{
	RECT r;
	char text[128], currentSelection[128];
} statusWin;

struct {
	POINT a, b;
	BOOL valid;
} selectRect;

typedef void (* EDITVARFUNC)(const char *) ;
typedef void (* SELECTFUNC)(int, int, int, int);

unsigned long mod = 0;

POINT mousePos;

/*	-------------------------------------------------------------------------------------------
	prototypes
*/

void DrawStatusInfo(void);
void ShowTileInfo(GAMETILE *tile);
void PlaceEditX(int x, int y);
void EditorMouseDown(int x, int y);
void EditorMouseUp(int x, int y);
void EditorMouseMove(int x, int y);
void EditorMoveCamera();
void EditorUpdateCamera();
void GetEditorCameraRotations(void);
void SaveEditorState(void);
void LoadEditorState(void);
int ToolbarMouseOver(int);
void ToolbarMouseDown(int);
GAMETILE *FindClickedTile(int x, int y);
CREATEENTITY *FindCreateNode(int x, int y);
EDITPATHNODE* FindFlag(int x, int y);
EDITPATHNODE* FindFlagOnTile(GAMETILE *tile);
GARIB *FindGarib(int x, int y);

/*	--------------------------------------------------------------------------------
	Function		: SaveEditorState
	Parameters		: 
	Returns			: 
*/

void SaveEditorState()
{
	FILE *f = fopen(EDITOR_STATE_FILE, "w");
	if (!f) return;

	fprintf(f, "%s\nFlags: ", savePath);

	if (invertMouse) fprintf(f, "invertmouse ");
	if (drawLinks) fprintf(f, "links ");
	if (drawNormals) fprintf(f, "normals ");
	if (drawVectors) fprintf(f, "vectors ");
	if (drawDots) fprintf(f, "dots ");
	fprintf(f, "\n");

	fprintf(f, "%f %f\n", HorizontalWhizziness, InOutWhizziness);

	fprintf(f, "%s\n", scriptPath);

	fclose(f);
}

/*	--------------------------------------------------------------------------------
	Function		: LoadEditorState
	Parameters		: 
	Returns			: 
*/

void LoadEditorState()
{
	char str[80];

	FILE *f = fopen(EDITOR_STATE_FILE, "r");
	if (!f) return;

	fscanf(f, "%s\n", savePath);
	fgets(str, 79, f);

	invertMouse = (strstr(str, "invertmouse")) ? TRUE : FALSE;
	drawNormals = (strstr(str, "normals")) ? TRUE : FALSE;
	drawVectors = (strstr(str, "vectors")) ? TRUE : FALSE;
	drawDots = (strstr(str, "dots")) ? TRUE : FALSE;
	drawLinks = (strstr(str, "links")) ? TRUE : FALSE;

	fscanf(f, "%f %f\n", &HorizontalWhizziness, &InOutWhizziness);
	fscanf(f, "%s\n", scriptPath);

	fclose(f);
}


/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void LoadEditIcons()
{
	char texturePath[MAX_PATH];
	int i;
	short *textureData;

	for (i=0; i<EDITORTEXTURES; i++)
	{
		//Copy the base directory and texture subdirectory into our path
		sprintf(texturePath, "%s" TEXTURE_BASE "editor\\editor%d.bmp", baseDirectory, i+1);

		//Load the bitmap
		textureData = GetGelfBmpDataAsShortPtr(texturePath,GELF_IFORMAT_16BPP555);
		
		//Convert it to a texture (copies to a surface)
		editicons[i].surface = CreateTextureSurface(64, 64, textureData, 1, 0x7C1F,0);
		editicons[i].handle = ConvertSurfaceToTexture(editicons[i].surface);
		
		// Fre the (no longer used) gelf data
		FreeGelfData(textureData);
	}

	dprintf"Loaded %d pages of editor textures\n", EDITORTEXTURES));
}
           
/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawMouse(void)
{
	float s = 16;

	if (lButton)
	{
		if (controlCamera) return;
		s = 12;
	}
	
	//GetCursorPos(&p);

	DrawAlphaSprite(
		(float)mousePos.x, (float)mousePos.y,0,
		s, s, 0, 0, 0.25f, 0.25f,
		editicons[0].handle, D3DRGBA(1,1,1,0.6) );
}

/*	--------------------------------------------------------------------------------
	Function		: InitEditor
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void InitEditor(void)
{
	static RECT statusSize = { 0, 350, 640, 480 };

	statusWin.r = statusSize;
	statusWin.text[0] = 0;
	strcpy(statusWin.currentSelection, "[no current tile]");

	selectRect.valid = FALSE;

	editPath = NULL;
	createList = MakeEditGroup();

	ZeroVector(&editCameraRot);
	//editCameraRot.v[X] = -(float)PI/4.0;

	//ZeroVector(&editCameraDir);

	LoadEditIcons();
	LoadEditorState();
}

/*	--------------------------------------------------------------------------------
	Function		: ShutdownEditor
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void ShutdownEditor(void)
{
	SaveEditorState();
	FreeEditPath(editPath);
	FreeCreateList();
}

/*	--------------------------------------------------------------------------------
	Function		: RunEditor
	Purpose			: Runs the editor
	Parameters		: 
	Returns			: 
	Info			: 
*/

void RunEditor(void)
{
	POINT p;
	RECT r;

	//if (editText.active) return;

	// Get mouse pos and scale it to screen coords
	GetCursorPos(&mousePos);

	if (winMode)
	{
		ScreenToClient(winInfo.hWndMain, &mousePos);
		GetClientRect(winInfo.hWndMain, &r);

		mousePos.x = (SCREEN_WIDTH * mousePos.x) / r.right;
		mousePos.y = (SCREEN_HEIGHT * mousePos.y) / r.bottom;
	}

	p = mousePos;

	EditorMoveCamera();

	if (showX) PlaceEditX(p.x, p.y);

	if (lButton)
	{
		if (!clicking)
			EditorMouseDown(p.x, p.y);
	}
	else
	{
		if (clicking)
		{
			//GetCursorPos(&p);
			EditorMouseUp(p.x, p.y);
			clicking = FALSE;
		}
	}

	if (p.x != oldMousePos.x || p.y != oldMousePos.y) // && p.y < toolbarRect.top)
		EditorMouseMove(p.x, p.y);

	oldMousePos = p;
}

void UpdateStatusString()
{
	if (editText.active) {
		sprintf(statusWin.text, "%s = %s_",
			editText.var,
			//EDITVARSTRINGS[editText.type],
			editText.string);
	}
	else if (cameraMode)
	{
		strcpy(statusWin.text, "Moving camera - press ESC to cancel");
	}
	else
	{
		switch (editMode)
		{
		case EDIT_PLACE_GARIB:
			sprintf(statusWin.text, "Place garib: %s", GARIBTYPESTRINGS[garibType]);
			break;

		case EDIT_PLACE_ENEMY:
			if( copyFlag )
				sprintf(statusWin.text, "Paste enemy on path: %s", createTypes[0]);
			else
				sprintf(statusWin.text, "Create enemy on path: %s", createTypes[0]);
			break;

		case EDIT_PLACE_PLATFORM:
			if( copyFlag )
				sprintf(statusWin.text, "Paste platform on path: %s", createTypes[1]);
			else
				sprintf(statusWin.text, "Create platform on path: %s", createTypes[1]);
			break;

		case EDIT_PLACE_PLACEHOLDER:
			sprintf(statusWin.text, "Create placeholder on path" );
			break;

		case EDIT_MAKE_PATH:
			strcpy(statusWin.text, "Making path");
			break;

		case EDIT_PLACE_FROG:
			strcpy(statusWin.text, "Placing frog");
			break;

		default:
			statusWin.text[0] = 0;
		}
	}
}

void PlaceEditX(int x, int y)
{
	int r = 100*100;

	if (!(editTile = FindClickedTile(x, y))) return;
	clickPos = editTile->centre;
}


void DoEditCamera(void)
{
	cameraMode = TRUE;
	TOOLBARBUTTONS[CAMERA_BUTTON].icon = 23;
	selectedIcon = -1;
	controlCamera = TRUE;

	SetCursorPos(320, 240);
	GetEditorCameraRotations();
	EditorUpdateCamera();
	UpdateStatusString();
}

/*	--------------------------------------------------------------------------------
	Function		: EditorMouseMove
	Purpose			: 
	Parameters		: int, int
	Returns			: 
	Info			: 
*/

void EditorMouseMove(int x, int y)
{
	float rot;
	VECTOR v;

	if (cameraMode && (lButton || rButton))
	{
		POINT mp;
		POINT p = { 320, 240 };

		if (winMode)
		{
			GetCursorPos(&mp);

			ScreenToClient(winInfo.hWndMain, &mp);
			x = mp.x; y = mp.y;

			ClientToScreen(winInfo.hWndMain, &p);
			SetCursorPos(p.x, p.y);
		}
		else
		{
			SetCursorPos(320, 240);
		}

		if (rButton)
		{
			if (lButton)
			{
				v.v[X] = (float)(x - 320) / 2;
				v.v[Y] = 0.0;
				v.v[Z] = (float)(y - 240) / 2;

				RotateVector2DXYZ(&v, &v, editCameraRot.v[X], X);
				RotateVector2DXYZ(&v, &v, editCameraRot.v[Y], Y);
				AddToVector(&currCamSource, &v);
				EditorUpdateCamera();
			}
			else 
			{
				editCameraRot.v[Y] += (320-x) * (float)(PI / 360);
				rot = (240-y) * (float)(PI / 2 / 480);
				if (invertMouse) rot = -rot;
				editCameraRot.v[X] += rot;
				EditorUpdateCamera();
			}
		}
		else if (lButton)
		{
			v.v[Y] = -(float)(y - 240) / 2;
			v.v[X] = (float)(x - 320) / 2;
			v.v[Z] = 0.0;

			RotateVector2DXYZ(&v, &v, editCameraRot.v[X], X);
			RotateVector2DXYZ(&v, &v, editCameraRot.v[Y], Y);
			AddToVector(&currCamSource, &v);
			EditorUpdateCamera();
		}
	}
	else if (rButton && !cameraMode)
	{
		DoEditCamera();
	}
	else if (lButton && editMode == EDIT_RECT_SELECT)
	{
		selectRect.b.x = x;
		selectRect.b.y = y;
	}
}

void EditorMoveCamera()
{
	float offsx, offsy;
	VECTOR v;
	BOOL foo = FALSE;

	if (KEYPRESS(DIK_UP))    { offsy = -InOutWhizziness; foo = TRUE; }
	if (KEYPRESS(DIK_DOWN))  { offsy = InOutWhizziness; foo = TRUE; }
	if (KEYPRESS(DIK_LEFT))	 { offsx = -HorizontalWhizziness; foo = TRUE; }
	if (KEYPRESS(DIK_RIGHT)) { offsx = HorizontalWhizziness; foo = TRUE; }

	if (KEYPRESS(DIK_LSHIFT) || KEYPRESS(DIK_RSHIFT)) { offsx *= 2; offsy *= 2; }

	if (foo && !controlCamera)
		DoEditCamera();

	if (foo)
	{
		v.v[Y] = 0.0;
		v.v[X] = offsx;
		v.v[Z] = offsy;

		RotateVector2DXYZ(&v, &v, editCameraRot.v[X], X);
		RotateVector2DXYZ(&v, &v, editCameraRot.v[Y], Y);
		AddToVector(&currCamSource, &v);

		//SetCursorPos(320,240);
	}

	if (controlCamera)
		EditorUpdateCamera();
}

void ToolbarMouseDown(int toolOpt)
{
	int command = 0;

	if (toolOpt >= 100)
	{
		if (submenu)
		{
			toolOpt -= 100;
			while (toolOpt--)
			{
				if (submenu[command].command == -1) return;
				command++;
			}
			ToolbarSelect(submenu[command].command);
		}
		else
			return; 
	}
	else
	{
		while (toolOpt--)
		{
			if (TOOLBARBUTTONS[command].command == -1) return;
			command++;
		}
		ToolbarSelect(TOOLBARBUTTONS[command].command);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: ToolbarMouseOver
	Purpose			: 
	Parameters		: 
	Returns			: enumerated command
	Info			: 
*/
int ToolbarMouseOver(int toolOpt)
{
	int command = 0;

	if (toolOpt >= 100)
	{
		if (submenu)
		{
			toolOpt -= 100;
			while (toolOpt--)
			{
				if (submenu[command].command == -1) return -1;
				command++;
			}
			return submenu[command].command;
		}
		else
			return -1; 
	}
	else
	{
		while (toolOpt--)
		{
			if (TOOLBARBUTTONS[command].command == -1) return -1;
			command++;
		}
	}

	return TOOLBARBUTTONS[command].command;
}

/*	--------------------------------------------------------------------------------
	Function		: EditorMouseDown
	Purpose			: 
	Parameters		: int, int
	Returns			: 
	Info			: 
*/

void EditorMouseDown(int x, int y)
{
	EDITPATHNODE *path;
	GAMETILE *tile;

	if (y > statusRect.top)
		ToolbarMouseDown(x / 16 + ((y - toolbarRect.top) / 18) * 100);
	else if (!cameraMode)
	{
		switch (editMode)
		{
		case EDIT_PLACE_GARIB:
			if (!(tile = FindClickedTile(x, y))) break;
			//v = tile->centre; v.v[Y] += 20;
			//CreateNewGarib(v, garibType, &firstTile[1], 0.0f);
			PlaceGarib(tile, garibType);
			break;

		case EDIT_MAKE_PATH:
			tile = FindClickedTile(x, y);
			if (tile)
			{
				if (!editPath) editPath = CreateEditPath();
				if (KEYPRESS(DIK_LSHIFT) || KEYPRESS(DIK_RSHIFT))
					EditorAddFlags(tile, editPath);
				else
					EditorAddFlag(tile, editPath);
			}
			break;

		case EDIT_PLACE_PLATFORM:
			path = FindFlagOnTile(editTile);
			if( !path )
			{
				FreeEditPath( editPath );
				editPath = CreateEditPath();
				path = EditorAddFlag(editTile, editPath);
			}
			PlacePlatform(path);
			UpdateSelection( );
			break;

		case EDIT_PLACE_PLACEHOLDER:
			path = FindFlagOnTile(editTile);
			if( !path )
			{
				FreeEditPath( editPath );
				editPath = CreateEditPath();
				path = EditorAddFlag(editTile, editPath);
			}
			PlacePlaceholder(path);
			UpdateSelection( );
			break;

		case EDIT_PLACE_ENEMY:
			path = FindFlagOnTile(editTile);
			if( !path )
			{
				FreeEditPath( editPath );
				editPath = CreateEditPath();
				path = EditorAddFlag(editTile, editPath);
			}
			PlaceEnemy(path);
			UpdateSelection( );
			break;

		case EDIT_RECT_SELECT:
			selectRect.a.x = x;
			selectRect.a.y = y;
			selectRect.b = selectRect.a;
			selectRect.valid = TRUE;
			break;

		case EDIT_PLACE_FROG:
			if (!(tile = FindClickedTile(x,y))) break;
			SetVector(&frog[0]->actor->pos, &tile->centre);
			
			if (currPlatform[0])
			{
				currPlatform[0]->carrying = NULL;
				currPlatform[0]->flags &= ~PLATFORM_NEW_CARRYINGFROG;
				currPlatform[0] = NULL;
			}
			currTile[0] = tile;
			gTStart[0] = tile;
			player[0].frogState = 0;
			break;
		}
	}

	clicking = TRUE;
}

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void EditorMouseUp(int x, int y)
{
	if (selectRect.valid) 
	{
		selectRect.valid = FALSE;
		
		if (selectRect.a.x == selectRect.b.x && selectRect.a.y == selectRect.b.y)
		{
			selectRect.a.x -= 2; selectRect.b.x += 2;
			selectRect.a.y -= 2; selectRect.b.y += 2;
		}

		if( addTiles && selectionList && selectionList->nodes && selectionList->nodes->thing &&
			((CREATEENTITY *)selectionList->nodes->thing)->thing == CREATE_CAMERACASE )
		{
			AddTilesToCameraCase( selectRect.a.x, selectRect.a.y, selectRect.b.x, selectRect.b.y );
		}
		else if (selectFunc)
		{
			selectFunc(selectRect.a.x, selectRect.a.y, selectRect.b.x, selectRect.b.y);
			UpdateStatusString();
		}
	}
}

void DrawEditorArrow(int x1, int y1, int x2, int y2, float width, D3DCOLOR col)
{
	float a, b;
	D3DTLVERTEX vtx[3];

	memset(vtx, 0, sizeof(D3DTLVERTEX)*3);

	if (x1==x2 && y1==y2) return;	// foolish

	if (x1 < 0 || x1 > 640 || x2 < 0 || x2 > 640 ||
		y1 < 0 || y1 > 480 || y2 < 0 || y2 > 480) return;	// pah

	a = (x2 - x1) * width; if (a < 1 && a > -1) a = (a < 0) ? -1 : 1;
	b = (y2 - y1) * width; if (b < 1 && b > -1) b = (b < 0) ? -1 : 1;

	vtx[0].dvSX = x2;
	vtx[0].dvSY = y2;
	vtx[0].dvSZ = 0.0001;
	vtx[0].dcColor = col;
	vtx[0].dcSpecular = D3DRGBA(0,0,0,1);

	vtx[1].dvSX = x1 - b;
	vtx[1].dvSY = y1 + a;
	vtx[1].dvSZ = 0.0001;
	vtx[1].dcColor = col;
	vtx[1].dcSpecular = D3DRGBA(0,0,0,1);

	vtx[2].dvSX = x1 + b;
	vtx[2].dvSY = y1 - a;
	vtx[2].dvSZ = 0.0001;
	vtx[2].dcColor = col;
	vtx[2].dcSpecular = D3DRGBA(0,0,0,1);

	Clip3DPolygon(vtx, 0);
/*	pDirect3DDevice->lpVtbl->DrawPrimitive(pDirect3DDevice,
		D3DPT_TRIANGLELIST, D3DVT_TLVERTEX,
		vtx, 3, D3DDP_DONOTLIGHT | D3DDP_DONOTUPDATEEXTENTS);
*/
}

/*	--------------------------------------------------------------------------------
	Function		: DrawEditorIcon
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawEditorIcon(int x, int y, int n, BOOL disabled)
{
	int page, row, col;

	if (n < 0) return;

	page = n >> 4;
	row = (n & 15) >> 2;
	col = (n & 3);

	DrawASprite((float)x, (float)y, 16, 16,
		col * EDITORICONSIZE, row * EDITORICONSIZE,
		(col+1) * EDITORICONSIZE, (row+1) * EDITORICONSIZE,
		editicons[page].handle,
		disabled ? D3DRGB(0.5f,0.5f,0.5f) : D3DRGB(1,1,1));
}

void DrawAllTiles(void)
{
	GAMETILE *cur;
	VECTOR v,v2,v3;
	int i;

	cur = firstTile;
	
	while (cur)
	{
		EdXfmPoint(&v, &cur->centre);
		
		if (v.v[Z])
		{
			for (i=0; i<4; i++)
			{
				if (drawLinks)
					if (cur->tilePtrs[i])
					{
						EdXfmPoint(&v2, &cur->tilePtrs[i]->centre);
						if (v2.v[Z])
							DrawEditorArrow(v.v[X], v.v[Y],v2.v[X], v2.v[Y], 0.05f, D3DRGB(0.9,0,1));
					}
				
				if (drawVectors)
				{
					SetVector(&v3, &cur->dirVector[i]);
					ScaleVector(&v3, 20.0);
					AddToVector(&v3, &cur->centre);
					
					EdXfmPoint(&v2, &v3);
				
					if (v2.v[Z])
						DrawEditorArrow(v.v[X], v.v[Y],v2.v[X], v2.v[Y], 0.1f, D3DRGB(0,0,1));

					if (i==0)
						DrawEditorIcon(v2.v[X]-7, v2.v[Y]-7, 45, FALSE);
				}


			}

			
			if (drawNormals)
			{
				SetVector(&v3, &cur->normal);
				ScaleVector(&v3,20.0);
				AddToVector(&v3, &cur->centre);

				EdXfmPoint(&v2, &v3);
				
				if (v2.v[Z])
					DrawEditorArrow(v.v[X], v.v[Y],v2.v[X], v2.v[Y], 0.15, D3DRGB(0,0,0));
			}

			if (drawDots)
				DrawEditorIcon(v.v[X] - 7, v.v[Y] - 7, 60, FALSE);
		}
		
		cur = cur->next;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: DrawEditor
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

static float womble = 0;

void DrawEditPath(EDITPATH *ep, BOOL shadey)
{
	EDITPATHNODE *path, *next;
	VECTOR v, w;

	for (path = ep->nodes; path; path = next)
	{
		next = path->link;
		/* if there's a link from this, draw a line */
		if (next) {
			EdXfmPoint(&v, &path->tile->centre);
			EdXfmPoint(&w, &next->tile->centre);

			if (w.v[Z] != 0 && v.v[Z] != 0)
			{
				SubFromVector(&w, &v);
				ScaleVector(&w, 0.5);
				AddToVector(&w, &v);

				//DrawALine(v.v[X], v.v[Y], w.v[X], w.v[Y], D3DRGBA(1,0,0,shadey?0.5:1.0));
				DrawEditorArrow(v.v[X], v.v[Y], w.v[X], w.v[Y], 0.1, D3DRGBA(1,0,0,shadey?0.5:1.0));
				DrawEditorIcon(w.v[X] - 3, w.v[Y] - 3, 4, shadey);
			}
		}

		EdXfmPoint(&v, &path->tile->centre);
		if (v.v[Z] == 0) continue;
		
		v.v[Y] -= path->level * 16;

		// draw checkered flag for start flag (and I KNOW that's wrong)
		/*if (path->start)
			DrawEditorIcon(v.v[X] - 12, v.v[Y] - 15, 3, shadey);
		else*/
		
		DrawEditorIcon(v.v[X] - 12, v.v[Y] - 15, 2, shadey);
	}
}

void DrawEditor(void)
{
	VECTOR v;
	EDITGROUPNODE *node, *flag;
	CREATEENTITY *create;
	RECT r;
	int i;

	// CREATE_ types
	static const int createicons[5] = { 5, 6, 7, 63, 79 };

	// Draw "X marks the spot"
	if (showX && (clickPos.v[X] != INT_MAX))
	{
		EdXfmPoint(&v, &clickPos);
		if (v.v[Z] != 0)
			DrawEditorIcon(v.v[X] - 8, v.v[Y] - 8, 1, FALSE);
	}

	if (drawLinks || drawVectors || drawNormals || drawDots) DrawAllTiles();

	if (editPath) DrawEditPath(editPath, FALSE);

	// Draw "create entity" flags
	for (node = createList->nodes; node; node = node->link)
	{
		create = (CREATEENTITY*)node->thing;
		EdXfmPoint(&v, &node->pos);
		if (v.v[Z] == 0) continue;

		DrawEditorIcon(v.v[X] - 12, v.v[Y] - 15,
			createicons[create->thing], (create->path != editPath));
		//DrawEditPath(create->path, TRUE);
	}

	if (selectionList)
		for (node = selectionList->nodes; node; node = node->link)
		{
			EdXfmPoint(&v, &node->pos);
			if (v.v[Z] != 0)
				DrawEditorIcon(v.v[X] - 8, v.v[Y] - 8, 62, FALSE);

			// Mark all affected tiles of a camera case
			if( ((CREATEENTITY *)node->thing)->thing == CREATE_CAMERACASE )
			{
				for( flag = ((CREATEENTITY *)node->thing)->group->nodes; flag; flag = flag->link )
				{
					EdXfmPoint(&v, &flag->pos);
					if (v.v[Z] != 0)
						DrawEditorIcon(v.v[X] - 8, v.v[Y] - 8, 89, FALSE);
				}
			}
		}

	DrawBatchedPolys();
	BlankFrame(pasta_aribiatta);

	DrawStatusInfo();

	if (selectRect.valid)
	{
		r.left = selectRect.a.x;
		r.right = selectRect.b.x;
		r.top = selectRect.a.y;
		r.bottom = selectRect.b.y;
		DrawFlatRect(r, D3DRGBA(sin(womble)*0.5+0.5, 0, cos(womble)*0.5+0.5, 0.5));
		womble+=0.2;
	}

	if (!winMode) DrawMouse();
}


/*	--------------------------------------------------------------------------------
	Function		: DrawStatusInfo
	Parameters		: 
	Returns			: 
*/

void DrawStatusInfo(void)
{
	HRESULT res;
	HDC hdc;
	int i, icon, flag, command;
	POINT p;
	RECT r;

	DrawFlatRect(statusRect, D3DRGBA(0, 0, 0.1, 0.5));
	DrawFlatRect(borderRect, D3DRGBA(0, 0, 0.1, 0.7));
	DrawFlatRect(toolbarRect, D3DRGBA(0, 0, 0.1, 0.7));

	res = IDirectDrawSurface4_GetDC(surface[RENDER_SRF], &hdc);
	if (res == DD_OK)
	{
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255,255,255));
		TextOut(hdc, statusRect.left+5, statusRect.top+5, statusWin.text, strlen(statusWin.text));
		TextOut(hdc, statusRect.left+5, statusRect.top+20, statusWin.currentSelection, strlen(statusWin.currentSelection));
		TextOut(hdc, statusRect.left+5, toolbarRect.bottom-20, statusMessage, strlen(statusMessage));

		//GetCursorPos(&p);
		p = mousePos;

		if (p.y > statusRect.top)
		{
			command = ToolbarMouseOver(p.x / 16 + ((p.y - toolbarRect.top) / 18) * 100);

			SetTextAlign( hdc, TA_RIGHT );
	
			if( command != -1 )
				TextOut( hdc, statusRect.right,
						toolbarRect.bottom-20, 
						tooltips[command],
						strlen(tooltips[command]) );
			else
				TextOut( hdc, statusRect.right,
						toolbarRect.bottom-20, 
						"Press H for help",
						16 );
		}

		if( helpScreen )
		{
			int half;
			for( i=0; HELPSTRINGS[i][0] != '\n'; i++ );

			half = (i/2)+1;
			r.left = 120;
			r.right = 520;
			r.top = 20;
			r.bottom = half*22;
			SetTextAlign( hdc, TA_LEFT );
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, RGB(255,255,255));
			for( --i; (i+1); i-- ) // Oh yeah baby!
				TextOut(hdc, (i<half)?r.left+5:r.left+((r.right-r.left)/2)+5,
						(i<half)?r.top+5+(i*20):r.top+5+((i-half)*20),
						HELPSTRINGS[i],
						strlen(HELPSTRINGS[i]));
		}

		IDirectDrawSurface4_ReleaseDC(surface[RENDER_SRF], hdc);
	}
	else
	{
		dprintf"Error getting surface DC: %d", res));
		return;
	}


	for (i=0; TOOLBARBUTTONS[i].command != -1; i++) {
		icon = TOOLBARBUTTONS[i].icon;
		if (icon == -1) continue;
		if( TOOLBARBUTTONS[i].command == TB_PASTE_SELECTION ) // Euuww, kludge!
			DrawEditorIcon(2 + i*16, toolbarRect.top + 2, icon, !copyFlag);
		else
			DrawEditorIcon(2 + i*16, toolbarRect.top + 2, icon,
				(selectedIcon != TOOLBARBUTTONS[i].command));
	}

	if (submenu)
	{
		for (i=0; submenu[i].command != -1; i++) {
			icon = submenu[i].icon;
			if (icon == -1)
				continue; // skip seperators
			else if( (icon & 0x8000) || (submenu[i].command == TB_CAMERACASE_ADDTILES && addTiles) )
				DrawEditorIcon(2 + i*16, toolbarRect.top+20, icon & 0x7fff, FALSE);
			else
				DrawEditorIcon(2 + i*16, toolbarRect.top+20, icon, TRUE);
		}
	}

/*
	if (editMode == EDIT_PLACE_ENEMY || editMode == EDIT_PLACE_PLATFORM)
	{
		DrawEditorIcon(2, toolbarRect.top+20, 32, FALSE);
		for (icon=1; icon<8; icon++, flag) {
			DrawEditorIcon(2 + icon*16, toolbarRect.top+20, icon+32,
				(createFlags & commonFlagIcons[icon-1]) == 0);
		}

		if (editMode == EDIT_PLACE_PLATFORM)
		{
			for (icon=0; icon<8; icon++, flag) {
				DrawEditorIcon(2 + 8 * 16 + icon*16, toolbarRect.top+20, icon+40,
					(createFlags & platformFlagIcons[icon]) == 0);
			}
		}
	}
*/
}


/*	--------------------------------------------------------------------------------
	Function		: FindSomething
	Parameters		: 
	Returns			: 
*/

CREATEENTITY *FindCreateNode(int x, int y)
{
	EDITGROUPNODE *node, *closest;
	VECTOR v;
	int dist, closestDist;

	closest = NULL; closestDist = 0x10000;

	for (node = createList->nodes; node; node = node->link)
	{
		EdXfmPoint (&v, &node->pos);
		if (!v.v[Z]) continue;

		v.v[X] -= x; v.v[Y] -= y; v.v[Z] = 0;
		
		dist = MagnitudeSquared(&v);

		if (dist < closestDist)
		{
			closest = node;
			closestDist = dist;
		}
	}

	return (CREATEENTITY*)closest->thing;
}	

/*	--------------------------------------------------------------------------------
	Function		: FindFlag
	Parameters		: 
	Returns			: 
*/

EDITPATHNODE* FindFlag(int x, int y)
{
	EDITPATHNODE *flag, *closest;
	VECTOR v;
	int dist, closestDist;

	if (!editPath) return NULL;

	closest = NULL; closestDist = 0x10000;

	for (flag = editPath->nodes; flag; flag = flag->link)
	{
		EdXfmPoint (&v, &flag->tile->centre);
		if (!v.v[Z]) continue;

		v.v[X] -= x; v.v[Y] -= y; v.v[Z] = 0;
		
		dist = MagnitudeSquared(&v);

		if (dist < closestDist)
		{
			closest = flag;
			closestDist = dist;
		}
	}

	return closest;
}

EDITPATHNODE* FindFlagOnTile(GAMETILE *tile)
{
	EDITPATHNODE *flag;

	if( !editPath )
		return NULL;

	for (flag = editPath->nodes; flag; flag = flag->link)
		if (flag->tile == tile)	return flag;

	return NULL;
}

/*	--------------------------------------------------------------------------------
	Function		: FindGarib
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

GARIB *FindGarib(int x, int y)
{
	GARIB *garib, *closest;
	VECTOR v;
	int dist, closestDist, i;

	closest = NULL; closestDist = 0x10000;

	for( garib=garibCollectableList.head.next, i=garibCollectableList.numEntries; i; garib=garib->next, i--)
	{
		EdXfmPoint(&v, &garib->pos );
		if (!v.v[Z]) continue;

		v.v[X] -= x; v.v[Y] -= y; v.v[Z] = 0;
		
		dist = MagnitudeSquared(&v);

		if (dist < closestDist)
		{
			closest = garib;
			closestDist = dist;
		}
	}

	return closest;
}


/*	--------------------------------------------------------------------------------
	Function		: FindClickedTile
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

GAMETILE *FindClickedTile(int x, int y)
{
	GAMETILE *t, *closest;
	VECTOR v;
	int dist, closestDist;

	closest = NULL; closestDist = 100*100;

	for (t = firstTile; t != NULL; t = t->next)
	{
		EdXfmPoint (&v, &t->centre);
		if (!v.v[Z]) continue;

		v.v[X] -= x; v.v[Y] -= y; //v.v[Z] = 0;
		
		dist = v.v[0]*v.v[0] + v.v[1]*v.v[1];

		if (dist < closestDist)
		{
			closest = t;
			closestDist = dist;
		}
	}

	return closest;
}

/*	--------------------------------------------------------------------------------
	Function		: EditorUpdateCamera
	Parameters		: 
	Returns			:
	Notes			:

	Legacy code meant that our camera had to be calculated with lookAt... since we've
	actually got a viewing matrix now, this could be updated to use proper rotations
	more control! More power! More spurious expenditure of effort...
*/

void EditorUpdateCamera()
{
	VECTOR v = { 0.0, 0.0, -500.0 };
	VECTOR up = { 0.0, 1.0, 0.0 };

	// Work out point to look at

	//SetVector(&v, &currCamTarget);
	//SubFromVector(&v, &currCamSource);
	RotateVector2DXYZ(&v, &v, editCameraRot.v[X], X);
	RotateVector2DXYZ(&v, &v, editCameraRot.v[Y], Y);
	AddToVector(&v, &currCamSource);

	SetVector(&currCamTarget, &v);

	// Calculate "up" vector

	RotateVector2DXYZ(&up, &up, editCameraRot.v[X], X);
	RotateVector2DXYZ(&up, &up, editCameraRot.v[Y], Y);

	SetVector(&camVect, &up);

	guLookAtF(vMatrix,
		currCamTarget.v[X],currCamTarget.v[Y],currCamTarget.v[Z],
		currCamSource.v[X],currCamSource.v[Y],currCamSource.v[Z],
		camVect.v[X],camVect.v[Y],camVect.v[Z]);	
}

void GetEditorCameraRotations(void)
{
	VECTOR v;
	float foo, bar;

	SubVector(&v, &currCamSource, &currCamTarget);

	bar = atan2(v.v[X], v.v[Z]);
	RotateVector2DXYZ(&v, &v, -bar, Y);
	foo = -atan2(v.v[Y], v.v[Z]);

	editCameraRot.v[X] = foo;
	editCameraRot.v[Y] = bar;
}


/*	--------------------------------------------------------------------------------
	Function		: EditorKeypress
	Parameters		: 
	Returns			: 
*/

void EditorKeypress(char c)
{
	char *p;
	int i;
	double d;
	EDITVARFUNC editFunc;

	if (editText.active)
	{
		p = editText.string;
		if (c == 8) // backspace
		{
			if (!*p) 
			{
				editText.active = 0;
				UpdateStatusString( );
				return;
			}
			while (*p) p++;
			*(p - 1) = 0;
		}
		else if (c == 13) // enter
		{
			switch (editText.type)
			{
			case EDITVAR_INT:
				*(int *)editText.variable = atoi(editText.string);
				sprintf(statusMessage, "Set %s to %d", editText.var, *(int*)editText.variable);
				break;

			case EDITVAR_STRING:
				strcpy(editText.variable, editText.string);
				sprintf(statusMessage, "Set %s to '%s'", editText.var, editText.string);
				break;

			case EDITVAR_FLOAT:
				d = atof(editText.string);
				*(float *)editText.variable = d;
				sprintf(statusMessage, "Set %s to %f", editText.var, *(float*)editText.variable);
				break;

			case EDITVAR_PASSTOFUNCTION:
				editFunc = (EDITVARFUNC)editText.variable;
				editFunc(editText.string);
				//FUNC_SetPathSpeed(editText.string);
				break;
			}
			editText.active = FALSE;
		}
		else if (c == 27)
		{
			editText.active = FALSE;
		}
		else
		{
			for (i = 0; *p; p++, i++); // count elements & find last char
			if (i < editText.charLimit)
			{
				*(p++) = c;
				*p = 0;
			}
		}
		UpdateStatusString();
	}
	else
	{
		if( c == 8 ) // Delete keys - remove selected, then clear flag path
		{
			if( selectionList )
				ToolbarSelect( TB_DELETE_SELECTED );
			else
				ToolbarSelect( TB_CLEARPATH );
		}
		else if ( c == 27 && cameraMode )
		{
			cameraMode = FALSE;
			UpdateStatusString();
		}
		else
		{
			for( i=0; EDKEYMAP[i].command != -1; i++ )
				if( c == EDKEYMAP[i].key )
				{
					ToolbarSelect(EDKEYMAP[i].command);
					return;
				}
			/*
			for (i = 0, p = EDITOR_SHORTCUTS_A; *p; p++, i++)
			{
				if (!TOOLBARBUTTONS[i].command) i++;
				if (*p == c) { ToolbarMouseDown(i); return; }
			}
			if (submenu)
				for (i = 100, p = EDITOR_SHORTCUTS_B; *p; p++, i++)
				{
					if (!submenu[i-100].command) i++;
					if (*p == c) { ToolbarMouseDown(i);	return; }
				}
				*/
		}
	}
}

