/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: path.c
	Programmer	: Andrew Eder
	Date		: 16/3/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI

#include <ultra64.h>

#include "incs.h"



/*	--------------------------------------------------------------------------------
	Function		: GetPositionForPathNode
	Purpose			: gets position for specified path node
	Parameters		: VECTOR *,PATHNODE *
	Returns			: void
	Info			: 
*/
void GetPositionForPathNode(VECTOR *vecPos,PATHNODE *pNode)
{
	vecPos->v[X] = pNode->worldTile->centre.v[X] + (pNode->worldTile->normal.v[X] * pNode->offset);
	vecPos->v[Y] = pNode->worldTile->centre.v[Y] + (pNode->worldTile->normal.v[Y] * pNode->offset);
	vecPos->v[Z] = pNode->worldTile->centre.v[Z] + (pNode->worldTile->normal.v[Z] * pNode->offset);
}


/*	--------------------------------------------------------------------------------
	Function		: GetPositionForPathNodeOffset2
	Purpose			: gets position for specified path node using offset 2
	Parameters		: VECTOR *,PATHNODE *
	Returns			: void
	Info			: 
*/
void GetPositionForPathNodeOffset2(VECTOR *vecPos,PATHNODE *pNode)
{
	vecPos->v[X] = pNode->worldTile->centre.v[X] + (pNode->worldTile->normal.v[X] * pNode->offset2);
	vecPos->v[Y] = pNode->worldTile->centre.v[Y] + (pNode->worldTile->normal.v[Y] * pNode->offset2);
	vecPos->v[Z] = pNode->worldTile->centre.v[Z] + (pNode->worldTile->normal.v[Z] * pNode->offset2);
}
