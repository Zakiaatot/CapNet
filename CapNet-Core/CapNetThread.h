#ifndef _CAPNET_THREAD_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
class CapNetThread
{
public:
	CapNetThread();
	virtual ~CapNetThread();

	virtual void Run() = 0;
	bool Start();
	void Wait(DWORD timeout = INFINITE);
	void SetAutoDelete();

private:
	HANDLE hThread_;
	unsigned threadId_;
	bool autoDelete_;
	static unsigned __stdcall ThreadFun(void* p);
};
#endif // !_CAPNET_THREAD_H_


