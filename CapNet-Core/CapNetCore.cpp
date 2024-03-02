#pragma comment(lib,"wpcap.lib")

#define DLLEXPORT
#include <map>
#include <pcap.h>
#include "CapNetCore.h"
#include "CapNetListenThread.h"


BOOL gInit = FALSE;
std::map<INT, pcap_if_t*> gDevMap;
CapNetListenThread* pListenThread_ = NULL;
pcap_t* gAdhandle = NULL;


CapNetCore::StatusVoid CapNetCore::LoadNpcapDlls()
{
	WCHAR npcapDir[512];
	UINT len;
	len = GetSystemDirectory(npcapDir, 480);
	if (!len) {
		return StatusVoid::Err("[CapNetCore] Error in GetSystemDirectory: %x", GetLastError());
	}
	wcscat_s(npcapDir, 512, L"\\Npcap");
	if (SetDllDirectory(npcapDir) == 0) {
		return StatusVoid::Err("[CapNetCore] Error in SetDllDirectory: %x", GetLastError());
	}
	gInit = TRUE;
	return StatusVoid::Ok();
}

CapNetCore::StatusVoid CapNetCore::Init()
{
	return LoadNpcapDlls();
}


CapNetCore::Status<std::vector<std::pair<int, std::string>>> CapNetCore::FindAllDev()
{
	if (!gInit)
	{
		return Status<std::vector<std::pair<int, std::string>>>::Err("[CapNetCore] Not Init");
	}
	pcap_if_t* d = NULL;
	CHAR errbuf[PCAP_ERRBUF_SIZE];
	if (pcap_findalldevs(&d, errbuf) == -1)
	{
		return Status<std::vector<std::pair<int, std::string>>>::Err("[CapNetCore] Error in pcap_findalldevs: %s\n", errbuf);
	}
	std::vector<std::pair<int, std::string>> ret;
	CHAR buf[PCAP_ERRBUF_SIZE] = { 0 };
	INT i = 0;
	FreeAllDev();
	for (d; d; d = d->next)
	{
		gDevMap[i] = d;
		if (d->description)
			sprintf_s(buf, "%d. %s\n", i, d->description);
		else
			sprintf_s(buf, "%d. No description available\n", i);
		ret.push_back({ i,buf });
		ZeroMemory(buf, PCAP_ERRBUF_SIZE);
		i++;
	}
	return Status<std::vector<std::pair<int, std::string>>>::Ok(ret);
}

CapNetCore::StatusVoid CapNetCore::FreeAllDev()
{
	if (!gInit)
	{
		return StatusVoid::Err("[CapNetCore] Not Init");
	}
	pcap_if_t* allDev = NULL;
	if (allDev = gDevMap[0])
	{
		pcap_freealldevs(allDev);
		gDevMap.clear();
	}
	return StatusVoid::Ok();
}

CapNetCore::StatusVoid CapNetCore::BeginListen
(
	INT devId,
	std::wstring type,
	std::wstring ip,
	CapNetCore::LISTEN_CALLBACK_FUNC pCallback,
	CapNetCore::LISTEN_END_CALLBACK_FUNC pEndCallback
)
{
	if (!gInit)
	{
		return StatusVoid::Err("[CapNetCore] Not Init");
	}
	pcap_if_t* dev = gDevMap[devId];
	if (!dev)
	{
		return StatusVoid::Err("[CapNetCore] Error Dev Select");
	}
	if (pListenThread_)
	{
		if (pListenThread_->Running())
		{
			return StatusVoid::Err("[CapNetCore] Another Listening Thread is Running");
		}
		else
		{
			delete pListenThread_;
			pListenThread_ = NULL;
		}
	}
	/* Open the adapter */
	if (gAdhandle)
	{
		pcap_close(gAdhandle);
	}
	CHAR errbuf[PCAP_ERRBUF_SIZE];
	if ((gAdhandle = pcap_open_live(dev->name,	// name of the device
		65536,			// portion of the packet to capture. 
		// 65536 grants that the whole packet will be captured on all the MACs.
		1,				// promiscuous mode (nonzero means promiscuous)
		100,			// read timeout
		errbuf			// error buffer
	)) == NULL)
	{
		return StatusVoid::Err("[CapNetCore] Unable to open the adapter. %s is not supported by Npcap.", dev->description);
	}

	pListenThread_ = new CapNetListenThread(gAdhandle, type, ip, pCallback, pEndCallback);
	if (!pListenThread_->Start())
	{
		delete pListenThread_;
		pListenThread_ = NULL;
		return StatusVoid::Err("[CapNetCore] Start Listen Thread Error.");
	}

	return StatusVoid::Ok();
}

CapNetCore::StatusVoid CapNetCore::EndListen()
{
	if (pListenThread_)
	{
		pListenThread_->Kill();
	}
	delete pListenThread_;
	pListenThread_ = NULL;

	return StatusVoid();
}
