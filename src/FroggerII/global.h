/* 
*	Global configuration file
*	
*	06-23-2025  atmossig	
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H

// __cplusplus def (easier) instead of #ifdef __cplusplus and all that.
#define BEGIN_C extern "C" {
#define END_C }

// Libraries we need
//
// d3d12.lib		For DirectX 12 of course.
// d3dcompiler.lib	For shader compiling.
// dxgi.lib			For Misc.
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxgi.lib")

// Platform definitions
#if defined(__WIN32) || defined(_WIN64)
#define PC_VERSION
#elif defined(__linux__)
#define LINUX_VERSION
#elif defined(__APPLE__)
#define MAC_VERSION
#elif defined(__ANDROID__)
#define ANDROID_VERSION
#endif

#endif // _GLOBAL_H