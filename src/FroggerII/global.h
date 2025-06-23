/* 
*	Global configuration file
*	
*	06-23-2025  atmossig	
*/

#ifndef _GLOBAL_H
#define _GLOBAL_H

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