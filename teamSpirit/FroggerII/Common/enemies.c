/*

	This file is part of Frogger2, (c) 1997 Interactive Studios Ltd.


	File		: enemies.c
	Programmer	: Andrew Eder
	Date		: 1/12/99

----------------------------------------------------------------------------------------------- */

#define F3DEX_GBI_2

#include <ultra64.h>

#include "incs.h"


ENEMYLIST enemyList;						// the enemy linked list

#define ENEMY_RANDOMNESS (0.5 + (Random(100)/100.0))	// returns a value from 0.5 to 1.5

void NMEDamageFrog( int num, ENEMY *nme );
void RotateWaitingNME( ENEMY *cur );

void UpdatePathNME( ENEMY *cur );
void UpdateSnapper( ENEMY *cur );
void UpdateTileSnapper( ENEMY *cur );
void UpdateVent( ENEMY *cur );
void UpdateMoveVerticalNME( ENEMY *cur );
void UpdateRotatePathNME( ENEMY *cur );
void UpdateHomingNME( ENEMY *cur );
void UpdateMoveOnMoveNME( ENEMY *cur );
void UpdateFlappyThing( ENEMY *cur );
void UpdateFrogWatcher( ENEMY *cur );

/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
ENEMY *GetEnemyFromUID (long uid)
{
	ENEMY *cur,*next;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = cur->next)
		if (cur->uid == uid)
			return cur;

	dprintf"GetEnemyFromUID: Couldn't find enemy %d\n", uid));
	return NULL;
}


