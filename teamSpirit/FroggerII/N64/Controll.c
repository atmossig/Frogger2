/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: Controll.c
	Programmer	: Andrew Eder
	Date		: 11/9/98

----------------------------------------------------------------------------------------------- */


#include <ultra64.h>

#include "incs.h"



OSMesgQueue	controllerMsgQ;
OSMesg		controllerMsgBuf[8];
OSMesgQueue     serialMsgQ;
OSMesg          serialMsg;

int initControllers(void);

OSPfs	rumble;

OSContStatus    statusdata[MAXCONTROLLERS];
OSContPad       controllerdata[MAXCONTROLLERS];
unsigned long	playerInputPause;
unsigned long	playerInputPause2;

static u16		lastbutton;

int				ControllerMode = 0;
char			debugCtrlMode[20];

extern char UseAAMode;
extern char useFilt;

unsigned long	INPUT_POLLPAUSE = 0;


/*	--------------------------------------------------------------------------------
	Function		: initControllers()
	Purpose			: init. controllers...
	Parameters		: none
	Returns			: lowest number controller connected to system
	Info			:
*/

int initControllers()
{
    int             i;
    u8              pattern;
	short res;

    osCreateMesgQueue(&serialMsgQ, &serialMsg, 1);
    osSetEventMesg(OS_EVENT_SI, &serialMsgQ, (OSMesg)1);

    osContInit(&serialMsgQ, &pattern, &statusdata[0]);

    osCreateMesgQueue(&controllerMsgQ, controllerMsgBuf, 8);
    osSetEventMesg(OS_EVENT_SI, &controllerMsgQ, (OSMesg)0);
	Wait(10000);

	playerInputPause = INPUT_POLLPAUSE;
	playerInputPause2 = INPUT_POLLPAUSE;

	res = osMotorInit (&controllerMsgQ, &rumble, 0 );

    
	if ( res == 0 )
		dprintf"Ya Baby\n"));
	else
		dprintf"No Baby\n"));
	// ENDIF

    for(i=0; i<MAXCONTROLLERS; i++)
	{
		if((pattern & (1<<i)) && !(statusdata[i].errno & CONT_NO_RESPONSE_ERROR))
			return i;
    }


	return -1;
}

/*	--------------------------------------------------------------------------------
	Function		: ResetParameters()
	Purpose			: reset program settings to defaults
	Parameters		: none
	Returns			: none
	Info			:
*/

void ResetParameters()
{
	int i;

	ScreenWidthDelta    = 0;

	UseUCode            = 4;
	OutLen              = RDP_OUTPUT_BUF_LEN;
  
	for (i=0; i<4; i++)
	{
		currCamSource[i].v[0] = 0;	currCamSource[i].v[1] = 0;	currCamSource[i].v[2] = -100;
		currCamTarget[i].v[0] = 0;	currCamTarget[i].v[1] = 0;	currCamTarget[i].v[2] = 0;
	}

	SetVector(&camVect,&cameraUpVect);
	camera.rot.v[0]		= 0;	
	camera.rot.v[1]		= 0;	
	camera.rot.v[2]		= 0;

	ShadingMode         = LIGHTING;
	UseGlobalTransforms = 0;

	UseTextureMode		= 1;
	UseAAMode			= 0;
	UseZMode			= 1;
	UseWireframeMode	= 0;
}

/*	--------------------------------------------------------------------------------
	Function		: ReadController0()
	Purpose			: reads controller 0....
	Parameters		: none
	Returns			: none
	Info			:
*/

void ReadControllerData()
{
/*	static u16 button;
	static s16 stickX, stickY;      
    
	(void)osRecvMesg(&controllerMsgQ, NULL, OS_MESG_BLOCK);
	osContGetReadData(controllerdata);*/
}

/*	--------------------------------------------------------------------------------
	Function		: ReadController1()
	Purpose			: reads controller 1....
	Parameters		: none
	Returns			: none
	Info			: this controller mode used for viewing animations during object viewing
*/

void ReadDebugPad()
{
	static u16 button;
	static s16 stickX, stickY;      
    
	//(void)osRecvMesg(&controllerMsgQ, NULL, OS_MESG_BLOCK);
	//osContGetReadData(controllerdata);

	button = controllerdata[1].button;
	stickX = controllerdata[1].stick_x;
	stickY = controllerdata[1].stick_y;

	if(stickX > 15)
    {
		camDist.v[1]+=5;
	}	
	else if(stickX < -15)
    {	
		camDist.v[1]-=5;
    }

	if(stickY > 15)
    {
		camDist.v[2]+=5;
    }
	else if(stickY < -15)
    {
		camDist.v[2]-=5;
	}

	if(button & CONT_UP)
    {
		camLookOfs+=5;
	}
	    
	if(button & CONT_DOWN)
    {
		camLookOfs-=5;
	}

	if((button & CONT_LEFT) && !(lastbutton & CONT_LEFT))
    {
		camDist.v[0]-=5;
	}

	if((button & CONT_RIGHT) && !(lastbutton & CONT_RIGHT))
    {
		camDist.v[0]+=5;
	}

	if((button & CONT_E) && !(lastbutton & CONT_E))
    {
    }

	if(button & CONT_D)
    {
		ShowJalloc();
	}

	if(button & CONT_F)
    {
	}

	if(button & CONT_C)
    {
	}

	if((button & CONT_G) && !(lastbutton & CONT_G))
    {
	}

	if((button & CONT_A) && !(lastbutton & CONT_A))
    {
	}

	if((button & CONT_B) && !(lastbutton & CONT_B))
    {
		if(autoHop)
			autoHop = 0;
		else
			autoHop = 10000;
	}

	if((button & CONT_L) && !(lastbutton & CONT_L))
    {
#ifndef ROM_BUILD
		if(UseWireframeMode)
			UseWireframeMode = 0;
		else
			UseWireframeMode = 1;
#endif
	}

	if((button & CONT_R) && !(lastbutton & CONT_R))
    {
		if(UseAAMode)
			UseAAMode = 0;
		else
			UseAAMode = 2;
	}

	if((button & CONT_START) && !(lastbutton & CONT_START))
	{
		timerMode = !timerMode;
	}
  
	lastbutton = button;
}

/*	--------------------------------------------------------------------------------
	Function		: ReadController()
	Purpose			: select which controller is currently being used
	Parameters		: none
	Returns			: none
	Info			:
*/

void ReadController()
{
	ReadControllerData();
	ReadDebugPad();

	if(playerInputPause)
		playerInputPause--;
	if(playerInputPause2)
		playerInputPause2--;
}
