#ifndef __MAIN_H
#define __MAIN_H


#define SPIRIT	1

#define PC		1
#define N64		2

extern int userFormat;
extern int user;


void RegisterWindowClasses();
int MessageLoop();
LRESULT CALLBACK MainWndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL FAR PASCAL mainDlgProc(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
void CleanUp();
BOOL AmITheOnlyUser();
void SetUserFace(HWND hwnd);
BOOL FAR PASCAL OnlyUser(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
void InitArrays();
void SetupProgressBar(int start, int end, char *);
void StepProgressBar();
void KillProgressBar();
BOOL FAR PASCAL Config(HWND hwnd, int msg, WPARAM wParam, LPARAM lParam);
void FreeGlobals();


extern char userName[];
extern char includeFiles[10][MAX_PATH];
extern char numIncludeFiles;


extern HWND dlgWnd;
extern HWND hTreeWnd;
extern HINSTANCE hAppInst;
extern HWND hMainWnd;
extern HWND hWorkingWnd;

extern RECT objectTextureRect;
extern RECT textureRect;


#endif
