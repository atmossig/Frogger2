#ifndef _ETC_H_INCLUDED
#define _ETC_H_INCLUDED

//#include "codes.h"
#include "type.h"

#define C_ON		0x40		// special!
#define C_CALL		0x7F		// special!

#define	C_INCLUDE	0x100
#define C_SET		0x101
#define C_SUB		0x102
	
const char* PARAMNAMESTRING[] = { NULL, "integer", "float", "string", "trigger", "block" };

#endif
