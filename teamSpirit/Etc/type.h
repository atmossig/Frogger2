#ifndef _INCLUDED_TYPE_H
#define _INCLUDED_TYPE_H

#ifndef BYTE
typedef unsigned char BYTE;
#endif

enum ParamTypeEnum
{
	PARAM_NONE = 0,
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_STRING,
	PARAM_TRIGGER,
	PARAM_BLOCK,
	NUMPARAMTYPE
};

typedef BYTE ParamType;

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

struct COMMAND_TABLE
{
	const char *str;
	int token;
	ParamType params[8];
};


#endif