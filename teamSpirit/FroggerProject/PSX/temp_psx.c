#include "world_eff.h"
#include "temp_psx.h"

void QuatToPSXMatrix(IQUATERNION* q, MATRIX* M)
{
    fixed Nq = FMul(q->x,q->x) + FMul(q->y,q->y)
    		 + FMul(q->z,q->z) + FMul(q->w,q->w);
    fixed s = (Nq > 0) ? FDiv(8192,Nq) : 0;
    fixed xs = FMul(q->x,s),	ys = FMul(q->y,s),	zs = FMul(q->z,s);
    fixed wx = FMul(q->w,xs),	wy = FMul(q->w,ys),	wz = FMul(q->w,zs);
    fixed xx = FMul(q->x,xs),	xy = FMul(q->x,ys),	xz = FMul(q->x,zs);
    fixed yy = FMul(q->y,ys),	yz = FMul(q->y,zs),	zz = FMul(q->z,zs);
    
//     M->m[0][0] = 4096 - (yy + zz);
//     M->m[0][1] = xy - wz;
//     M->m[0][2] = xz + wy;
//     
//     M->m[1][0] = xy + wz;
//     M->m[1][1] = 4096 - (xx + zz);
//     M->m[1][2] = yz - wx;
//     
//     M->m[2][0] = xz - wy;
//     M->m[2][1] = yz + wx;
//     M->m[2][2] = 4096 - (xx + yy);
    
    M->m[0][0] = 4096 - (yy + zz);
    M->m[1][0] = xy - wz;
    M->m[2][0] = xz + wy;
    
    M->m[0][1] = xy + wz;
    M->m[1][1] = 4096 - (xx + zz);
    M->m[2][1] = yz - wx;
    
    M->m[0][2] = xz - wy;
    M->m[1][2] = yz + wx;
    M->m[2][2] = 4096 - (xx + yy);


//    M.m[3][0]=
//    M.m[3][1]=
//    M.m[3][2]=
//    M.m[0][3]=
//    M.m[1][3]=
//    M.m[2][3]=0;
    M->t[0]=
    M->t[1]=
    M->t[2]=0;

	
	//bbtest

	//invert x axis
 	M->m[0][0] = -M->m[0][0];
 	M->m[1][0] = -M->m[1][0];
 	M->m[2][0] = -M->m[2][0];

	//invert y axis
// 	M->m[0][1] = -M->m[0][1];
// 	M->m[1][1] = -M->m[1][1];
// 	M->m[2][1] = -M->m[2][1];

	//invert z axis
 	M->m[0][2] = -M->m[0][2];
 	M->m[1][2] = -M->m[1][2];
 	M->m[2][2] = -M->m[2][2];

//    M.m[3][3]=4096;
}

