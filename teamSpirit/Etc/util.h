#ifndef _UTIL_H_INCLUDED
#define _UTIL_H_INCLUDED

#include "type.h"

/*	----------------------------------------------------------------------- */

class VarTableEntry
{
public:
	char *name, *value;
	VarTableEntry *link;

	VarTableEntry(const char* n, const char *v = 0);
	~VarTableEntry();
	void SetValue(const char*);
};


/*	----------------------------------------------------------------------- */

class LookupEntry;

class Lookup
{
	LookupEntry *list;
public:
	Lookup();
	~Lookup();

	bool AddEntry(const char* name, void* whatever);
	void* GetEntry(const char *name);
};

/*	----------------------------------------------------------------------- */

struct ScriptToken
{
	int token;
	ParamType *params;
	ScriptToken *next;
};

class ScriptTokenList
{
	ScriptToken *list;
	Lookup lookup;

public:
	ScriptTokenList();
	~ScriptTokenList();
	
	bool AddEntry(const char* name, int token, const ParamType *params, int n);
	ScriptToken *GetEntry(const char* name);
};

/*	----------------------------------------------------------------------- */

class CharSet
{
	unsigned char table[256];
public:
	CharSet(const char *chars);
	inline bool is_member(char c) { return (table[(unsigned char)c] != 0); }
};

/*	----------------------------------------------------------------------- */

void GetPath(char *path, const char *filename);
void GetFilenameStart(char *str, const char *filename);

/*	----------------------------------------------------------------------- */

#endif