const char* TILESTATESTRINGS[NUMTILESTATES] = {
	"Normal",	//	TILESTATE_NORMAL,
	"Deadly",	//	TILESTATE_DEADLY,
	"Sink",		//	TILESTATE_SINK,
	"Ice",		//	TILESTATE_ICE,
	"Superhop",	//	TILESTATE_SUPERHOP,
	"Join",		//	TILESTATE_JOIN,
	"Spring",	//	TILESTATE_SPRING,
	"Teleport",	//	TILESTATE_TELEPORTER,
	"Grapple",	//	TILESTATE_GRAPPLE,
	"Smash",	//	TILESTATE_SMASH
	"Barred"	//	TILESTATE_BARRED
};

const char* GARIBTYPESTRINGS[] = {
	"SPAWN_GARIB",			//0
	"EXTRAHEALTH_GARIB",	//1
	"EXTRALIFE_GARIB",		//2
	"AUTOHOP_GARIB",		//3
	"LONGHOP_GARIB",		//4
	"LONGTONGUE_GARIB",		//5
	"WHOLEKEY_GARIB",		//6
	"HALFLKEY_GARIB",		//7
	"HALFRKEY_GARIB",		//8
};

/* -------------------------------------------------------------------------

	Global editor variables (lots of them)

   ------------------------------------------------------------------------- */

EDITPATH *editPath = NULL;
EDITPATH *pathList = NULL;
EDITGROUP *createList = NULL;
EDITTEXT editText;
BOOL	drawLinks = FALSE;
BOOL	drawNormals = FALSE;
BOOL	drawVectors = FALSE;
BOOL	drawDots = FALSE;
BOOL	invertMouse = FALSE; // getting like Quake here
float	HorizontalWhizziness = 6.0;
float	InOutWhizziness = 12.0; // for heaven's sake
BOOL	clicking = FALSE;
BOOL	showX = FALSE;
VECTOR	clickPos;
POINT	oldMousePos;
short	selectedIcon = -1, garibType = 0;
int		createFlags = ENEMY_NEW_FORWARDS;

EDITMODE editMode = EDIT_NONE;
GAMETILE *editTile = NULL;
//CREATEENTITY *editCreate = NULL;
EDITPATHNODE *editFlag = NULL;
GARIB *editGarib = NULL;
VECTOR editCameraRot = { 0.0, 0.0, 0.0 };

TOOLBAR_SUBMENU submenu = NULL;

char savePath[80] = EDITOR_SAVE_BASE;

SELECTFUNC selectFunc = NULL;
EDITGROUP *selectionList = NULL;

char createTypes[2][MAXTYPELENGTH] = { "wasp.ndo", "pltlilly.ndo" };
