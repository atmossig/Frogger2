#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "buffer.h"

BUFFER* MakeBuffer(void)
{
	BUFFER *b = (BUFFER*)malloc(sizeof(BUFFER));
	b->p = (unsigned char*)malloc(100);
	b->size = 0; b->alloc = 100;
	return b;
}

void FreeBuffer(BUFFER*b)
{
	if (b)
	{
		free(b->p);
		free(b);
	}
}

void AddToBuffer(const void *data, int size, BUFFER *buffer)
{
	unsigned char* newBuffer;

	if (buffer->size + size > buffer->alloc)
	{
		buffer->alloc *= 2;
		newBuffer = (unsigned char*)malloc(buffer->alloc);
		memcpy(newBuffer, buffer->p, buffer->size);
		free(buffer->p);
		buffer->p = newBuffer;
	}

	memcpy(buffer->p + buffer->size, data, size);
	buffer->size += size;
}
