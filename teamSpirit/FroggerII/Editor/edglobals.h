#ifndef _EDGLOBALS_H_INCLUDED
#define _EDGLOBALS_H_INCLUDED

/*	-------------------------------------------------------------------------------------------
	Globals
*/

#define NUMTILESTATES 11

extern const char* TILESTATESTRINGS[NUMTILESTATES];
extern const char* GARIBTYPESTRINGS[];

extern EDITPATH *editPath;
extern EDITPATH *pathList;
extern EDITGROUP *createList;
extern BOOL		drawLinks;
extern BOOL		drawNormals;
extern BOOL		drawVectors;
extern BOOL		drawDots;
extern BOOL		invertMouse;
extern float	HorizontalWhizziness;
extern float	InOutWhizziness;
extern BOOL		clicking;
extern BOOL		showX;
extern VECTOR	clickPos;
extern POINT	oldMousePos;
extern short	selectedIcon;
extern short	garibType;
extern int		createFlags;
extern EDITMODE editMode;
extern GAMETILE *editTile;
extern EDITPATHNODE *editFlag;
extern GARIB	*editGarib;
extern VECTOR	editCameraRot;
extern TOOLBAR_SUBMENU submenu;
extern char	savePath[80];
extern SELECTFUNC selectFunc;
extern EDITGROUP *selectionList;
extern EDITTEXT editText;
extern char createTypes[][MAXTYPELENGTH];


#endif