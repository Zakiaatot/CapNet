#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <atlstr.h>
#include "Parse.h"
#include "CapNetListenThread.h"



CapNetListenThread::CapNetListenThread
(
	pcap_t* adhandle,
	const WCHAR* type,
	const WCHAR* ip,
	CapNetCore::LISTEN_CALLBACK_FUNC pCallback,
	CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback
) :
	kill_(FALSE),
	adhandle_(adhandle),
	isRunning_(FALSE),
	errMsg_(),
	type_(type),
	ip_(ip),
	pCallback_(pCallback),
	pEndCallback_(pEndCallback),
	packMap_()
{
}


CapNetListenThread::~CapNetListenThread()
{

}

VOID CapNetListenThread::Kill()
{
	TerminateThread(GetHandle(), 0);
	Wait();
}


void CapNetListenThread::Run()
{
	isRunning_ = TRUE;
	/* Retrieve the packets */
	int res = 0;
	pcap_pkthdr* header = NULL;
	const UCHAR* pkt_data;
	time_t local_tv_sec;
	struct tm* ltime = NULL;
	char timestr[64];
	UINT i = 0;
	while ((res = pcap_next_ex(adhandle_, &header, &pkt_data)) >= 0) {
		if (kill_)
		{
			res = 0;
			break;
		}
		if (res == 0)
			/* Timeout elapsed */
			continue;

		packMap_[i] = std::make_pair(header, pkt_data);

		/* convert the timestamp to readable format */
		local_tv_sec = header->ts.tv_sec;
		ltime = localtime(&local_tv_sec);
		strftime(timestr, sizeof timestr, "%H:%M:%S", ltime);
		strcat_s(timestr, ":");
		CHAR* p = &timestr[strlen(timestr)];
		wsprintfA(p, "%.6d", header->ts.tv_usec);

		CapNetCore::Pack pack = { 0 };
		pack.no = i;
		CA2W ca2w(timestr);
		pack.time = std::wstring(ca2w);
		pack.length = header->len;

		auto sd = Parse::GetSourceDestIp(pkt_data);
		pack.source = sd.first;
		pack.dest = sd.second;

		auto protocolAndInfo = Parse::GetProtocolAndInfo(pkt_data);
		pack.protocol = protocolAndInfo.first;
		pack.info = protocolAndInfo.second;
		if (pCallback_)
			pCallback_(pack);
		i++;
	}

	if (res == -1) {
		CHAR buf[PCAP_ERRBUF_SIZE] = { 0 };
		wsprintfA(buf, "[CapNetCore] Error reading the packets: %s\n", pcap_geterr(adhandle_));
		CA2W ca2w(buf);
		errMsg_ = std::wstring(ca2w);
		pEndCallback_(errMsg_);
	}
	else if (res == 0)
	{
		//  ÷∂ØΩ· ¯
	}
	else
	{
		errMsg_ = L"[CapNetCore] Unknown Error";
		pEndCallback_(errMsg_);
	}
	isRunning_ = FALSE;
}
