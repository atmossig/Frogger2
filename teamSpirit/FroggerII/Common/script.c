

#include "ultra64.h"
#include "incs.h"
#include "codes.h"

/* --------------------------------------------------------------------------------- */

#define MEMGETBYTE(p) (*((*p)++))

inline int MEMGETINT(UBYTE **p)		// get a little-endian integer
{
	unsigned int i;

	i = (unsigned int)*((*p)++);
	i += ((unsigned int)*((*p)++) << 8);
	i += ((unsigned int)*((*p)++) << 16);
	i += ((unsigned int)*((*p)++) << 24);

	return i;
}

#define MEMGETFLOAT(p) ((float)MEMGETINT(p) / (float)0x10000)

inline char *MemLoadString(UBYTE **p)
{
	char *ptr;
	int size;
	
	size = MEMGETBYTE(p);
	ptr = JallocAlloc(size + 1, NO, "str");
	memcpy(ptr, *p, size);
	ptr[size] = 0;

	(*p) += size;

	return ptr;
}

/* --------------------------------------------------------------------------------- */

BOOL ExecuteCommand(UBYTE command. UBYTE *buffer)
{
	UBYTE *p = buffer;

	switch (command)
	{
	case EV_DEBUG:
		{
			char string[80];
			int len;

			len = MEMGETBYTE(&p);
			memcpy(string, p, len);
			string[len] = 0;

			dprintf"[Interpreter Debug] %s\n", string));
		}
		break;
	}
}


int Interpret(UBYTE *buffer)
{
	UBYTE *p, *q;
	int events, size;
	UBYTE command;

	p = buffer;

	events = MEMGETINT(&p);

	while (events--)
	{
		size = MEMGETINT(p);
		q = p;
		command = MEMGETBYTE(q);
		if (!ExecuteCommand(command, q)) return 0;
		p += size;
	}

	return 1;
}