void NMEDamageFrog( int num, ENEMY *nme )
{
	if( !nme || num < 0 || num > 2) 
		return;

	frog[num]->action.healthPoints--;
	
	if(frog[num]->action.healthPoints != 0)
	{
		/* 
		Check for NME flags and do different effects
		*/
		CreateAndAddSpecialEffect( FXTYPE_FROGSTUN, &frog[num]->actor->pos, &currTile[num]->normal, 30, 0, 0, 3.0 );
		PlaySample(42,NULL,192,128);
		frog[num]->action.safe = 25;
	}
	else
	{
		/* 
		Check for NME flags and do different effects
		*/
		PlaySample(110,NULL,192,128);
		AnimateActor(frog[num]->actor,FROG_ANIM_TRYTOFLY,NO,NO,0.5F,0,0);
		frog[num]->action.dead = 50;
		frog[num]->action.healthPoints = 3;
		frog[num]->action.deathBy = DEATHBY_NORMAL;
		player[num].frogState |= FROGSTATUS_ISDEAD;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemies
	Purpose			: updates all enemies and their states / movements, etc.
	Parameters		: 
	Returns			: void
	Info			: 
*/
void UpdateEnemies()
{
	ENEMY *cur,*next;
	float fxDist;
	long i;

	if(enemyList.numEntries == 0)
		return;

	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		// check if enemy is active
		if(!cur->active)
			continue;

		// Set point of interest for frog
		if( cur->nmeActor->distanceFromFrog < pOIDistance )
		{
			VECTOR moveVec;
			float angle;
			SubVector( &moveVec, &cur->nmeActor->actor->pos, &frog[0]->actor->pos );
			MakeUnit( &moveVec );
			angle = acos(DotProduct( &currTile[0]->dirVector[frogFacing[0]], &moveVec ));
			if( angle < 0.7 )
			{
				pOIDistance = cur->nmeActor->distanceFromFrog;
				pointOfInterest = &cur->nmeActor->actor->pos;
			}
		}

		// check if this enemy is currently 'waiting' at a node
		if(cur->isWaiting)
		{
			if(cur->isWaiting == -1)
				continue;

			if(actFrameCount > cur->path->startFrame)
				cur->isWaiting = 0;
			else
			{
				// if enemy is following a path, do a slerp so it'll rotate nicely
				// (except the way I do it is a bit poo - Dave)
				if (cur->flags & ENEMY_NEW_FOLLOWPATH && !(cur->flags & ENEMY_NEW_FACEFORWARDS))
					RotateWaitingNME( cur );

				continue;
			}
		}

		// Do update functions for individual enemy types
		if (cur->Update)
			cur->Update(cur);

		// Single and multiplayer damage to frog
		if( frameCount > 10 )
		{
			if (NUM_FROGS==1)
			{
				if( (cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION) && !frog[0]->action.dead && 
					!frog[0]->action.safe && ActorsHaveCollided(frog[0],cur->nmeActor) )
				{
					NMEDamageFrog(0,cur);
				}
				else if( (currTile[0] == cur->inTile) && !frog[0]->action.dead && !frog[0]->action.safe &&
						(!(player[0].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
						!currPlatform[0] && !(player[0].frogState & FROGSTATUS_ISFLOATING) && !(cur->flags & ENEMY_NEW_NODAMAGE) )
				{
					NMEDamageFrog(0,cur);
				}
			}
			else
			{
				for (i=0; i<NUM_FROGS; i++)
					if( (cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION) && !frog[i]->action.safe &&
						ActorsHaveCollided(frog[i],cur->nmeActor) )
					{
						KillMPFrog(i);
					}
					else if( (currTile[i] == cur->inTile) && !frog[i]->action.safe && 
							(!(player[i].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
							!(player[i].frogState & FROGSTATUS_ISFLOATING))
					{
						KillMPFrog(i);
					}
			}
		}

		// Do Special Effects attached to enemies
		if( cur->nmeActor->effects & EF_RIPPLE_RINGS )
		{
			long r;
			VECTOR rPos;

			if( cur->nmeActor->value1 )
				r = Random(cur->nmeActor->value1)+1;
			else
				r = 10;

			SetVector( &rPos, &cur->nmeActor->actor->pos );
			rPos.v[Y] = cur->inTile->centre.v[Y];

			fxDist = DistanceBetweenPointsSquared(&frog[0]->actor->pos,&cur->nmeActor->actor->pos);

			if( !(actFrameCount%r) && (fxDist < ACTOR_DRAWDISTANCEINNER))
			{
				if( cur->flags & ENEMY_NEW_FOLLOWPATH ) // More of a wake effect when moving
					CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &cur->currNormal, 30, 1/cur->speed, 0.3, 2 );
				else // Gentle ripples
					CreateAndAddSpecialEffect( FXTYPE_BASICRING, &rPos, &cur->currNormal, 50, 1, 0.1, 3 );
			}
		}
		if( cur->nmeActor->effects & EF_SMOKE_CLOUDS )
		{
			long r;
			if( cur->nmeActor->value1 )
				r = Random(cur->nmeActor->value1)+1;
			else
				r = 10;

			if( !(actFrameCount%r) && (fxDist < ACTOR_DRAWDISTANCEINNER))
				CreateAndAddSpecialEffect( FXTYPE_EXHAUSTSMOKE, &cur->nmeActor->actor->pos, &cur->currNormal, 64, 0, 0, 1.5 );
		}
		if( cur->nmeActor->effects & EF_SPARK_BURSTS )
		{
			long r;
			if( cur->nmeActor->value1 )
				r = Random(cur->nmeActor->value1)+1;
			else
				r = 10;

			if( !(actFrameCount%r) && (fxDist < ACTOR_DRAWDISTANCEINNER))
				CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->nmeActor->actor->pos, &cur->currNormal, 50, 4, 0, 2 );
		}
	}
}

void RotateWaitingNME( ENEMY *cur )
{
	QUATERNION q, res;
	float t;
	VECTOR fromPosition, toPosition, fwd;
	long start_t, end_t, time;
	
	end_t = cur->path->startFrame;
	time = cur->isWaiting*waitScale;
	start_t = end_t - time;

	t = 1.0 - (float)(actFrameCount-start_t)/(float)(time);

	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	MakeUnit( &fwd );
	Orientate(&q,&fwd,&inVec,&cur->currNormal);

	QuatSlerp(&q, &cur->nmeActor->actor->qRot, t, &res);

	SetQuaternion(&cur->nmeActor->actor->qRot, &res);
}

/*	--------------------------------------------------------------------------------
	Function		: UpdatePathNME
	Purpose			: update enemies that move along a path
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdatePathNME( ENEMY *cur )
{
	VECTOR fromPosition,toPosition, fwd, moveVec;
	float length;
	
	// first, update the enemy position
	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	
	SubVector(&fwd,&toPosition,&fromPosition);
	
	length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
	
	ScaleVector(&fwd,length);
	AddVector(&cur->nmeActor->actor->pos,&fwd,&fromPosition);
	MakeUnit (&fwd);

	AddToVector(&cur->currNormal,&cur->deltaNormal);

	if (!(cur->flags & ENEMY_NEW_FACEFORWARDS))
	{
		Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&cur->currNormal);
	}
	else
	{
		SubVector( &moveVec, &cur->path->nodes[cur->path->startNode+1].worldTile->centre, &cur->path->nodes[cur->path->startNode].worldTile->centre );
		if (cur->flags & ENEMY_NEW_BACKWARDS) ScaleVector (&fwd,-1);
		MakeUnit( &moveVec );
		Orientate(&cur->nmeActor->actor->qRot,&moveVec,&inVec,&cur->currNormal);
	}

	// check if this enemy has arrived at a path node
	if( actFrameCount > cur->path->endFrame )
	{
		UpdateEnemyPathNodes(cur);

		cur->path->startFrame = cur->path->endFrame + cur->isWaiting * waitScale;

		if (cur->flags & ENEMY_NEW_RANDOMSPEED)
			cur->path->endFrame = cur->path->startFrame + (60*((float)Random(100)/100.0F) );
		else
			cur->path->endFrame = cur->path->startFrame + (60*cur->speed);
	}

	/*	------------- Push blocks, lovely blocks that push --------------- */

	if (cur->flags & ENEMY_NEW_PUSHESFROG)
	{
		//GAMETILE *tile = cur->path->nodes[cur->path->toNode].worldTile;

		VECTOR v;
		AddVector(&v, &cur->nmeActor->actor->pos, &fwd);
		SubFromVector(&v, &frog[0]->actor->pos);

		if (MagnitudeSquared(&v) < 1000 && player[0].canJump)
		//if (tile == currTile[0])
		{
			PushFrog(&cur->nmeActor->actor->pos, &fwd, 0);
			player[0].canJump = 0;
			AnimateActor(frog[0]->actor, FROG_ANIM_FORWARDSOMERSAULT, NO, NO, 0.6f, 0, 0);

/*
			if (cur->path->toNode < (cur->path->numNodes - 1))
				tile = cur->path->nodes[cur->path->toNode+1].worldTile;

			currTile[0] = destTile[0] = tile;
			SetVector( &frog[0]->actor->pos, &tile->centre);
*/
		}
	}

	GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
	GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);

	length = DistanceBetweenPoints(&fromPosition,&toPosition) / 2.0F;

	if(DistanceBetweenPointsSquared(&fromPosition,&cur->nmeActor->actor->pos) < (length * length))
		cur->inTile = cur->path->nodes[cur->path->fromNode].worldTile;
	else
		cur->inTile = cur->path->nodes[cur->path->toNode].worldTile;
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateSnapper
	Purpose			: Snap at the frog
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateSnapper( ENEMY *cur )
{
	QUATERNION q1, q2;
	ACTOR *act = cur->nmeActor->actor;
	PATH *path = cur->path;
	float speed;

	// Idle animations
	switch( cur->isSnapping )
	{
	case 0:
		if( actFrameCount < path->endFrame )
			break;

		if (Random(1000)>980)
			AnimateActor(act,2,NO,NO,cur->nmeActor->animSpeed, 0, 0);
	
		if (Random(1000)>950)
		{
			if (Random(1000)>950)
				AnimateActor(act,3,NO,YES,cur->nmeActor->animSpeed, 0, 0);
			else
				AnimateActor(act,0,NO,YES,cur->nmeActor->animSpeed, 0, 0);
		}

		// Slerp orientation towards frog
		SetQuaternion( &q1, &act->qRot );
		ActorLookAt( act, &frog[0]->actor->pos, LOOKAT_2D );
		SetQuaternion( &q2, &act->qRot );
		speed = path->nodes[0].speed * gameSpeed;
		if( speed > 0.999 ) speed = 0.999;
		QuatSlerp( &q1, &q2, speed, &act->qRot );

		// If the snapper has just spotted the frog, set snap time
		if( cur->nmeActor->distanceFromFrog < 5625 ) // 75*75
		{
			SetVector( &act->rotaim, &currTile[0]->centre );

			SetQuaternion( &q1, &act->qRot );
			ActorLookAt( act, &act->rotaim, LOOKAT_2D );
			SetQuaternion( &q2, &act->qRot );
			QuatSlerp( &q1, &q2, speed, &act->qRot );

			path->startFrame = actFrameCount;
			path->endFrame = path->startFrame + (path->nodes[0].waitTime * waitScale);

			cur->isSnapping = 2;
		}
		break;

	case 1: // Time to snap and hope
		if( actFrameCount < path->endFrame )
			break;

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (path->nodes[0].waitTime * waitScale);
		cur->isSnapping = 0;

		if( (DistanceBetweenPointsSquared(&act->rotaim,&frog[0]->actor->pos)<900) && 
			!frog[0]->action.dead && !frog[0]->action.safe ) // If frog is within hitting distance
		{
			NMEDamageFrog(0,cur);
		}
		break;

	case 2: 				// Snap animation
		if( (actFrameCount-path->startFrame) < 0.8*(path->endFrame-path->startFrame) )
			break;

		AnimateActor(act,1,NO,NO,cur->nmeActor->animSpeed, 0, 0);
		cur->isSnapping = 1;
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateTileSnapper
	Purpose			: Snap at a sequence of tiles
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateTileSnapper( ENEMY *cur )
{
	QUATERNION q1, q2;
	VECTOR toPosition;
	float speed;

	switch( cur->isSnapping )
	{
	case 0:
		if( actFrameCount < cur->path->endFrame )
			break;

		if(cur->path->toNode >= GET_PATHLASTNODE(cur->path))	// reached end of path nodes
		{
			cur->path->fromNode	= GET_PATHLASTNODE(cur->path);
			cur->path->toNode	= 1; // Don't snap at the tile we're sitting on
		}
		else
		{
			cur->path->fromNode = cur->path->toNode;
			cur->path->toNode++;
		}

		cur->path->startFrame = actFrameCount;
		cur->path->endFrame = cur->path->startFrame + (cur->path->nodes[cur->path->fromNode].waitTime * waitScale);
		cur->isSnapping = 2;
		break;

	case 1:
		if( actFrameCount < cur->path->endFrame )
			break;

		cur->path->startFrame = actFrameCount;
		cur->path->endFrame = cur->path->startFrame + (cur->path->nodes[cur->path->fromNode].waitTime * waitScale);
		cur->isSnapping = 0;

		// If the frog is on our current target tile
		if( (cur->path->nodes[cur->path->fromNode].worldTile == currTile[0]) && (!frog[0]->action.dead) && (!frog[0]->action.safe) )//FindNearestTile(frog[0]->actor->pos) )
			NMEDamageFrog(0,cur);
		break;

	case 2:
		if( (actFrameCount-cur->path->startFrame) < 0.8*(cur->path->endFrame-cur->path->startFrame) )
		{
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->fromNode]);
			SetQuaternion( &q1, &cur->nmeActor->actor->qRot );
			ActorLookAt( cur->nmeActor->actor, &toPosition, LOOKAT_2D );
			SetQuaternion( &q2, &cur->nmeActor->actor->qRot );
			speed = cur->path->nodes[0].speed * gameSpeed;
			if( speed > 0.999 ) speed = 0.999;
			QuatSlerp( &q1, &q2, speed, &cur->nmeActor->actor->qRot );
			break;
		}

		AnimateActor(cur->nmeActor->actor,1,NO,NO,cur->nmeActor->animSpeed, 0, 0);
		cur->isSnapping = 1;
		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateVent
	Purpose			: Blow hurt at the frog
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateVent( ENEMY *cur )
{
	PLANE2 rebound;
	PATH *path = cur->path;
	unsigned long i;

	switch( cur->isSnapping )
	{
	case -2:
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*cur->nmeActor->radius);

		cur->isSnapping = -1;
		break;

	case -1:
		if( actFrameCount > path->endFrame )
			cur->isSnapping = 0;

		break;

	case 0: // Start timer
		// Delay until fire
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*path->nodes[0].waitTime);

		cur->isSnapping = 1;
		break;

	case 1:
		if( actFrameCount < path->endFrame )
			break;

		// Delay until stop
		path->startFrame = actFrameCount;
		path->endFrame = actFrameCount + (60*path->nodes[0].speed);

		cur->isSnapping = 2;
		break;

	case 2:
		// Stop firing on timeout, and reset
		if( actFrameCount > path->endFrame )
		{
			cur->isSnapping = 0;
			break;
		}

		// Create fx
		/* TODO: Options for different types of effects specified in the effects thing. Need effects first...
		if( cur->nmeActor->effects & EF_FLAMEBURST_UP )
		{

		}
		else if...
		*/

		CreateAndAddSpecialEffect( FXTYPE_SMOKEBURST, &cur->nmeActor->actor->pos, &cur->path->nodes->worldTile->normal, 64, cur->nmeActor->animSpeed*path->numNodes, 0, cur->nmeActor->value1*path->numNodes );

		// Check for collision with frog, and do damage
		for( i=0; i < path->numNodes; i++ )
			if( (path->nodes[i].worldTile == currTile[0]) && (!frog[0]->action.dead) && (!frog[0]->action.safe) )
				NMEDamageFrog(0,cur);

		break;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateMoveVerticalNME
	Purpose			: Enemy moves up and down
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateMoveVerticalNME( ENEMY *cur )
{
	VECTOR moveVec;

	// get up vector for this enemy
	SetVector(&moveVec,&cur->path->nodes[0].worldTile->normal);

	moveVec.v[X] *= cur->speed;
	moveVec.v[Y] *= cur->speed;
	moveVec.v[Z] *= cur->speed;

	// check if this enemy is moving up or down
	if(cur->flags & ENEMY_NEW_MOVEUP)
	{
		// enemy is moving up
		AddToVector(&cur->nmeActor->actor->pos,&moveVec);
	}
	else if(cur->flags & ENEMY_NEW_MOVEDOWN)
	{
		// enemy is moving down
		SubFromVector(&cur->nmeActor->actor->pos,&moveVec);
	}

	if(EnemyReachedTopOrBottomPoint(cur))
	{
		UpdateEnemyPathNodes(cur);
		cur->path->startFrame = cur->path->endFrame + cur->isWaiting * waitScale;
		cur->path->endFrame = cur->path->startFrame + (60*cur->speed);
	}
}

/*	--------------------------------------------------------------------------------
	Function		: UpdateRotatePathNME
	Purpose			: Enemy that flies around a point
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateRotatePathNME( ENEMY *cur )
{
	float length;
	VECTOR toPosition;

	length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
	length *= PI2;
	length += cur->path->nodes->offset2 * PI2; // Fraction of circumference it starts at

	if( cur->speed < 0 )
		length = PI2 - length;

	if( cur->flags & ENEMY_NEW_ROTATEPATH_XZ )
	{
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * cosf(length));
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + cur->path->nodes->offset;
	}
	else if( cur->flags & ENEMY_NEW_ROTATEPATH_XY )
	{
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z];
	}
	else if( cur->flags & ENEMY_NEW_ROTATEPATH_ZY )
	{
		toPosition.v[Y] = cur->path->nodes->worldTile->centre.v[Y] + (cur->nmeActor->radius * cosf(length)) + cur->path->nodes->offset;
		toPosition.v[Z] = cur->path->nodes->worldTile->centre.v[Z] + (cur->nmeActor->radius * sinf(length));
		toPosition.v[X] = cur->path->nodes->worldTile->centre.v[X];
	}

	if( !(cur->flags & ENEMY_NEW_FACEFORWARDS) ) // Look in direction of travel
		ActorLookAt( cur->nmeActor->actor, &toPosition, LOOKAT_ANYWHERE );

	SetVector( &cur->nmeActor->actor->pos, &toPosition );

	if( actFrameCount > cur->path->endFrame )
	{
		cur->path->startFrame = cur->path->endFrame;
		cur->path->endFrame = cur->path->startFrame+(60*Fabs(cur->speed));
	}

	cur->inTile = FindNearestJoinedTile( cur->inTile, &cur->nmeActor->actor->pos );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateHomingNME
	Purpose			: Enemy moves towards frog
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateHomingNME( ENEMY *cur )
{
	GAMETILE *chTile;
	VECTOR nmeup, tVec, moveVec;
	float distance=10000, best=-2;
	short bFlag = 0;
	unsigned long i;

	SubVector( &moveVec, &frog[0]->actor->pos, &cur->nmeActor->actor->pos );
	MakeUnit( &moveVec );
	chTile = FindNearestTile( cur->nmeActor->actor->pos );

	// Do check for close direction vector from tile. If none match closely, do not move.
	for( i=0; i<4; i++ )
		if( chTile->tilePtrs[i] )
		{
			// Direction to tile
			SubVector( &tVec, &chTile->tilePtrs[i]->centre, &chTile->centre );
			MakeUnit( &tVec );
			// Cosine of angle between vectors
			distance = DotProduct(&tVec,&moveVec);
			if( distance > best )
				best = distance;
		}
		else
			bFlag = 1; // There is some invalid tile

	// If the best direction match is close enough we can carry on (approx 45 degrees)
	// Also check that we're over a tile.
	if( best > 0.7 )
	{
		ScaleVector( &moveVec, cur->speed );
		AddVector( &tVec, &moveVec, &cur->nmeActor->actor->pos );
		chTile = FindNearestTile( tVec );
		if( bFlag )
		{
			SubVector( &moveVec, &chTile->centre, &tVec );
			distance = abs(Magnitude( &moveVec ));
		}
		if( distance < 35 || !bFlag )
			cur->nmeActor->actor->pos = tVec;
	}
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateMoveOnMoveNME
	Purpose			: Move when frog has moved
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateMoveOnMoveNME( ENEMY *cur )
{
	VECTOR frogVec, up, fwd;
	PATH *path = cur->path;
	float length;

	if( path->numNodes < 3 )
		return;

	if( cur->nmeActor->distanceFromFrog > (path->nodes[0].waitTime*path->nodes[0].waitTime) )
	{
		cur->isIdle = 0;
		path->nodes[2].worldTile = NULL;
		return;
	}

	// Check if the enemy has 3 path nodes. Allocate if not ( first time through )
	if( !cur->isSnapping )
	{
		path->nodes[1].worldTile = NULL;
		path->nodes[2].worldTile = path->nodes[0].worldTile;
		cur->isSnapping = 1; // This is a completely random misuse of isSnapping. And I don't care.
	}

	// If frog has moved
	cur->isIdle += player[0].hasJumped;

	// If enemy is on the next path node, set startnode worldtile and the next to zero
	if( path->nodes[2].worldTile )
	{
		if( actFrameCount > path->endFrame )
		{
			path->nodes[1].worldTile = path->nodes[2].worldTile;
			path->nodes[2].worldTile = NULL;
		}
		else if( (actFrameCount-path->startFrame) > (0.5*(float)(path->endFrame-path->startFrame)) )
		{
			cur->inTile = path->nodes[2].worldTile;//FindNearestJoinedTile( cur->inTile, &cur->nmeActor->actor->pos );
		}
	}

	// If we need a new destination tile, find it by the direction to the frog
	if( cur->isIdle && !path->nodes[2].worldTile )
	{
		SubVector( &frogVec, &currTile[0]->centre, &cur->nmeActor->actor->pos ); 
		path->nodes[2].worldTile = FindJoinedTileByDirection( path->nodes[1].worldTile, &frogVec );

		path->startFrame = actFrameCount;
		path->endFrame = path->startFrame + (60*path->nodes[0].speed);
		cur->isIdle--;
	}

	// Move towards next node - third condition is so fwd is not scaled to zero
	if( path->nodes[1].worldTile && path->nodes[2].worldTile && (actFrameCount > path->startFrame) )
	{
		// Move towards frog
		SubVector(&fwd,&path->nodes[2].worldTile->centre,&path->nodes[1].worldTile->centre);
		length = (float)(actFrameCount - path->startFrame)/(float)(path->endFrame - path->startFrame);
		ScaleVector(&fwd,length);
		AddVector(&cur->nmeActor->actor->pos,&fwd,&path->nodes[1].worldTile->centre);

		// Orientate to direction of travel
		MakeUnit( &fwd );
		if (!(cur->flags & ENEMY_NEW_FACEFORWARDS))
			Orientate(&cur->nmeActor->actor->qRot,&fwd,&inVec,&path->nodes[1].worldTile->normal);

		// Elevate above gametile
		SetVector( &up, &cur->inTile->normal );
		ScaleVector( &up, path->nodes[0].offset );
		AddToVector( &cur->nmeActor->actor->pos, &up );
	}
}


void UpdateFrogWatcher( ENEMY *cur )
{
	QUATERNION q1, q2;
	ACTOR *act = cur->nmeActor->actor;
	float speed;

	SetQuaternion( &q1, &act->qRot );
	ActorLookAt( act, &frog[0]->actor->pos, LOOKAT_ANYWHERE );
	SetQuaternion( &q2, &act->qRot );
	speed = cur->path->nodes[0].speed * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateFlappyThing
	Purpose			: Make the enemy fly around in a box
	Parameters		: ENEMY
	Returns			: void
	Info			: One day, some enemies will be done this way...
*/
void UpdateFlappyThing( ENEMY *nme )
{
	PATH *path = nme->path;
	ACTOR *act = nme->nmeActor->actor;
	VECTOR fwd;
	QUATERNION q1, q2, q3;
	float t, best = 10000000, speed;
	unsigned long i;
		
	// If nme is very close to destination (stored in nmeactor->actor->rotaim) then choose a new destination.
	if( (DistanceBetweenPointsSquared(&act->pos,&act->rotaim) < 300) || !Magnitude(&act->rotaim) ) // 10 units apart
	{
		VECTOR up;

		// If just got to the special node we're aiming for, wait at it
		if( path->startNode )
		{
			path->startFrame = actFrameCount + (path->nodes[path->startNode].waitTime*60);
			nme->isWaiting = 1;
			path->startNode = 0;
			return;
		}

		// On a random chance, aim at the closest special node (DOESN'T INCLUDE OFFSET!)
		if( Random(100)>95 )
			for( i=2; i<path->numNodes; i++ )
			{
				t = DistanceBetweenPointsSquared( &act->pos, &path->nodes[i].worldTile->centre );
				if( t < best )
				{
					best = t;
					path->startNode = i;
				}
			}

		// If we just found a node to aim at, go there
		if( path->startNode )
		{
			SetVector( &up, &path->nodes[path->startNode].worldTile->normal );
			ScaleVector( &up, path->nodes[path->startNode].offset );
			AddVector( &act->rotaim, &up, &path->nodes[path->startNode].worldTile->centre );
		}
		else // Else pick a random point in space
		{
			VECTOR p1, p2;
			int dX, dY, dZ;

			SetVector( &up, &path->nodes[0].worldTile->normal );
			ScaleVector( &up, path->nodes[0].offset );
			AddVector( &p1, &path->nodes[0].worldTile->centre, &up );

			SetVector( &up, &path->nodes[1].worldTile->normal );
			ScaleVector( &up, path->nodes[1].offset );
			AddVector( &p2, &path->nodes[1].worldTile->centre, &up );

			// Get dimensions of the box we can move in.
			dX = Fabs(p2.v[0] - p1.v[0]);
			dY = Fabs(p2.v[1] - p1.v[1]);
			dZ = Fabs(p2.v[2] - p1.v[2]);

			p1.v[0] = min(p2.v[0], p1.v[0]);
			p1.v[1] = min(p2.v[1], p1.v[1]);
			p1.v[2] = min(p2.v[2], p1.v[2]);

			// Random destination within the valid box
			act->rotaim.v[0] = p1.v[0]+Random(dX);
			act->rotaim.v[1] = p1.v[1]+Random(dY);
			act->rotaim.v[2] = p1.v[2]+Random(dZ);
		}
	}

	// Store current orientation
	SetQuaternion( &q1, &act->qRot );
	
	// Set direction to desired point
	SubVector(&fwd,&act->rotaim,&act->pos);
	MakeUnit(&fwd);

	// Skewer a line to rotate around, and make a rotation
	CrossProduct((VECTOR *)&q3,&inVec,&fwd);
	t = DotProduct(&inVec,&fwd);
	if (t<-0.999)
		t=-0.999;
	if (t>0.999)
		t = 0.999;
	q3.w=acos(t);
	// Duh.
	GetQuaternionFromRotation(&q2,&q3);

	// Slerp between current and desired orientation
	speed = path->nodes[0].speed * gameSpeed;
	if( speed > 0.999 ) speed = 0.999;
	QuatSlerp( &q1, &q2, speed, &act->qRot );

	// Move forwards a bit in direction of facing
	RotateVectorByQuaternion( &fwd, &inVec, &act->qRot );
	ScaleVector( &fwd, path->nodes[1].speed*gameSpeed );
	AddVector( &act->pos, &fwd, &act->pos );

	nme->flags |= ENEMY_NEW_NODAMAGE;
}

/*	--------------------------------------------------------------------------------
	Function		: InitEnemyLinkedList
	Purpose			: initialises the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void InitEnemyLinkedList()
{
	enemyList.numEntries = 0;
	enemyList.head.next = enemyList.head.prev = &enemyList.head;
}

/*	--------------------------------------------------------------------------------
	Function		: AddEnemy
	Purpose			: adds an enemy to the linked list
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void AddEnemy(ENEMY *enemy)
{
	if(enemy->next == NULL)
	{
		enemyList.numEntries++;
		enemy->prev = &enemyList.head;
		enemy->next = enemyList.head.next;
		enemyList.head.next->prev = enemy;
		enemyList.head.next = enemy;
	}
}

/*	--------------------------------------------------------------------------------
	Function		: SubEnemy
	Purpose			: removes an enemy from the linked list
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void SubEnemy(ENEMY *enemy)
{
	if(enemy->next == NULL)
		return;

	if(enemy->path)
	{
		if(enemy->path->nodes)
			JallocFree((UBYTE**)&enemy->path->nodes);

		JallocFree((UBYTE**)&enemy->path);
	}

	enemy->prev->next = enemy->next;
	enemy->next->prev = enemy->prev;
	enemy->next = NULL;
	enemyList.numEntries--;

	JallocFree((UBYTE **)&enemy);
}


/*	--------------------------------------------------------------------------------
	Function		: FreeEnemyLinkedList
	Purpose			: frees the enemy linked list
	Parameters		: 
	Returns			: void
	Info			: 
*/
void FreeEnemyLinkedList()
{
	ENEMY *cur,*next;

	// check if any elements in list
	if(enemyList.numEntries == 0)
		return;

	dprintf"Freeing linked list : ENEMY : (%d elements)\n",enemyList.numEntries));

	// traverse enemy list and free elements
	for(cur = enemyList.head.next; cur != &enemyList.head; cur = next)
	{
		next = cur->next;

		SubEnemy(cur);
	}

	// initialise list for future use
	InitEnemyLinkedList();
}


ENEMY *CreateAndAddEnemy(char *eActorName, int flags, long ID, PATH *path )
{
	int enemyType = 0;
	float shadowRadius = 0;
	int initFlags;

	ENEMY *newItem = (ENEMY *)JallocAlloc(sizeof(ENEMY),YES,"NME");
	AddEnemy(newItem);
	newItem->flags = flags;

	if( !(flags & ENEMY_NEW_SWARM) )
	{
		initFlags |= INIT_ANIMATION;
		initFlags |= INIT_SHADOW;
		shadowRadius = 20;
	}

	// create and add the nme actor
	newItem->nmeActor = CreateAndAddActor(eActorName,0,0,0,initFlags,0,0);
	if(newItem->nmeActor->actor->objectController)
		InitActorAnim(newItem->nmeActor->actor);

	// set shadow radius (if applicable)
	if(shadowRadius)
		if (newItem->nmeActor->actor->shadow)
			newItem->nmeActor->actor->shadow->radius = shadowRadius;

	// specify enemy radius if the enemy is radius based
	if( flags & ENEMY_NEW_RADIUSBASEDCOLLISION)
		newItem->nmeActor->radius = 15.0F; 	// set a default collision radius
	else
		newItem->nmeActor->radius = 0.0F;	// set radius to zero - not used for collision detection

	AnimateActor(newItem->nmeActor->actor,0,YES,NO,newItem->nmeActor->animSpeed, 0, 0);
	newItem->nmeActor->actor->scale.v[X] = 1.5F;
	newItem->nmeActor->actor->scale.v[Y] = 1.5F;
	newItem->nmeActor->actor->scale.v[Z] = 1.5F;

	newItem->active			= 1;
	
	newItem->path			= NULL;
	newItem->inTile			= NULL;
	
	newItem->speed			= 1.0F;
	newItem->startSpeed		= 1.0F;
	newItem->accel			= 0.0F;
	newItem->uid = ID;
	newItem->isSnapping = 0;

	AssignPathToEnemy(newItem,path,0);

	if( newItem->flags & ENEMY_NEW_FOLLOWPATH )
		newItem->Update = UpdatePathNME;
	else if( newItem->flags & ENEMY_NEW_WATCHFROG )
		newItem->Update = UpdateFrogWatcher;
	else if( newItem->flags & ENEMY_NEW_SNAPFROG )
		newItem->Update = UpdateSnapper;
	else if( newItem->flags & ENEMY_NEW_SNAPTILES )
		newItem->Update = UpdateTileSnapper;
	else if( newItem->flags & ENEMY_NEW_VENT )
	{
		newItem->isSnapping = -2;
		newItem->Update = UpdateVent;
	}
	else if( (newItem->flags & ENEMY_NEW_MOVEUP) || (newItem->flags & ENEMY_NEW_MOVEDOWN) )
		newItem->Update = UpdateMoveVerticalNME;
	else if( (newItem->flags & ENEMY_NEW_ROTATEPATH_XZ ) || (newItem->flags & ENEMY_NEW_ROTATEPATH_XY) || (newItem->flags & ENEMY_NEW_ROTATEPATH_ZY) )
		newItem->Update = UpdateRotatePathNME;
	else if( newItem->flags & ENEMY_NEW_HOMING )
		newItem->Update = UpdateHomingNME;
	else if( newItem->flags & ENEMY_NEW_MOVEONMOVE )
		newItem->Update = UpdateMoveOnMoveNME;
	else if( newItem->flags & ENEMY_NEW_FLAPPYTHING )
		newItem->Update = UpdateFlappyThing;

	return newItem;
}

void AssignPathToEnemy(ENEMY *nme,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR enemyStartPos;

	nme->path = path;

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
		for(i=0; i<path->numNodes; i++)
			nme->path->nodes[i].worldTile = &firstTile[(unsigned long)path->nodes[i].worldTile];

	// set the start position for the enemy
	nme->path->fromNode	= nme->path->startNode;

	if(nme->flags & ENEMY_NEW_FORWARDS)
	{
		// this enemy moves forward thru path nodes
		nme->flags |= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode + 1;

		if(nme->path->toNode > GET_PATHLASTNODE(path))
			nme->path->toNode = 0;
	}
	else if(nme->flags & ENEMY_NEW_BACKWARDS)
	{
		// this enemy moves backward thru path nodes
		nme->flags |= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode - 1;

		if(nme->path->toNode < 0)
			nme->path->toNode = GET_PATHLASTNODE(path);
	}
	else if((nme->flags & ENEMY_NEW_MOVEUP) ||
			(nme->flags & ENEMY_NEW_MOVEDOWN))
	{
		// this enemy moves up or down
		nme->path->fromNode = nme->path->toNode = 0;
	}

	// set enemy position to relevant point on path
	GetPositionForPathNode(&enemyStartPos,&path->nodes[nme->path->fromNode]);
	SetVector(&nme->nmeActor->actor->pos,&enemyStartPos);
	NormalToQuaternion(&nme->nmeActor->actor->qRot,&path->nodes[nme->path->fromNode].worldTile->normal);

	// set enemy current 'in' tile and speeds and pause times
	nme->inTile		= path->nodes[nme->path->fromNode].worldTile;
	nme->speed		= path->nodes[nme->path->fromNode].speed;
	nme->startSpeed	= path->nodes[nme->path->fromNode].speed;
	nme->isWaiting	= path->nodes[nme->path->fromNode].waitTime;

	nme->path->startFrame = actFrameCount;
	nme->path->endFrame = (actFrameCount+(60*nme->speed));

	CalcEnemyNormalInterps(nme);
}


/*	--------------------------------------------------------------------------------
	Function		: EnemyReachedTopOrBottomPoint
	Purpose			: checks if an enemy has arrived at top or bottom point (1 node based)
	Parameters		: ENEMY *
	Returns			: BOOL - TRUE if node reached, else FALSE
	Info			: 
*/
BOOL EnemyReachedTopOrBottomPoint(ENEMY *nme)
{
	VECTOR toPos;
	PATH *path = nme->path;

	// check if path extreme point is reached
	if(nme->flags & ENEMY_NEW_MOVEUP)
	{
		// moving up
		GetPositionForPathNodeOffset2(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&toPos) < (nme->speed * nme->speed))
			return TRUE;
	}
	else if(nme->flags & ENEMY_NEW_MOVEDOWN)
	{
		// moving down
		GetPositionForPathNode(&toPos,&path->nodes[0]);

		if(DistanceBetweenPointsSquared(&nme->nmeActor->actor->pos,&toPos) < (nme->speed * nme->speed))
			return TRUE;
	}

	return FALSE;
}


/*	--------------------------------------------------------------------------------
	Function		: UpdateEnemyPathNodes
	Purpose			: updates enemy path move status
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
void UpdateEnemyPathNodes(ENEMY *nme)
{
	VECTOR nmePos;
	int wait;
	PATH *path = nme->path;
	unsigned long flags = nme->flags;
	
	if(flags & ENEMY_NEW_FORWARDS)	// enemy moves forward through path nodes
	{
		if(path->toNode >= GET_PATHLASTNODE(path))	// reached end of path nodes
		{
			if(flags & ENEMY_NEW_PINGPONG)		// this enemy has ping-pong movement
			{
				nme->flags		^= (ENEMY_NEW_FORWARDS | ENEMY_NEW_BACKWARDS);	// reverse enemy path movement
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;
			}
			else if(flags & ENEMY_NEW_CYCLE) 	// enemy has cyclic movement
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= 0;
			}
			else
			{
				path->fromNode	= 0;
				path->toNode	= 1;
				GetPositionForPathNode(&nmePos,&path->nodes[0]);
				SetVector(&nme->nmeActor->actor->pos,&nmePos);
			}
		}
		else
		{
			path->fromNode = path->toNode;
			path->toNode++;
		}
	}
	else if(flags & ENEMY_NEW_BACKWARDS) // enemy moves backwards through path nodes
	{
		if(path->toNode <= 0)
		{
			// reached beginning of path nodes
			// check if this enemy has ping-pong movement
			if(flags & ENEMY_NEW_PINGPONG)
			{
				// reverse enenmy path movement
				nme->flags		^= (ENEMY_NEW_FORWARDS | ENEMY_NEW_BACKWARDS);	// reverse enemy path movement
				path->fromNode	= 0;
				path->toNode	= 1;
			}
			else if(flags & ENEMY_NEW_CYCLE)
			{
				// enenmy has cyclic movement
				path->fromNode	= 0;
				path->toNode	= GET_PATHLASTNODE(path);
			}
			else
			{
				path->fromNode	= GET_PATHLASTNODE(path);
				path->toNode	= GET_PATHLASTNODE(path) - 1;
				GetPositionForPathNode(&nmePos,&path->nodes[GET_PATHLASTNODE(path)]);
				SetVector(&nme->nmeActor->actor->pos,&nmePos);
			}
		}
		else
		{
			path->fromNode = path->toNode;
			path->toNode--;
		}

	}
	else if((flags & ENEMY_NEW_PINGPONG) && flags & (ENEMY_NEW_MOVEUP | ENEMY_NEW_MOVEDOWN))
	{
		nme->flags	^= (ENEMY_NEW_MOVEUP | ENEMY_NEW_MOVEDOWN);
	}

	nme->speed		= path->nodes[path->fromNode].speed;
	nme->isWaiting	= path->nodes[path->fromNode].waitTime;

	// Stop overshoot when waiting on a path node
	if (nme->isWaiting)
	{
		GetPositionForPathNode(&nmePos, &path->nodes[path->fromNode]);
		SetVector(&nme->nmeActor->actor->pos, &nmePos);
	}

	if (nme->flags & ENEMY_NEW_RANDOMSPEED) nme->speed *= ENEMY_RANDOMNESS;

	
	CalcEnemyNormalInterps(nme);
}


/*	--------------------------------------------------------------------------------
	Function		: CalcEnemyNormalInterps
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/
void CalcEnemyNormalInterps(ENEMY *nme)
{
	PATH *path;
	PATHNODE *fromNode,*toNode;
	float numSteps;
	VECTOR destNormal,fromPos,toPos;

	path = nme->path;
	if(path->numNodes < 2)
		return;

	fromNode	= &path->nodes[path->fromNode];
	toNode		= &path->nodes[path->toNode];

	// set the current enemy normal to that of the 'from' node and get the dest normal
	SetVector(&nme->currNormal,&fromNode->worldTile->normal);
	SetVector(&destNormal,&toNode->worldTile->normal);

	// calculate deltas for linear interpolation of enemy normal during movement
	SubVector(&nme->deltaNormal,&destNormal,&nme->currNormal);

	// determine number of 'steps' over which to interpolate
	GetPositionForPathNode(&fromPos,fromNode);
	GetPositionForPathNode(&toPos,toNode);
	
	numSteps = DistanceBetweenPoints(&fromPos,&toPos);
	if(numSteps == 0)
		numSteps = 1;
	numSteps /= nme->speed;

	nme->deltaNormal.v[X] /= numSteps;
	nme->deltaNormal.v[Y] /= numSteps;
	nme->deltaNormal.v[Z] /= numSteps;
}
