#include <windows.h>
#include <process.h>
#include "sysprocess.h"

// ------------------------------ class thread ------------------------------

thread::~thread()
{
	wait();
}

void thread::start()
{
	if (_hdl == NULL)
		_hdl = ::_beginthreadex(NULL, 0, thread::_threadproc, this, 0, &_id);
}

unsigned __stdcall thread::_threadproc(void* p)
{
	thread &t = *((thread*)p);
	_endthreadex((unsigned)t.run());
	return 0;
}

void thread::stop(int code)	{ TerminateThread((HANDLE)_hdl, code); }
void thread::pause()		{ SuspendThread((HANDLE)_hdl); }
void thread::resume()		{ ResumeThread((HANDLE)_hdl); }
void thread::wait()			{ WaitForSingleObject((HANDLE)_hdl, INFINITE); }
void thread::wait(long millis) { WaitForSingleObject((HANDLE)_hdl, millis); }

void thread::sleep(long millis) { Sleep(millis); }

// ------------------------------ class mutex ------------------------------

mutex::mutex() { InitializeCriticalSection(&sync); }
mutex::~mutex() { DeleteCriticalSection(&sync); }
void mutex::open()	{ EnterCriticalSection(&sync); }
void mutex::close()	{ LeaveCriticalSection(&sync); }

//bool mutex::try_open()	{ return TryEnterCriticalSection(&sync); }
