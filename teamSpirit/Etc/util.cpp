#include <stdio.h>
#include <string.h>
#include "util.h"

VarTableEntry::VarTableEntry(const char *v)
{
	value = NULL;
	SetValue(v);
}

VarTableEntry::~VarTableEntry()
{
	delete [] value;
}

void VarTableEntry::SetValue(const char* v)
{
	if (value)
		delete [] value;

	if (v)
	{
		value = new char[strlen(v) + 1];
		strcpy(value, v);
	}
	else
		value = NULL;
}

void VarTableEntry::link(VarTableEntry *&list)
{
	next = list;
	list = this;
}

CharSet::CharSet(const char *chars)
{
	for (char *c = (char*)chars; *c; c++) table[unsigned char(*c)] = 1;
}

/*	-----------------------------------------------------------------------
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

ScriptTokenList::ScriptTokenList()
{
	list = NULL;
}

bool ScriptTokenList::AddEntry(const char* name, const char* define, int token, const ParamType *params, int numParams)
{
	ScriptToken *t;

	t = new ScriptToken;

	if (!lookup.AddEntry(name, (void*)t))
	{
		delete t;
		return false;
	}

	t->token = token;
	t->params = new ParamType[numParams + 1];
	memcpy(t->params, params, numParams * sizeof(ParamType));
	t->params[numParams] = 0;

	if (define)
	{
		int len = strlen(define);
		t->define = new char[len + 1];
		memcpy(t->define, define, len);
		t->define[len] = 0;
	}
	else
		t->define = NULL;
	
	t->next = list;
	list = t;

	return true;
}

ScriptTokenList::~ScriptTokenList()
{
	ScriptToken *n, *t = list;

	for (t = list; t; t = n)
	{
		n = t->next;
		delete [] t->params;
		delete [] t->define;
		delete t;
	}
}

ScriptToken *ScriptTokenList::GetEntry(const char* name)
{
	return (ScriptToken*)lookup.GetEntry(name);
}


void ScriptTokenList::OutputDefines(FILE *f)
{
	ScriptToken *t;

	for (t = list; t; t = t->next)
	{
		if (t->define) fprintf(f, "#define %s %d\n", t->define, t->token);
	}
}

/*	----------------------------------------------------------------------- */
// Returns the path of the given filename
// i.e. the string up to and including the last '\' or '/'.

void GetPath(char *path, const char *filename)
{
	char *c;
	int i, length = 0;
	for (c = (char*)filename, i = 0; *c; c++)
	{
		i++;
		if (*c == '\\' || *c == '/') length = i;
	}
	
	memcpy(path, filename, length);
	path[length] = 0;
}


// Returns the complete filename and path, without the extension
// i.e. the string up to but NOT including the last '.'.

void GetFilenameStart(char *str, const char *filename)
{
	char *c;
	int i, length = 0;
	for (c = (char*)filename, i = 0; *c; c++)
	{
		if (*c == '.') length = i;
		i++;
	}
	
	memcpy(str, filename, length);
	str[length] = 0;
}


