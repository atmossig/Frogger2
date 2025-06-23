/*
*	Original Author: Matthew Cloy
* 
*	File: mdxInfo.h
*	Programmer: atmossig (re-programmed from the ground up)
*	Date: 06-23-2025
* 
*	Description:
*		DX12 info.
*/

#ifndef MDXINFO_H_
#define MDXINFO_H_

// Easier instead of #ifdef __cplusplus
//
// No worries about including 'global.h'
// since it is force included with the rest of the project
// in C++ > Advanced
BEGIN_C

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

// Show capabilities of the adapter device.
void d3d12ShowCaps(IDXGIAdapter4* pAdapter);

// Convert D3D12 error.
void d3d12ShowError(HRESULT error);

END_C

#endif // MDXINFO_H_