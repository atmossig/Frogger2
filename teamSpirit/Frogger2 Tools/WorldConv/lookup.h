#ifndef _INCLUDED_LOOKUP_H
#define _INCLUDED_LOOKUP_H

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

#endif