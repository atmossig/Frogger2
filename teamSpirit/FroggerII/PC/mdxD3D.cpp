
unsigned long d3DInit(hardware)
{
	if (pDirectDraw->QueryInterface(IID_IDirect3D2, (LPVOID *)&pDirect3D) != S_OK)
	return FALSE;

	// Find a device we can use

	DDINIT(search);
	DDINIT(result);
	search.dwFlags = /*D3DFDS_HARDWARE |*/ D3DFDS_COLORMODEL;
//	search.bHardware = TRUE;
	search.dcmColorModel = D3DCOLOR_RGB;
	
	if (pDirect3D->FindDevice(&search, &result) != D3D_OK) 
		return FALSE;

	// Create the D3D device
	if (pDirect3D->CreateDevice(result.guid, surface[HIDDEN_SRF], &pDirect3DDevice) != D3D_OK)
	 return FALSE;

	// Create a viewport onto the device
	DDINIT (viewport);
	viewport.dwWidth = rXRes;
	viewport.dwHeight = rYRes;
	viewport.dvScaleX = (rXRes / 2.0f);
	viewport.dvScaleY = (rYRes / 2.0f);
	viewport.dvMaxX = D3DVAL(1.0);
	viewport.dvMaxY = D3DVAL(1.0);
	viewport.dvMinZ = D3DVAL(0.0);
	viewport.dvMaxZ = D3DVAL(1.0);

	if (pDirect3D->CreateViewport(&pDirect3DViewport, NULL) != D3D_OK) 
		return FALSE;

	if (pDirect3DDevice->AddViewport(pDirect3DViewport) != D3D_OK) 
		return FALSE;

	if (pDirect3DViewport->SetViewport(&viewport) != D3D_OK) 
		return FALSE;

	if (pDirect3DDevice->SetCurrentViewport(pDirect3DViewport) != D3D_OK) 
		return FALSE;
}