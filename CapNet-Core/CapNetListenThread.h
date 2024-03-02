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
	explicit CapNetListenThread
	(
		pcap_t* adhandle,
		std::wstring  type,
		std::wstring  ip,
		CapNetCore::LISTEN_CALLBACK_FUNC pCallback,
		CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback
	);
	~CapNetListenThread();
	BOOL Running() const { return isRunning_; }
	VOID Kill();
private:
	BOOL kill_;
	pcap_t* adhandle_;
	BOOL isRunning_;
	std::wstring errMsg_;
	std::wstring type_;
	std::wstring  ip_;
	CapNetCore::LISTEN_CALLBACK_FUNC pCallback_;
	CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback_;
	std::map <UINT, std::pair<pcap_pkthdr*, const UCHAR*>> packMap_;
	void Run();
};

#endif // !_CAPNET_LISTEN_THREAD_H_


