#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <atlstr.h>
#include "CapNetParse.h"
#include "CapNetListenThread.h"



CapNetListenThread::CapNetListenThread
(
	pcap_t* adhandle,
	CapNetCore::LISTEN_CALLBACK_FUNC pCallback,
	CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback
) :
	kill_(FALSE),
	adhandle_(adhandle),
	isRunning_(FALSE),
	errMsg_(),
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

std::vector<BYTE> CapNetListenThread::GetRawData(UINT pacId)
{
	return  packMap_[pacId].second;
}

CapNetCore::PacDetailTree CapNetListenThread::GetDetailTree(UINT pacId)
{
	if (packMap_.find(pacId) == packMap_.end())
		return NULL;
	std::vector<BYTE> dataVec = packMap_[pacId].second;
	BYTE* buf = new BYTE[dataVec.size()];
	int j = 0;
	for (auto i = dataVec.begin(); i != dataVec.end(); i++)
		buf[j++] = *i;
	return CapNetDetailParse::CreataDetailTree(buf);
}

std::string CapNetListenThread::Save(PCWCHAR filePath)
{
	CW2A cw2a(filePath);
	pcap_dumper_t* dumpfile = pcap_dump_open(adhandle_, cw2a);

	if (dumpfile == NULL)
	{
		return "[CapNetCore] Error opening output file";
	}

	for (auto i = packMap_.begin(); i != packMap_.end(); i++)
	{
		BYTE* dataBuf = new UCHAR[i->second.second.size()]{ 0 };
		INT index = 0;
		for (auto j = i->second.second.begin(); j != i->second.second.end(); j++)
		{
			dataBuf[index++] = *j;
		}
		pcap_dump((UCHAR*)dumpfile, &i->second.first, dataBuf);
		delete[] dataBuf;
	}
	pcap_dump_close(dumpfile);
	return "";
}

void CapNetListenThread::Run()
{
	isRunning_ = TRUE;
	/* Retrieve the packets */
	INT res = 0;
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

		std::vector<BYTE> rawData;
		for (INT i = 0; i < (INT)header->caplen; i++)
		{
			rawData.push_back(pkt_data[i]);
		}
		packMap_[i] = std::make_pair(*header, rawData);

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
		pack.length = header->caplen;

		auto sd = CapNetParse::GetSourceDestIp(pkt_data);
		pack.source = sd.first;
		pack.dest = sd.second;

		auto protocolAndInfo = CapNetParse::GetProtocolAndInfo(pkt_data);
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
		if (pEndCallback_)
			pEndCallback_(errMsg_);
	}
	else if (res == 0)
	{
		// 手动结束
	}
	std::wstring msg = L"退出";
	if (pEndCallback_)
		pEndCallback_(msg);
	isRunning_ = FALSE;
}
