/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: editdefs.c
	Programmer	: David Swift
	Date		: 08/07/99

----------------------------------------------------------------------------------------------- */


EDITPATHNODE *editPath;


/*	--------------------------------------------------------------------------------
	Function		: EditorAddCreateNode

	Parameters		: const char*, int, PATH*, CREATETYPE
	Returns			: CREATEENTITYNODE*
*/

CREATEENTITYNODE *EditorAddCreateNode(const char *id, int flags, CREATETYPE type, VECTOR pos)
{
	CREATEENTITYNODE *newNode;

	newNode = (CREATEENTITYNODE*)malloc(sizeof(newNode));

	newNode->prev = NULL;
	newNode->next = createList;
	if (createList) createList->prev = newNode;
	createList = newNode;

	strncpy(newNode->type, id, 11); newNode->type[11] = 0;
	newNode->flags = flags;
	
	newNode->path = editPath; editPath = NULL;
	
	newNode->thing = type;
	newNode->iconPos = pos;

	return newNode;
}

void EditorSubCreateNode(CREATEENTITYNODE *node)
{
	CREATEENTITYNODE *next;

	if (node->next) node->next->prev = node->prev;
	if (node->prev) node->prev->next = node->next;
	
	FreeEditPath(node->path);

	free(node);
}

/*	--------------------------------------------------------------------------------
	Function		: EditorPathMake


	Parameters		: 
	Returns			: 
*/

PATH *EditorPathMake()
{
	int count, i;
	EDITPATHNODE *path;
	PATHNODE *node;

	PATH *newPath = (PATH *)JallocAlloc(sizeof(PATH), YES, "epath");

	/* count the number of nodes first */
	for (path = editPath, count = 0; path; path = path->next, count++);

	if (!count) return NULL;

	newPath->numNodes = count;
	newPath->startNode = 0;

	// allocate memory for path nodes
	newPath->nodes = (PATHNODE *)JallocAlloc(sizeof(PATHNODE) * newPath->numNodes,YES,"enode");

	i = count - 1;
	node = newPath->nodes + i;

	for (path = editPath; path; path = path->next, node--, i--)
	{
		node->worldTile = path->tile;
		node->speed		= path->speed;
		node->offset	= path->offset;
		node->offset2	= path->offset2;

		if (path->start) newPath->startNode = i;
	}

	return newPath;
}

/*	--------------------------------------------------------------------------------
	Function		: EditorGetPath
	Parameters		: 
	Returns			: 
*/

void EditorGetPath(PATH *path)
{
	int i;
	EDITPATHNODE* newPath;

	FreeEditPath(editPath); editPath = NULL;

	for (i = 0; i<path->numNodes; i++)
	{
		newPath = (EDITPATHNODE*)malloc(sizeof(EDITPATHNODE));
		newPath->tile = path->nodes[i].worldTile;
		newPath->speed = path->nodes[i].speed;
		newPath->offset = path->nodes[i].offset;
		newPath->offset2 = path->nodes[i].offset2;
		newPath->next = editPath;
		newPath->start = (i == path->startNode) ? TRUE : FALSE;
		editPath = newPath;
	}
}

