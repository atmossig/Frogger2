/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.

	File		: editgfx.cpp
	Programmer	: David Swift
	Date		: 30/06/99

----------------------------------------------------------------------------------------------- */



/*	--------------------------------------------------------------------------------
	Function		: DrawEditWindow
	Purpose			: draw the background (+frame?) for editor windows
	Parameters		: 
	Returns			: 
	Info			: 
*/

void DrawEditWindow(RECT r)
{
	D3DLVERTEX v[4] = {
		{
			r.left,r.top,0,0,
			TOOLBAR_COLOR,D3DRGB(0,0,0),
			0,0
		},
		{
			r.left,r.bottom,0,0,
			TOOLBAR_COLOR,D3DRGB(0,0,0),
			0,0
		},
		{
			r.right,r.bottom,0,0,
			TOOLBAR_COLOR,D3DRGB(0,0,0),
			0,0
		},
		{
			r.right,r.top,0,0,
			TOOLBAR_COLOR,D3DRGB(0,0,0),
			0,0
	}};

	if (h!=NULL)
	{
		pDirect3DDevice->SetRenderState(D3DRENDERSTATE_TEXTUREHANDLE,NULL);
		lastH = NULL;
	}

	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,0);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	pDirect3DDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,0);


	if (pDirect3DDevice->DrawPrimitive(
		D3DPT_TRIANGLEFAN,
		D3DVT_TLVERTEX,
		v,
		4,
		D3DDP_DONOTCLIP 
			| D3DDP_DONOTLIGHT 
			| D3DDP_DONOTUPDATEEXTENTS 
			| D3DDP_WAIT)!=D3D_OK)
	{
		dp("Could not draw edit window\n");
		// BUGGER !!!!! CAN'T DRAW EDIT WINDOW!
	}

}