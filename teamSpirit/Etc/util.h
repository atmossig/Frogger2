#ifndef _UTIL_H_INCLUDED
#define _UTIL_H_INCLUDED

enum TokenType
{
	T_NONE = 0,
	T_NUMBER,
	T_STRING,
	T_VARIABLE,
	T_COMMAND,
	T_SYMBOL,
	NUMTOKENTYPE
};


class VarTableEntry
{
public:
	char *name, *value;
	VarTableEntry *link;

	VarTableEntry(const char* n, const char *v = 0);
	~VarTableEntry();
	void SetValue(const char*);
};


class LookupEntry;

class Lookup
{
	LookupEntry *list;
public:
	Lookup();
	~Lookup();

	void AddEntry(char* name, int token, TokenType* params = 0, int nt = 0);
	int GetToken(char *name);
};


class CharSet
{
	unsigned char table[256];
public:
	CharSet(const char *chars);
	inline bool is_member(char c) { return (table[(unsigned char)c] != 0); }
};


#endif