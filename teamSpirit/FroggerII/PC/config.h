/*

	This file is part of Frogger2, (c) 1999 Interactive Studios Ltd.


	File		: config.h
	Programmer	: 
	Date		: 

----------------------------------------------------------------------------------------------- */

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

//

typedef struct TAG_CONFIG
{
	char *name;
	char *type;
	void *var;
} CONFIG;

extern void GetArgs(char *arglist);
extern void ReadConfigFile (void);

//

#ifdef __cplusplus
}
#endif

#endif