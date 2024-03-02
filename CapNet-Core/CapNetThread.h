#ifndef _CAPNET_THREAD_H_

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
class CapNetThread
{
public:
	CapNetThread();
	~CapNetThread();
	virtual void Run() = 0;
	bool Start();
	void Wait(DWORD timeout = INFINITE);
	void SetAutoDelete();
	HANDLE GetHandle()const { return hThread_; }

private:
	HANDLE hThread_;
	unsigned threadId_;
	bool autoDelete_;
	static unsigned __stdcall ThreadFun(void* p);
	VOID KillThread() const { CloseHandle(hThread_); }
};
#endif // !_CAPNET_THREAD_H_


