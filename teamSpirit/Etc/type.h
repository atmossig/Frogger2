#ifndef _INCLUDED_TYPE_H
#define _INCLUDED_TYPE_H

#ifndef BYTE
typedef unsigned char BYTE;
#endif

typedef enum { PARAM_NONE = 0, PARAM_INT, PARAM_FLOAT, PARAM_STRING } PARAMTYPES;

typedef BYTE PARAMTYPE;

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

#endif