#ifndef SYSPROCESS_H_INCLUDED
#define SYSPROCESS_H_INCLUDED

#ifdef WIN32
#include <windows.h>

#ifndef _MT
#error Must include multi-threaded libraries!
#endif

class mutex
{
private:
	CRITICAL_SECTION sync;	// single-process mutex for Win32

public:
	mutex();
	~mutex();

	void open();
	void close();
//	bool try_open();
};

class thread
{
private:
	unsigned long _hdl;
	unsigned _id;
	static unsigned __stdcall _threadproc(void*);

protected:
	virtual int run() = 0;

public:
	thread() : _hdl(0) { }
	~thread();

	void start();
	void stop(int code = 0);
	void pause();
	void resume();
	void wait();
	void wait(long millis);

	static void sleep(long millis);	// causes the _current_ thread to sleep
};

#endif // WIN32

#endif // sysprocess