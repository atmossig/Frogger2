/* Buffer.h */

#ifndef _INCLUDED_BUFFER_H
#define _INCLUDED_BUFFER_H

class Buffer
{
	char* p;
	int size, alloc;

public:
	Buffer();
	~Buffer();
	void Clear();
	void AddData(const void *data, int size);
	void AddChar(char);
	void AddUchar(unsigned char);
	void AddWord(int);
	void AddInt(int);
	void AddString(const char*);
	void AddFloat(float);
	void CopyData(void **data);
	void Append(Buffer &b);
	inline int Size()	{ return size; };
	inline const void *Data()	{ return p; };
};

#endif