#include <string.h>
#include "util.h"

VarTableEntry::VarTableEntry(const char* n, const char *v)
{
	if (n)
	{
		name = new char[strlen(n) + 1];
		strcpy(name, n);
	}
	if (v)
	{
		value = new char[strlen(v) + 1];
		strcpy(value, v);
	}
}

VarTableEntry::~VarTableEntry()
{
	delete [] name; delete [] value;
}

void VarTableEntry::SetValue(const char* v)
{
	if (v)
	{
		value = new char[strlen(v) + 1];
		strcpy(value, v);
	}
	else
		value = NULL;
}



CharSet::CharSet(const char *chars)
{
	for (char *c = (char*)chars; *c; c++) table[unsigned char(*c)] = 1;
}

class LookupEntry
{
	char *name;
	int token;
	TokenType *params;
	int numParams;

	LookupEntry *next;

	LookupEntry(char* name, int token, TokenType* params = 0, int nt = 0);
	~LookupEntry();

	friend class Lookup;
};

LookupEntry::LookupEntry(char* n, int t, TokenType *p, int num)
{
	name = new char[strlen(n) + 1];
	strcpy(name, n);
	strupr(name);
	token = t;
	numParams = num;

	params = new TokenType[num + 1];
	if (num) memcpy(params, p, sizeof(TokenType)*num);
	params[num] = T_NONE;
}

LookupEntry::~LookupEntry()
{
	delete [] name;
	delete [] params;
}

Lookup::Lookup()
{
	list = NULL;
}

Lookup::~Lookup()
{
	LookupEntry *e, *n;

	for (e = list; e; e = n)
	{
		n = e->next;
		delete e;
	}
}

int Lookup::GetToken(char *name)
{
	strupr(name);
	for (LookupEntry *e = list; e; e = e->next)
		if (!strcmp(name, e->name)) return e->token;
	
	return 0;
}