void CreateLevelObjects(unsigned long worldID,unsigned long levelID)
{
	short i;
	char tmp[5];
	int flags;

	ACTOR2 *theActor;
	SCENIC *cur = firstScenic;


	char *compare;

	/*int actCount = 0;
	*/
	// go through and add items
	while (cur)
	{
		utilPrintf("%s, %d, %d, %d\n", cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz);
		if ( (gstrcmp ( cur->name,"world.psi") != 0) && (gstrcmp ( cur->name,"backdrop.psi") != 0) )
		{
			for( i=0; i<4; i++ )
				tmp[i] = cur->name[i];
			tmp[4] = '\0';

			if ( ( gstrcmp ( tmp,"cam_\0") != 0 ) )
			{
				if ( ( gstrcmp ( tmp,"wat_\0") ==0 ) || ( gstrcmp ( tmp,"slu_\0") ==0 ) )
				{
					flags = 0;

					//if ( cur->name[4] == 'f' )
						//flags = ACTOR_SLIDYTEX;
					// ENDIF

					CreateAndAddWaterObject ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, flags );

				}
				else if ( ( compare = strstr ( cur->name, "xx_" ) ) || ( compare = strstr ( cur->name, "lea_" ) ) ||
									( compare = strstr ( cur->name, "xxa_" ) ) || ( compare = strstr ( cur->name, "spl_" ) ) || ( compare = strstr ( cur->name, "wh" ) ) || ( compare = strstr ( cur->name, "wat_" ) )|| ( compare = strstr ( cur->name, "slu_" ) ))
				{
					flags = 0;

					if ( ( gstrcmp ( cur->name, "wat_" ) != 0 ) )
						flags |= TRANSPARENT;
					// ENDIF

					if ( ( gstrcmp ( cur->name, "xx_gflo.psi" ) != 0 ) )
						flags |= TRANSPARENT | ADDATIVE;
					// ENDIF

					if ( ( gstrcmp ( cur->name, "xx_g_flake.psi" ) == 0 ) )
						flags |= SHIFT_DEPTH;
					// ENDIF
					if ( ( gstrcmp ( cur->name, "xx_g_fblake.psi" ) == 0 ) )
						flags |= SHIFT_DEPTH;
					// ENDIF
					if ( ( gstrcmp ( cur->name, "xx_g_fcroc.psi" ) == 0 ) )
						flags |= SHIFT_DEPTH;
					// ENDIF

					if ( ( gstrcmp ( cur->name, "xxa_whlite.psi" ) == 0 ) )
						flags |= SHIFT_DEPTH;
					// ENDIF

					if ( ( compare = strstr ( cur->name, "_g" ) ) )
					{
						flags |= 0;
					}
					// ENDIF

					if ( ( compare = strstr ( cur->name, "_f" ) ) )
					{
						flags |= ACTOR_SLIDYTEX;
					}
					// ENDIF

					CreateAndAddScenicObject ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, flags );
				}
				else
				{
					theActor = CreateAndAddActor ( cur->name, cur->pos.vx, cur->pos.vy, cur->pos.vz, INIT_ANIMATION, 0, 0 );

					actorAnimate ( theActor->actor, 0, YES, NO, 150, NO );
				}
				// ENDELSEIF
			}
			// ENDIF

		}
		// ENDIF
		/*fixed tv;

		stringChange(ts->name);

		if(gstrcmp(ts->name,"backdrop.obe") == 0)
		{
			backGnd = CreateAndAddActor(ts->name, ToFixed(ts->pos.vx), ToFixed(ts->pos.vz), ToFixed(ts->pos.vy), INIT_ANIMATION, ToFixed(0), 0);
			backGnd->actor->scale.vx = ToFixed(5);
			backGnd->actor->scale.vy = ToFixed(5);
			backGnd->actor->scale.vz = ToFixed(5);
			actList = actList->next;
			if (actList)
				actList->prev = NULL;
		}
		else
		{*/



			/*if(gstrcmp(ts->name,"world.obe") == 0)
			{
				theActor->flags = ACTOR_DRAW_ALWAYS;
				globalLevelActor = theActor;
			}

			for( i=0; i<4; i++ )
				tmp[i] = ts->name[i];
			tmp[4] = '\0';
			// If a water object, draw always
			if( !gstrcmp( tmp, "wat_\0" ) )
			{
				theActor->flags = ACTOR_WATER | ACTOR_DRAW_ALWAYS;

				if (ts->name[4]=='f')
					theActor->flags |= ACTOR_SLIDYTEX;

#ifdef N64_VERSION
				AddN64WaterObjectResource(theActor->actor);
#endif
			}

			tv = ToFixed(ts->rot.y);
			ts->rot.y = ts->rot.z;
//bb
//			ts->rot.z = tv/4096;
			ts->rot.z = tv/4096.0F;

			GetQuaternionFromRotation (&theActor->actor->qRot,&ts->rot);

			actorAnimate(theActor->actor,0,YES,NO,1433/16,0);
			if(ts->name[0] == 'a')
			{
				fixed rMin,rMax,rNum;
				if(ts->name[2] == '_')
				{
					rMin = ToFixed(ts->name[1] - 30);
					if(rMin == ToFixed(0)) 
						rMin = ToFixed(10);
					rMin /= 10;
					actorAnimate(theActor->actor,0,YES,NO,rMin>>4, 0);
				}
				else if(ts->name[3] == '_')
				{
					rMin = ToFixed(ts->name[1] - 30);
					if(rMin == ToFixed(0)) 
						rMin = ToFixed(10);
					rMin /= 10;
				
					rMax = ToFixed(ts->name[1] - 30);
					if (rMax == ToFixed(0)) 
						rMax = ToFixed(10);
					rMax /= 10;

					rNum = ToFixed(Random(1000));
					rNum= rMin + FDiv( FMul(rNum,(rMax-rMin)), ToFixed(1000) );
										
					actorAnimate(theActor->actor,0,YES,NO,rNum>>4, 0);
				}
			}
			actCount++;
		}
*/
		cur = cur->next;

	}
	// ENDWHILE

//	dprintf"\n\n** ADDED %d ACTORS **\n\n",actCount));
}
