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
	char* define;
	ScriptToken *next;
};

class ScriptTokenList
{
	ScriptToken *list;

public:
	Lookup lookup;

	ScriptTokenList();
	~ScriptTokenList();
	
	bool AddEntry(const char* name, const char* define, int token, const ParamType *params, int n);
	ScriptToken *GetEntry(const char* name);
	void OutputDefines(FILE *);
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