#include <ultra64.h>
#include "incs.h"

#define BABY_UID	255

int multiplayerMode;


/*	--------------------------------------------------------------------------------
	Function		: UpdateCTF
	Purpose			: Do game mechanics for Capture the Frog multipleyer mode
	Parameters		: 
	Returns			: 
	Info			:
*/
void UpdateCTF( )
{
	multiplayerMode = MULTIMODE_CTF;


}


/*	--------------------------------------------------------------------------------
	Function		: UpdateRace
	Purpose			: Do game mechanics for mulitplayer race mode
	Parameters		: 
	Returns			: 
	Info			:
*/
void UpdateRace( )
{
	int i, j;

	multiplayerMode = MULTIMODE_RACE;

	for( i=0; i<NUM_FROGS; i++ )
	{
		if ((frog[i]->action.safe == 0) && (frameCount > 20))
		if (!IsPointVisible(&frog[i]->actor->pos))
		{
			KillMPFrog(i);

			for( j=0; j<NUM_FROGS; j++ )
				if (IsPointVisible(&frog[j]->actor->pos))
				{
					TeleportActorToTile(frog[i],currTile[j],i);
					destTile[i] = currTile[j];
				}
		}
	}
}


/*	--------------------------------------------------------------------------------
	Function		: PickupBabyFrogMulti
	Purpose			: Player has collected baby frog in a multiplayer game
	Parameters		: ACTOR2
	Returns			: void
	Info			:
*/
void PickupBabyFrogMulti( ENEMY *baby, int pl )
{
	int i;
	ENEMY *nme;
	PATH *path = baby->path, *path2;

	for( i=0; i<numBabies; i++ ) if( babyList[i].baby == baby->nmeActor ) break;

	if( i==numBabies ) return;

	lastBabySaved = i;

	// For different multiplayer modes do different things:
		// CTF - transport baby back to frogger start point - assumes babies are all randommove nmes

	if( multiplayerMode == MULTIMODE_CTF )
	{
		// Find placeholder enemy that holds the baby respawn path for this frog
		for(nme = enemyList.head.next; nme != &enemyList.head; nme = nme->next )
			if( nme->uid-BABY_UID == pl ) break;

		if( nme == &enemyList.head ) return;

		path2 = nme->path;

		if( baby->flags & ENEMY_NEW_RANDOMMOVE )
		{
			path->nodes[0].worldTile = path2->nodes[0].worldTile;
			path->nodes[1].worldTile = path2->nodes[1].worldTile;
			path->nodes[2].worldTile = path2->nodes[2].worldTile;
			baby->isSnapping = 0;
		}
	}
}

void KillMPFrog(int num)
{
	frog[num]->action.stun = 50;
	frog[num]->action.safe = 80;
	
	if (frog[num]->action.healthPoints > 0)
		sprHeart[num*3+(--frog[num]->action.healthPoints)]->draw = 0;
}
