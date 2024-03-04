#ifndef _CAPNET_LISTEN_THREAD_H_
#define _CAPNET_LISTEN_THREAD_H_

#include <pcap.h>
#include <string>
#include <map>
#include <vector>
#include "CapNetCore.h"
#include "CapNetThread.h"

class CapNetListenThread :public CapNetThread
{
public:
	explicit CapNetListenThread
	(
		pcap_t* adhandle,
		CapNetCore::LISTEN_CALLBACK_FUNC pCallback,
		CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback
	);
	~CapNetListenThread();
	BOOL Running() const { return isRunning_; }
	VOID Kill();
	std::vector<BYTE>  GetRawData(UINT pacId);
private:
	BOOL kill_;
	pcap_t* adhandle_;
	std::wstring errMsg_;
	CapNetCore::LISTEN_CALLBACK_FUNC pCallback_;
	CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback_;
	std::map <UINT, std::pair<pcap_pkthdr, std::vector<BYTE>>> packMap_;
	void Run();
public:
	BOOL isRunning_;
};

#endif // !_CAPNET_LISTEN_THREAD_H_


