/*
	This file is part of Frogger II, Copyright 2000 Interactive Studios Ltd

	File        : backdrop.c
	Description : Backdrop routines and structures
	Date        : 09/03/2000
	Version     : 0.01
*/

#include "include.h"

// -------
// Globals

BACKDROP 		backDrop;
KMSTRIPCONTEXT	backStripContext;
KMSTRIPHEAD		backStripHead;
KMVERTEX_03		backdropVertices[] =
{
{ KM_VERTEXPARAM_NORMAL,                  0,  BACKDROP_HEIGHT, 1.0f, 0.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,                  0,                0, 1.0f, 0.0f, 0.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_NORMAL,     BACKDROP_WIDTH,  BACKDROP_HEIGHT, 1.0f, 1.0f, 1.0f, RGBA(255,255,255,255), 0 },
{ KM_VERTEXPARAM_ENDOFSTRIP, BACKDROP_WIDTH,                0, 1.0f, 1.0f, 0.0f, RGBA(255,255,255,255), 0 }
};

/*	--------------------------------------------------------------------------------
	Function 	: InitBackdrop
	Purpose 	: Load and setup a background imageg
	Parameters 	: filename
	Returns 	: none
	Info 		:
*/

void InitBackdrop(char * const filename)
{
	int				i,x,y;
	char			buf[256],*headerPtr,*ptr;
    PKMDWORD   	 	TexturePtr,TwiddledPtr;
  	USHORT			width,height,*pixelPtr,pixel,newPixel;
  	int				headerSize,colFormat,texFormat,texAttrib,attrib;
	char			r,g,b,a;

	backDrop.draw = TRUE;

	if(backDrop.init)
		return;

	sprintf(buf,"%s.pvr",filename);
	
	backDrop.init = TRUE;
	backDrop.rect.x = 0;			// leave the y till the update
	backDrop.rect.y = 0;
	backDrop.rect.w = 640; 
	backDrop.rect.h = 480;

	// load the backdrop into a VRAM surface
	loadPVRFileIntoSurface(buf, "backdrops", &backDrop.surface, FALSE);
	backDrop.imageXD = backDrop.surface.u0.USize;
	backDrop.imageYD = backDrop.surface.u1.VSize;

	// setup (u,v) coordinates vertices
	backdropVertices[0].fU = 0;
	backdropVertices[0].fV = 384.0/512;	
	
	backdropVertices[1].fU = 0;
	backdropVertices[1].fV = 0;	
	
	backdropVertices[2].fU = 1.0;
	backdropVertices[2].fV = 384.0/512;
	
	backdropVertices[3].fU = 1.0;
	backdropVertices[3].fV = 0;
	
	// initialise strip context and head
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &backStripContext);
	memset(&backStripContext,0,sizeof(backStripContext));
	memset(&backStripHead,0,sizeof(backStripHead));
	backStripContext.nSize = sizeof(KMSTRIPCONTEXT);
    kmInitStripContext(KM_STRIPCONTEXT_SYS_GOURAUD | KM_OPAQUE_POLYGON, &backStripContext);
	backStripContext.StripControl.nListType		 					= KM_OPAQUE_POLYGON;
	backStripContext.StripControl.nUserClipMode	 					= KM_USERCLIP_DISABLE;
	backStripContext.StripControl.nShadowMode			 			= KM_NORMAL_POLYGON;
	backStripContext.StripControl.bGouraud		 					= KM_TRUE;
	backStripContext.ObjectControl.nDepthCompare			 		= KM_GREATER;
	backStripContext.ObjectControl.nCullingMode			 			= KM_NOCULLING;
	backStripContext.ObjectControl.bZWriteDisable					= KM_TRUE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nSRCBlendingMode	= KM_ONE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nDSTBlendingMode	= KM_ZERO;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bSRCSelect		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bDSTSelect		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nFogMode			= KM_NOFOG;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bColorClamp		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].bUseAlpha		= KM_FALSE;
	backStripContext.ImageControl[KM_IMAGE_PARAM1].nFilterMode		= KM_BILINEAR;
    backStripContext.ImageControl[KM_IMAGE_PARAM1].pTextureSurfaceDesc = &backDrop.surface;
	kmGenerateStripHead03(&backStripHead,&backStripContext);
}

/*	--------------------------------------------------------------------------------
	Function 	: DrawBackDrop
	Purpose 	: Draw the current background image to the screen
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void DrawBackDrop(int num, int i)
{
	int	fogFade;
	
	fogFade = 100;

//	kmBeginScene(&kmSystemConfig);
//	kmBeginPass(&vertexBufferDesc);
        
	if((backDrop.init)&&(backDrop.draw))
	{
		kmStartStrip(&vertexBufferDesc, &backStripHead);	
		kmSetVertex(&vertexBufferDesc, &backdropVertices[0], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &backdropVertices[1], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));
		kmSetVertex(&vertexBufferDesc, &backdropVertices[2], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmSetVertex(&vertexBufferDesc, &backdropVertices[3], KM_VERTEXTYPE_03, sizeof(KMVERTEX_03));	
		kmEndStrip(&vertexBufferDesc);
	}	
	
//	kmEndPass(&vertexBufferDesc);
				
//	kmRender(KM_RENDER_FLIP);
//	kmEndScene(&kmSystemConfig);
}

/*	--------------------------------------------------------------------------------
	Function 	: FreeBackdrop
	Purpose 	: Free the current background image
	Parameters 	: none
	Returns 	: none
	Info 		:
*/

void FreeBackdrop(void)
{
	if(backDrop.draw == FALSE)
		return;
		
	backDrop.draw = FALSE;
//	kmFreeTexture(&backDrop.surface);
}