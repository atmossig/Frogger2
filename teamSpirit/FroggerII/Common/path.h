/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: path.h
	Programmer	: Andrew Eder
	Date		: 16/3/99

----------------------------------------------------------------------------------------------- */

#ifndef PATH_H_INCLUDED
#define PATH_H_INCLUDED


#define NODE_RANGETHRESHOLD				10.0F


#define GET_PATHLASTNODE(x)		(x->numNodes - 1)


//----- [ PATH FLAGS ] -------------------------------------------------------------------------//

#define PATH_MAKENODETILEPTRS			(1 << 0)



//----- [ PATHNODE DATA STRUCTURE ] -----//

typedef struct TAGPATHNODE
{
	GAMETILE				*worldTile;				// ptr to underlying world tile
	float					offset;	 				// offset(1) from underlying world tile
	float					offset2;	 			// offset(2) from underlying world tile

} PATHNODE;

//----- [ PATH DATA STRUCTURE ] -----//

typedef struct TAGPATH
{
	int						numNodes;				// number of nodes in path
	int						fromNode;				// from node
	int						toNode;					// node we're heading towards
	int						startNode;				// start node for this path

	PATHNODE				*nodes;					// actual path nodes

} PATH;


//----- [ FUNCTION PROTOTYPES ] -----//

extern void GetPositionForPathNode(VECTOR *vecPos,PATHNODE *pNode);
extern void GetPositionForPathNodeOffset2(VECTOR *vecPos,PATHNODE *pNode);
extern BOOL ActorHasArrivedAtNode(ACTOR2 *pAct,PATH *pPath,int nodeID);

#endif
