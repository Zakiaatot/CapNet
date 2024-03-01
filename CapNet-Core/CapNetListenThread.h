#ifndef _CAPNET_LISTEN_THREAD_H_
#define _CAPNET_LISTEN_THREAD_H_

#include <pcap.h>
#include <string>
#include <map>
#include "CapNetCore.h"
#include "CapNetThread.h"

class CapNetListenThread :public CapNetThread
{
public:
	CapNetListenThread
	(
		pcap_t* adhandle,
		const WCHAR* type,
		const WCHAR* ip,
		CapNetCore::LISTEN_CALLBACK_FUNC pCallback
	);
	~CapNetListenThread();

private:
	pcap_t* adhandle_;
	BOOL isRunning_;
	std::wstring errMsg_;
	const WCHAR* type_;
	const WCHAR* ip_;
	CapNetCore::LISTEN_CALLBACK_FUNC pCallback_;
	std::map <UINT, std::pair<pcap_pkthdr*, const UCHAR*>> packMap_;
	void Run();
};

#endif // !_CAPNET_LISTEN_THREAD_H_


