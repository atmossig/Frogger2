/* Buffer.h */

#ifndef _INCLUDED_BUFFER_H
#define _INCLUDED_BUFFER_H

typedef struct TAGBUFFER {
	unsigned char* p;
	int size, alloc;
} BUFFER;


BUFFER* MakeBuffer(void);
void FreeBuffer(BUFFER*b);
void AddToBuffer(const void *data, int size, BUFFER *buffer);

#endif