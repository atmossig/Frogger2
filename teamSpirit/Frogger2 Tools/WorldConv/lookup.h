#ifndef _INCLUDED_LOOKUP_H
#define _INCLUDED_LOOKUP_H

class LookupEntry;

class Lookup
{
	LookupEntry *list;
public:
	Lookup();
	~Lookup();

	bool AddEntry(const char* name, unsigned long whatever);
	unsigned long GetEntry(const char *name);
};

#endif