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

static void GetEnemyActiveTile(ENEMY *enemy);

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

/*	--------------------------------------------------------------------------------
	Function		: GetEnemyActiveGameTile
	Purpose			: determines which tile the enemy is currently 'in'
	Parameters		: ENEMY *
	Returns			: void
	Info			: 
*/
static void GetEnemyActiveTile(ENEMY *nme)
{
	VECTOR v1,v2,diff;
	float halfdist;

	if(nme->flags & ENEMY_NEW_FOLLOWPATH)
	{
		GetPositionForPathNode(&v1,&nme->path->nodes[nme->path->fromNode]);
		GetPositionForPathNode(&v2,&nme->path->nodes[nme->path->toNode]);

		halfdist = DistanceBetweenPoints(&v1,&v2) / 2.0F;

		if(DistanceBetweenPointsSquared(&v1,&nme->nmeActor->actor->pos) < (halfdist * halfdist))
			nme->inTile = nme->path->nodes[nme->path->fromNode].worldTile;
		else
			nme->inTile = nme->path->nodes[nme->path->toNode].worldTile;
	}
	else if( (nme->flags & ENEMY_NEW_ROTATEPATH_XZ) ||
			(nme->flags & ENEMY_NEW_ROTATEPATH_XY) ||
			(nme->flags & ENEMY_NEW_ROTATEPATH_ZY) )
	{
		nme->inTile = FindNearestTile( nme->nmeActor->actor->pos );
	}
	else
	{
		// no path associated with this enemy
		nme->inTile = nme->path->nodes[0].worldTile;
	}
}
/*	--------------------------------------------------------------------------------
	Function		: 
	Purpose			: 
	Parameters		: 
	Returns			: 
	Info			: 
*/

GAMETILE *FindJoinedTileByDirection( GAMETILE *st, VECTOR *d )
{
	float distance = 100000, t;
	int i;
	GAMETILE *res = NULL;

	for (i=0; i<4; i++)
	{
		t = DotProduct( d, &st->dirVector[i] );
		if( (t < distance) && st->tilePtrs[i] )
		if( st->tilePtrs[i]->state == TILESTATE_NORMAL )
		{
			distance = t;
			res = st->tilePtrs[i];
		}
	}

	return res;
}

