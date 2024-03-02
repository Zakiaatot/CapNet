#include <process.h>
#include "CapNetThread.h"

CapNetThread::CapNetThread()
	:hThread_(NULL),
	threadId_(0),
	autoDelete_(false)
{
}

CapNetThread::~CapNetThread()
{
	if (hThread_ != NULL)
		KillThread();
}

bool CapNetThread::Start()
{
	hThread_ = (HANDLE)_beginthreadex(0, 0, ThreadFun, this, 0, &threadId_);
	return hThread_ != NULL;
}

unsigned __stdcall CapNetThread::ThreadFun(void* p)
{
	CapNetThread* pObj = (CapNetThread*)p;
	pObj->Run();
	if (pObj->autoDelete_)
		delete pObj;
	return 0;
}

void CapNetThread::Wait(DWORD timeout)
{
	WaitForSingleObject(hThread_, timeout);
}

void CapNetThread::SetAutoDelete()
{
	autoDelete_ = true;
}