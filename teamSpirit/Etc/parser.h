/* Parser */

#ifndef _INCLUDED_PARSER_H
#define _INCLUDED_PARSER_H

#include "type.h"

extern char token[80];
extern TokenType tokenType;
extern const char* tokenNames[];

bool OpenFile(const char* filename);
void CloseCurrFile();
void CloseAllFiles();
int NextToken();
const char *GetStringToken(void);
bool GetNumberToken(double *);
bool GetIntegerToken(int *);

void Error(const char* message);
const char* CurrentFilename();
const int CurrentLineNum();

void SetVariable(const char* key, const char* value);

extern char *includePath;

#endif