void DamageFrog( int num )
{
	frog[num]->action.healthPoints--;
	
	if(frog[num]->action.healthPoints != 0)
	{
		cameraShake = 25;
		PlaySample(42,NULL,192,128);
		frog[num]->action.safe = 25;
	}
	else
	{
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
#define SNAPPER_TIME 10
float rotLimit = 0.7;

void UpdateEnemies()
{
	ENEMY *cur,*next;
	VECTOR fromPosition,toPosition;
	VECTOR fwd;
	VECTOR moveVec;
	PLANE2 rebound;
	float length,fxDist;
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
			float angle;
			SubVector( &moveVec, &cur->nmeActor->actor->pos, &frog[0]->actor->pos );
			MakeUnit( &moveVec );
			angle = acos(DotProduct( &currTile[0]->dirVector[frogFacing[0]], &moveVec ));
			if( angle < rotLimit )
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
				{
					QUATERNION q, res;
					float t;
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
				continue;
			}
		}

//		if ( cur->isIdle )
//			cur->isIdle--;
		// ENDIF

		if(cur->flags & ENEMY_NEW_FOLLOWPATH)
		{
			// process enemies that follow a path (>1 node in path)
			
			// first, update the enemy position
			GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->toNode]);
			GetPositionForPathNode(&fromPosition,&cur->path->nodes[cur->path->fromNode]);
			
			SubVector(&fwd,&toPosition,&fromPosition);
			
			length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
			
			ScaleVector(&fwd,length);
			AddVector(&cur->nmeActor->actor->pos,&fwd,&fromPosition);

			if (cur->flags & ENEMY_NEW_PUSHESFROG)
			{
				if( cur->nmeActor->distanceFromFrog < 30 )
				{
					GAMETILE *tile = NULL;
					AddVector( &frog[0]->actor->pos, &frog[0]->actor->pos, &fwd );

					tile = FindNearestTile( frog[0]->actor->pos );

					if( tile != currTile[0] )
					{
						currTile[0] = tile;
						destTile[0] = currTile[0];
					}
				}
			}

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
/*
			Dave's new push block code will go in here ...

			if (cur->flags & ENEMY_NEW_PUSHESFROG)
			{
				GAMETILE *tile = cur->path->nodes[cur->path->toNode].worldTile;

				if (currTile[0] == tile)
				{
					// USE FROG MOVE CODE TO PUSH FROG!

					// Except I can't, so just do something fairly random for now.

					if (cur->path->toNode < (cur->path->numNodes - 1))
						tile = cur->path->nodes[cur->path->toNode+1].worldTile;

					currTile[0] = destTile[0] = tile;
					SetVector( &frog[0]->actor->pos, &tile->centre);

					// i hate frogs
				}
			}
*/
		}
		else
			if(cur->flags & ENEMY_NEW_WATCHFROG)
				ActorLookAt( cur->nmeActor->actor, &frog[0]->actor->pos, LOOKAT_ANYWHERE );
			else if (cur->flags & ENEMY_NEW_SNAPFROG)
			{
				static GAMETILE *tile = NULL;
				QUATERNION q1, q2;

				// Idle animations
				switch( cur->isSnapping )
				{
				case 0:
					if( actFrameCount < cur->path->endFrame )
						break;

					tile = NULL;

					if (Random(1000)>980)
						AnimateActor(cur->nmeActor->actor,2,NO,NO,cur->nmeActor->animSpeed, 0, 0);
				
					if (Random(1000)>950)
					{
						if (Random(1000)>950)
							AnimateActor(cur->nmeActor->actor,3,NO,YES,cur->nmeActor->animSpeed, 0, 0);
						else
							AnimateActor(cur->nmeActor->actor,0,NO,YES,cur->nmeActor->animSpeed, 0, 0);
					}

					// Slerp orientation towards frog
					SetQuaternion( &q1, &cur->nmeActor->actor->qRot );
					ActorLookAt( cur->nmeActor->actor, &frog[0]->actor->pos, LOOKAT_2D );
					SetQuaternion( &q2, &cur->nmeActor->actor->qRot );
					QuatSlerp( &q1, &q2, cur->path->nodes[0].speed, &cur->nmeActor->actor->qRot );

					// If the snapper has just spotted the frog, set snap time
					if( cur->nmeActor->distanceFromFrog < 5625 ) // 75*75
					{
						tile = FindNearestTile( frog[0]->actor->pos );

						SetQuaternion( &q1, &cur->nmeActor->actor->qRot );
						ActorLookAt( cur->nmeActor->actor, &tile->centre, LOOKAT_2D );
						SetQuaternion( &q2, &cur->nmeActor->actor->qRot );
						QuatSlerp( &q1, &q2, cur->path->nodes[0].speed, &cur->nmeActor->actor->qRot );

						cur->path->startFrame = actFrameCount;
						cur->path->endFrame = cur->path->startFrame + (cur->path->nodes[0].waitTime * waitScale);

						cur->isSnapping = SNAPPER_TIME;
					}
					break;

				case 1: // Time to snap and hope
					if( actFrameCount < cur->path->endFrame )
						break;

					cur->path->startFrame = actFrameCount;
					cur->path->endFrame = cur->path->startFrame + (cur->path->nodes[0].waitTime * waitScale);
					cur->isSnapping = 0;

					if( (tile == currTile[0]) && (!frog[0]->action.dead) && (!frog[0]->action.safe) ) // If frog is still on the target tile
						DamageFrog(0);
					break;

				case SNAPPER_TIME: 				// Snap animation
					if( (actFrameCount-cur->path->startFrame) < 0.8*(cur->path->endFrame-cur->path->startFrame) )
						break;

					AnimateActor(cur->nmeActor->actor,1,NO,NO,cur->nmeActor->animSpeed, 0, 0);
					cur->isSnapping = 1;
					break;
				}
			}
			else if( cur->flags & ENEMY_NEW_SNAPTILES )
			{
				QUATERNION q1, q2;

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
					cur->isSnapping = SNAPPER_TIME;
					break;

				case 1:
					if( actFrameCount < cur->path->endFrame )
						break;

					cur->path->startFrame = actFrameCount;
					cur->path->endFrame = cur->path->startFrame + (cur->path->nodes[cur->path->fromNode].waitTime * waitScale);
					cur->isSnapping = 0;

					// If the frog is on our current target tile
					if( (cur->path->nodes[cur->path->fromNode].worldTile == currTile[0]) && (!frog[0]->action.dead) && (!frog[0]->action.safe) )//FindNearestTile(frog[0]->actor->pos) )
						DamageFrog(0);
					break;

				case SNAPPER_TIME:
					if( (actFrameCount-cur->path->startFrame) < 0.8*(cur->path->endFrame-cur->path->startFrame) )
					{
						GetPositionForPathNode(&toPosition,&cur->path->nodes[cur->path->fromNode]);
						SetQuaternion( &q1, &cur->nmeActor->actor->qRot );
						ActorLookAt( cur->nmeActor->actor, &toPosition, LOOKAT_2D );
						SetQuaternion( &q2, &cur->nmeActor->actor->qRot );
						QuatSlerp( &q1, &q2, cur->path->nodes[0].speed, &cur->nmeActor->actor->qRot );
						break;
					}

					AnimateActor(cur->nmeActor->actor,1,NO,NO,cur->nmeActor->animSpeed, 0, 0);
					cur->isSnapping = 1;
					break;
				}
			}
			else if( cur->flags & ENEMY_NEW_VENT ) // Speed on flag is how long the vent fires for, waittime is delay between bursts
			{
				PATH *path = cur->path;

				switch( cur->isSnapping )
				{
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
					SetVector(&rebound.point,&cur->inTile->centre);
					SetVector(&rebound.normal,&cur->inTile->normal);
					CreateAndAddFXExplodeParticle( EXPLODEPARTICLE_TYPE_NORMAL, &cur->nmeActor->actor->pos, &cur->inTile->normal, cur->nmeActor->animSpeed*path->numNodes, 30, &rebound, cur->nmeActor->value1*path->numNodes );

					// Check for collision with frog, and do damage
					for( i=0; i < path->numNodes; i++ )
						if( (path->nodes[i].worldTile == currTile[0]) && (!frog[0]->action.dead) && (!frog[0]->action.safe) )
							DamageFrog(0);

					break;
				}
			}
			else
			{
				// process enemies that are based on a single node
	
				if( (cur->flags & ENEMY_NEW_MOVEUP) || (cur->flags & ENEMY_NEW_MOVEDOWN) )
				{
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

				// Move around a single flag
				if( (cur->flags & ENEMY_NEW_ROTATEPATH_XZ ) ||
					(cur->flags & ENEMY_NEW_ROTATEPATH_XY) ||
					(cur->flags & ENEMY_NEW_ROTATEPATH_ZY) )
				{
					length = (float)(actFrameCount - cur->path->startFrame)/(float)(cur->path->endFrame - cur->path->startFrame);
					length *= PI2;
					length += cur->path->nodes->offset2 * PI2; // Fraction of circumference it starts at

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
						cur->path->endFrame = cur->path->startFrame+(60*cur->speed);
					}
				}
			}

		// determine which world tile the enemy is currently 'in'
		GetEnemyActiveTile(cur);

		// Enemy homes in on frog - no tiles!
		if( cur->flags & ENEMY_NEW_HOMING )
		{
			GAMETILE *chTile;
			VECTOR nmeup, tVec;
			float distance=10000, best=-2;
			short bFlag = 0;

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
		else if( cur->flags & ENEMY_NEW_MOVEONMOVE )
		{
			VECTOR frogVec, up;
			PATH *path = cur->path;

			if( cur->nmeActor->distanceFromFrog > 100000 )
			{
				cur->isIdle = 0;
				continue;
			}

			// Check if the enemy has 3 path nodes. Allocate if not ( first time through )
			if( cur->path->numNodes < 3 )
			{
				PATHNODE n;

				memcpy( &n, path->nodes, sizeof(PATHNODE) );
				JallocFree( (UBYTE **)&path->nodes );
				path->nodes = (PATHNODE *)JallocAlloc( sizeof(PATHNODE)*3,YES,"Path" );
				path->numNodes = 3;
				memcpy( path->nodes, &n, sizeof(PATHNODE) );

				// Find initial move for enemy to make
				path->nodes[1].worldTile = NULL;
				path->nodes[2].worldTile = path->nodes[0].worldTile;
			}

			// If frog has moved
			cur->isIdle += player[0].hasJumped;

			// If enemy is on the next path node, set startnode worldtile and the next to zero
			if( (path->nodes[2].worldTile) && (actFrameCount > path->endFrame) )
			{
				path->nodes[1].worldTile = path->nodes[2].worldTile;
				path->nodes[2].worldTile = NULL;
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

				cur->inTile = FindNearestTile( cur->nmeActor->actor->pos );

				// Elevate above gametile
				SetVector( &up, &cur->inTile->normal );
				ScaleVector( &up, path->nodes[0].offset );
				AddVector( &cur->nmeActor->actor->pos, &cur->nmeActor->actor->pos, &up );
			}
		}

		if (NUM_FROGS!=1)
		{
			for (i=0; i<NUM_FROGS; i++)
			{
				if(cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION)
				{
					if((!frog[i]->action.safe) && ActorsHaveCollided(frog[i],cur->nmeActor))
						KillMPFrog(i);
				}
				else
					if((currTile[i] == cur->inTile) && 	(!frog[i]->action.safe) && (!(player[i].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
						!(player[i].frogState & FROGSTATUS_ISFLOATING))
					{
						KillMPFrog(i);
					}
			}
		}
		else
		{
			// check if frog has been 'killed' by current enemy - radius based collision
			if(cur->flags & ENEMY_NEW_RADIUSBASEDCOLLISION)
			{
				// perform radius collision check between frog and current enemy
				if((!frog[0]->action.dead) && (!frog[0]->action.safe) && ActorsHaveCollided(frog[0],cur->nmeActor))
				{
					frog[0]->action.healthPoints--;
					if(frog[0]->action.healthPoints != 0)
					{
						cameraShake = 25;
						PlaySample(42,NULL,192,128);
						frog[0]->action.safe = 25;
					}
					else
					{
						PlaySample(110,NULL,192,128);
						AnimateActor(frog[0]->actor,FROG_ANIM_TRYTOFLY,NO,NO,0.5F,0,0);
						frog[0]->action.dead = 50;
						frog[0]->action.healthPoints = 3;
				
						switch(cur->nmeActor->actor->type)
						{
							case NMETYPE_CAR:
							case NMETYPE_TRUCK:
							case NMETYPE_FORK:
								cameraShake = 50;
								frog[0]->action.deathBy = DEATHBY_RUNOVER;
								PlaySample(31,NULL,192,128);
								break;

							default:
								frog[0]->action.deathBy = DEATHBY_NORMAL;
						}
				
						player[0].frogState |= FROGSTATUS_ISDEAD;
					}
				}
			}
			
			// check if frog has been 'killed' by current enemy - tile based collision
			else if((currTile[0] == cur->inTile) && (!frog[0]->action.dead) &&
					(!frog[0]->action.safe) && (!(player[0].frogState & FROGSTATUS_ISSUPERHOPPING) || (cur->flags & ENEMY_NEW_NOJUMPOVER)) &&
					(!currPlatform[0]) && !(player[0].frogState & FROGSTATUS_ISFLOATING) && !(cur->flags & ENEMY_NEW_NODAMAGE))
			{
				frog[0]->action.healthPoints--;
				if(frog[0]->action.healthPoints != 0)
				{
					cameraShake = 25;
					PlaySample(42,NULL,192,128);
					frog[0]->action.safe = 25;
				}
				else
				{
					PlaySample(110,NULL,192,128);
					AnimateActor(frog[0]->actor,FROG_ANIM_TRYTOFLY,NO,NO,0.5F,0,0);
					frog[0]->action.dead = 50;
					frog[0]->action.healthPoints = 3;
				
					switch(cur->nmeActor->actor->type)
					{
						case NMETYPE_CAR:
						case NMETYPE_TRUCK:
						case NMETYPE_FORK:
							cameraShake = 50;
							frog[0]->action.deathBy = DEATHBY_RUNOVER;
							PlaySample(31,NULL,192,128);
							break;

						default:
							frog[0]->action.deathBy = DEATHBY_NORMAL;
					}
				
					player[0].frogState |= FROGSTATUS_ISDEAD;
				}
			}
		}
		// process enemies (update anims, etc.)
		/*
		switch(cur->nmeActor->actor->type)
		{
			case NMETYPE_MOLE:
				ProcessNMEMole(cur);
				break;
			case NMETYPE_MOWER:
				ProcessNMEMower(cur->nmeActor);
				break;
		}
		*/
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
					CreateAndAddFXRipple( RIPPLE_TYPE_RING, &rPos, &cur->currNormal, 30, cur->speed, 1, 5 );
				else // Gentle ripples
					CreateAndAddFXRipple( RIPPLE_TYPE_RING, &rPos, &cur->currNormal, 50, 1, 0.1, 20 );
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
				CreateAndAddFXSmoke(&cur->nmeActor->actor->pos,80,20);
		}
		if( cur->nmeActor->effects & EF_SPARK_BURSTS )
		{
			long r;
			if( cur->nmeActor->value1 )
				r = Random(cur->nmeActor->value1)+1;
			else
				r = 10;

			if( !(actFrameCount%r) && (fxDist < ACTOR_DRAWDISTANCEINNER))
			{
				SetVector(&rebound.point,&cur->inTile->centre);
				SetVector(&rebound.normal,&cur->inTile->normal);
				CreateAndAddFXExplodeParticle( EXPLODEPARTICLE_TYPE_NORMAL, &cur->nmeActor->actor->pos, &cur->currNormal, 5, 30, &rebound, 10 );
			}
		}
	}
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


ENEMY *CreateAndAddEnemy(char *eActorName, int initFlags )
{
	int enemyType = 0;
	float shadowRadius = 0;

	ENEMY *newItem = (ENEMY *)JallocAlloc(sizeof(ENEMY),YES,"NME");
	AddEnemy(newItem);

	enemyType = NMETYPE_WASP;

	// check nme type and assign shadow if necessary

//	if( !(initFlags & INIT_SWARM) )
//	{
		initFlags |= INIT_ANIMATION;
		if(enemyType == NMETYPE_WASP)
		{
			initFlags |= INIT_SHADOW;
			shadowRadius = 20;
		}
//	}

	// create and add the nme actor
	newItem->nmeActor = CreateAndAddActor(eActorName,0,0,0,initFlags,0,0);
	if(newItem->nmeActor->actor->objectController)
		InitActorAnim(newItem->nmeActor->actor);

	// set shadow radius (if applicable)
	if(shadowRadius)
		if (newItem->nmeActor->actor->shadow)
			newItem->nmeActor->actor->shadow->radius = shadowRadius;

	// specify enemy radius if the enemy is radius based
	if(initFlags & ENEMY_NEW_RADIUSBASEDCOLLISION)
		newItem->nmeActor->radius = 15.0F; 	// set a default collision radius
	else
		newItem->nmeActor->radius = 0.0F;	// set radius to zero - not used for collision detection

	// set animation depending on enemy type
	/*
	if(enemyType > NMETYPE_NONE)
	{
		switch(enemyType)
		{
			case NMETYPE_MOLE:
				AnimateActor(newItem->nmeActor->actor,3,NO,NO,0.1F*newItem->nmeActor->animSpeed, 0, 0);
				newItem->nmeActor->actor->status = NMESTATE_MOLE_IDLE;
				newItem->nmeActor->actor->scale.v[X] = 0.0075F;
				newItem->nmeActor->actor->scale.v[Y] = 0.0075F;
				newItem->nmeActor->actor->scale.v[Z] = 0.0075F;
				newItem->isIdle = Random(500);
				break;

			case NMETYPE_MOWER:
			case NMETYPE_ROLLER:
				AnimateActor(newItem->nmeActor->actor,0,YES,NO,1.5F*newItem->nmeActor->animSpeed, 0, 0);
				newItem->nmeActor->actor->status = NMESTATE_MOWER_IDLE;
				break;
			case NMETYPE_WASP:*/
	AnimateActor(newItem->nmeActor->actor,0,YES,NO,newItem->nmeActor->animSpeed, 0, 0);
	//newItem->nmeActor->actor->status = NMESTATE_WASP_MOVING;
	newItem->nmeActor->actor->scale.v[X] = 1.5F;
	newItem->nmeActor->actor->scale.v[Y] = 1.5F;
	newItem->nmeActor->actor->scale.v[Z] = 1.5F;
				/*
				break;
		}

		newItem->nmeActor->actor->type = enemyType;
		newItem->nmeActor->action.dead = 30;
	}
	*/

	newItem->active			= 1;
	
	newItem->path			= NULL;
	newItem->inTile			= NULL;
	
	newItem->speed			= 1.0F;
	newItem->startSpeed		= 1.0F;
	newItem->accel			= 0.0F;
	newItem->isSnapping 	= 0;

	return newItem;
}

void AssignPathToEnemy(ENEMY *nme,unsigned long enemyFlags,PATH *path,unsigned long pathFlags)
{
	int i;
	VECTOR enemyStartPos;

	// assign the path to this enemy
	nme->flags		|= enemyFlags;
	nme->path		= path;

	//dprintf"Add enemy path : "));

	// check if pathnode indices need converting to game tile pointers
	if(pathFlags & PATH_MAKENODETILEPTRS)
	{
		for(i=0; i<path->numNodes; i++)
		{
			// convert integer to a valid game tile
			//dprintf"%d, ",(unsigned long)path->nodes[i].worldTile));
			nme->path->nodes[i].worldTile = &firstTile[(unsigned long)path->nodes[i].worldTile];
		}
	}

	// set the start position for the enemy
	nme->path->fromNode	= nme->path->startNode;

	if(enemyFlags & ENEMY_NEW_FORWARDS)
	{
		// this enemy moves forward thru path nodes
		nme->flags				|= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode + 1;
		if(nme->path->toNode > GET_PATHLASTNODE(path))
			nme->path->toNode = 0;
	}
	else if(enemyFlags & ENEMY_NEW_BACKWARDS)
	{
		// this enemy moves backward thru path nodes
		nme->flags				|= ENEMY_NEW_FOLLOWPATH;
		nme->path->toNode = nme->path->fromNode - 1;
		if(nme->path->toNode < 0)
			nme->path->toNode = GET_PATHLASTNODE(path);
	}
	else if((enemyFlags & ENEMY_NEW_MOVEUP) ||
			(enemyFlags & ENEMY_NEW_MOVEDOWN))
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

	//dprintf"\n"));
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
