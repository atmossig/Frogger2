#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "buffer.h"

Buffer::Buffer()
{
	size = 0;
	alloc = 100;
	p = new char[alloc];
}

Buffer::~Buffer()
{
	delete [] p;
}

void Buffer::Clear()
{
	size = 0;	// set "size" to zero, but leave memory allocated to be reused. Efficient!
}

void Buffer::AddData(const void *data, int s)
{
	char* newBuffer;

	if (size + s > alloc)
	{
		alloc *= 2;
		newBuffer = new char[alloc];
		memcpy(newBuffer, p, size);
		delete [] p;
		p = newBuffer;
	}

	memcpy(p + size, data, s);
	size += s;
}

void Buffer::CopyData(void **data)
{
	char *foo = new char[size];
	memcpy(foo, p, size);
	*data = (void*)foo;
}

void Buffer::Append(Buffer &b)
{
	AddData(b.p, b.size);
}

void Buffer::AddString(const char *str)
{
	unsigned char len = strlen(str);

	AddData(&len, 1);
	AddData(str, len);
}

void Buffer::AddChar(char c)
{
	AddData(&c, 1);
}

void Buffer::AddInt(int value)
{
	unsigned char foo[4];

	foo[0] = (value & 0xff);			// ALWAYS store little-endian regardless of platform
	foo[1] = ((value >> 8) & 0xff);
	foo[2] = ((value >> 16) & 0xff);
	foo[3] = ((value >> 24) & 0xff);

	AddData(foo, 4);
}

void Buffer::AddFloat(float value)
{
	int v = (int)(value * (float)0x10000);
	AddInt(v);
}

