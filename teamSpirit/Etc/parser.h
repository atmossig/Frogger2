/* Parser */

#ifndef _INCLUDED_PARSER_H
#define _INCLUDED_PARSER_H


bool OpenFile(const char* filename);
void CloseCurrFile();
void CloseAllFiles();

extern char token[80];
extern TokenType tokenType;

#endif