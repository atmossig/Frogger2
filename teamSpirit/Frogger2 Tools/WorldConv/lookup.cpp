#include "lookup.h"


*	-----------------------------------------------------------------------
	LookupEntry
*/

class LookupEntry
{
	char *name;
	void *thing;
	//TokenType *params;
	//int numParams;

	LookupEntry *left, *right;

	LookupEntry(const char* name, void *whatever);
	~LookupEntry();

	void Add(LookupEntry*);

	friend class Lookup;
};

LookupEntry::LookupEntry(const char* n, void *t)
{
	name = new char[strlen(n) + 1];
	strcpy(name, n);
	strupr(name);
	thing = t;

	left = NULL; right = NULL;
}

LookupEntry::~LookupEntry()
{
	delete [] name;

	if (left) delete left;
	if (right) delete right;
}

Lookup::Lookup()
{
	list = NULL;
}

Lookup::~Lookup()
{
	if (list) delete list;
}

void *Lookup::GetEntry(const char *name)
{
	LookupEntry *e = list;
	int len = strlen(name);
	char *str = new char[len+1];
	
	strcpy(str, name);
	strupr(str);
	
	while (e)
	{
		int res = strcmp(str, e->name);

		if (res == 0)	// we've found the entry! return it.
		{
			delete [] str;
			return e->thing;
		}
		else if (res > 0)	
			e = e->right;	// name is alphabetically higher, search RIGHT tree
		else
			e = e->left;	// search LEFT tree
	}
	
	return 0;
}



bool Lookup::AddEntry(const char *name, void *thing)
{
	LookupEntry *l = list, *e = new LookupEntry(name, thing);

	if (!list)
		list = e;
	else while(1)
	{
		int r = strcmp(e->name, l->name);

		if (r == 0)		// there is already an entry with this name
		{
			delete e;
			return false;
		}
		if (r < 0)		// new entry is alphabetically lower, so store on LEFT
		{
			if (l->left)
				l = l->left;
			else
			{
				l->left = e; break;
			}
		}
		else			// alphabetically higher, store on RIGHT
		{
			if (l->right)
				l = l->right;
			else
			{
				l->right = e; break;
			}
		}
	}

	return true;
}

/*	----------------------------------------------------------------------- */